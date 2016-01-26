// fstest.cc
//    Simple test routines for the file system.
//
//    We implement:
//       Copy -- copy a file from UNIX to Nachos
//       Print -- cat the contents of a Nachos file
//       Perftest -- a stress test for the Nachos file system
//        read and write a really large file in tiny chunks
//        (won't work on baseline system!)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "utility.h"
#include "filesys.h"
#include "system.h"
#include "thread.h"
#include "disk.h"
#include "stats.h"


#define TransferSize     10     // make it small, just to be difficult
#define SHELL_BUFFER_SIZE 1<<16
#define SHELL_ARGS_SIZE 1<<16

void Print(char *name);
void Copy(const char *from, const char *to);
void PerformanceTest();
static void FileWrite();
static void FileRead();

void parse_args(char *buffer, char** args, size_t args_size, size_t *nargs)
{
    char *buf_args[args_size];
    char **cp;
    char *wbuf;
    size_t i, j;

    wbuf=buffer;
    buf_args[0]=buffer;
    args[0] =buffer;

    for(cp=buf_args; (*cp=strsep(&wbuf, " \n\t")) != NULL ;){
        if ((*cp != '\0') && (++cp >= &buf_args[args_size]))
            break;
    }

    for (j=i=0; buf_args[i]!=NULL; i++){
        if(strlen(buf_args[i])>0)
            args[j++]=buf_args[i];
    }

    *nargs=j;
    args[j]=NULL;
}

void prompt() {
    printf("nachos %s $ ", fileSystem->GetWorkingDirectory());
}

void test_prompt(const char * cmd) {
    prompt();
    printf("%s\n", cmd);
}

void show_help() {
    printf("\nCommandes disponibles :\n");
    printf("exit\n");
    printf("ls [<path>]\n");
    printf("cd [<path>]\n");
    printf("touch <filename>\n");
    printf("print <filename>\n");
    printf("cp <src> <dest>\n");
    printf("rm <path>\n");
    printf("mkdir <dirname>\n");
    printf("mkdir -p <path>\n");
    printf("pwd\n");
    printf("test  : lance les tests\n");
}

void run_test() {
    fileSystem = new FileSystem (true);
    char * test_args[2];
    test_args[0] = new char[SHELL_BUFFER_SIZE];
    test_args[1] = new char[SHELL_BUFFER_SIZE];

    test_prompt("ls");
    fileSystem->List();

    test_prompt("mkdir foo");
    strcpy(test_args[0], "foo");
    fileSystem->CreateDir(test_args[0]);

    test_prompt("ls foo");
    strcpy(test_args[0], "foo");
    fileSystem->List (test_args[0]);

    test_prompt("mkdir foo/bar");
    strcpy(test_args[0], "foo/bar");
    fileSystem->CreateDir(test_args[0]);

    test_prompt("cd foo/bar");
    strcpy(test_args[0], "foo/bar");
    fileSystem->ChangeDir(test_args[0]);

    test_prompt("touch zero");
    strcpy(test_args[0], "zero");
    fileSystem->Create(test_args[0], 0);

    test_prompt("print zero");
    strcpy(test_args[0], "zero");
    Print (test_args[0]);

    test_prompt("cp ../filesys/test/small small");
    strcpy(test_args[0], "../filesys/test/small");
    strcpy(test_args[1], "small");
    Copy(test_args[0], test_args[1]);

    test_prompt("print small");
    strcpy(test_args[0], "small");
    Print (test_args[0]);

    test_prompt("cp ../filesys/test/big big");
    strcpy(test_args[0], "../filesys/test/big");
    strcpy(test_args[1], "big");
    Copy(test_args[0], test_args[1]);

    test_prompt("ls");
    fileSystem->List();

    test_prompt("ls big");
    strcpy(test_args[0], "big");
    fileSystem->List(test_args[0]);

    test_prompt("cd ..");
    strcpy(test_args[0], "..");
    fileSystem->ChangeDir(test_args[0]);

    test_prompt("rm bar");
    strcpy(test_args[0], "bar");
    fileSystem->Remove (test_args[0]);

    test_prompt("rm bar/zero");
    strcpy(test_args[0], "bar/zero");
    fileSystem->Remove(test_args[0]);

    test_prompt("rm bar/small");
    strcpy(test_args[0], "bar/small");
    fileSystem->Remove(test_args[0]);

    test_prompt("rm bar/big");
    strcpy(test_args[0], "bar/big");
    fileSystem->Remove (test_args[0]);

    test_prompt("ls bar");
    strcpy(test_args[0], "bar");
    fileSystem->List (test_args[0]);

    test_prompt("rm bar");
    strcpy(test_args[0], "bar");
    fileSystem->Remove(test_args[0]);

    test_prompt("rm .");
    strcpy(test_args[0], ".");
    fileSystem->Remove(test_args[0]);

    test_prompt("rm .");
    strcpy(test_args[0], ".");
    fileSystem->Remove(test_args[0]);

    test_prompt("mkdir -p I/create/recursive/directory");
    strcpy(test_args[0], "I/create/recursive/directory");
    fileSystem->CreateFatherDir(test_args[0]);

    test_prompt("cd I/create/recursive/directory");
    strcpy(test_args[0], "I/create/recursive/directory");
    fileSystem->ChangeDir(test_args[0]);

    test_prompt("pwd");
    printf("%s\n", fileSystem->GetWorkingDirectory());

    test_prompt("cd");
    fileSystem->MoveToRoot();
}


void ShellFileSys() 
{
    char buffer[SHELL_BUFFER_SIZE];
    size_t nargs;
    char *args[SHELL_ARGS_SIZE];
    show_help();
    fileSystem = new FileSystem (true);
    while(1) {
        prompt();
        if (fgets(buffer, SHELL_BUFFER_SIZE, stdin) == NULL) break;
        parse_args(buffer, args, SHELL_ARGS_SIZE, &nargs);

        if (nargs==0) continue;
        else if (!strcmp(args[0], "exit" )) break;
        else if (!strcmp(args[0], "ls" ) && (nargs == 1)) {
            fileSystem->List();
        }
        else if (!strcmp(args[0], "ls" ) && (nargs == 2)) {
            fileSystem->List(args[1]);
        }
        else if (!strcmp(args[0], "cd") && (nargs == 1) ) {
            fileSystem->MoveToRoot();
        }
        else if (!strcmp(args[0], "cd") && (nargs == 2) ) {
            fileSystem->ChangeDir(args[1]);
        }
        else if (!strcmp(args[0], "touch") && (nargs == 2) ) {
            fileSystem->Create(args[1], 0);
        }
        else if (!strcmp(args[0], "print") && (nargs == 2) ) {
            Print(args[1]);
        }
        else if (!strcmp(args[0], "cp") && (nargs == 3) ) {
            Copy(args[1], args[2]);
        }
        else if (!strcmp(args[0], "rm") && (nargs == 2) ) {
            fileSystem->Remove(args[1]);
        }
        else if (!strcmp(args[0], "mkdir") && (nargs == 2) ) {
            fileSystem->CreateDir(args[1]);
        }
        else if (!strcmp(args[0], "mkdir") && (nargs == 3) ) {
            if (!strcmp(args[1], "-p"))
                fileSystem->CreateFatherDir(args[2]);
            else
                show_help();
        }
        else if (!strcmp(args[0], "pwd") && (nargs == 1) ) {
            printf("%s\n", fileSystem->GetWorkingDirectory());
        }
        else if (!strcmp(args[0], "format")) {
            fileSystem = new FileSystem(true);
        }
        else if (!strcmp(args[0], "test")) {
            // on lance quelques tests
            run_test();
        }
        else {
            show_help();
        }
    }

    printf("\nBye\n");
    interrupt->Halt();
}


//----------------------------------------------------------------------
// Copy
// Copy the contents of the UNIX file "from" to the Nachos file "to"
//----------------------------------------------------------------------
void Copy(const char *from, const char *to) {
    FILE *fp;
    OpenFile* openFile;
    int amountRead, fileLength;
    char *buffer;

    // Open UNIX file
    if ((fp = fopen(from, "r")) == NULL) {
        printf("Copy: couldn't open input file %s\n", from);
        return;
    }

    // Figure out length of UNIX file
    fseek(fp, 0, 2);
    fileLength = ftell(fp);
    fseek(fp, 0, 0);

    // Create a Nachos file of the same length
    DEBUG('f', "Copying file %s, size %d, to file %s\n", from, fileLength, to);
    char *paths[MaxDepth];
    int npath;
    fileSystem->parse_path((char *)to, paths, &npath);
    if(npath == 1)
    {
		if (!fileSystem->Create(to, fileLength)) 
		{     // Create Nachos file
		    printf("Copy: couldn't create output file %s\n", to);
		    fclose(fp);
		    return;
		}

		openFile = fileSystem->Open(to);
		ASSERT(openFile != NULL);

		// Copy the data in TransferSize chunks
		buffer = new char[TransferSize];
		while ((amountRead = fread(buffer, sizeof(char), TransferSize, fp)) > 0)
		openFile->Write(buffer, amountRead);
		delete [] buffer;

		// Close the UNIX and the Nachos files
		delete openFile;
    	fclose(fp);
    }else
    {
    	Directory *directory = new Directory(NumDirEntries);
    	directory->FetchFrom(fileSystem->GetdirectoryFile());
    	char *dirName = new char[DirNameMaxLen];
		int i;
		for(i =0; i< npath - 1; i++)
		{
			if(i == 0)
				strcpy(dirName, paths[i]);
			else
			{
				strcat(dirName, (char *)"/");
				strcat(dirName, paths[i]);
			}
			DEBUG('j',"%s : %s\n",paths[i], dirName);
		}
		char *curName = directory->GetDirName();
		fileSystem->ChangeDir(dirName);
		directory = new Directory(NumDirEntries);
    	directory->FetchFrom(fileSystem->GetdirectoryFile());
    	to  = paths[i];
    	if (!fileSystem->Create(to, fileLength)) 
		{     // Create Nachos file
		    printf("Copy: couldn't create output file %s\n", to);
		    fclose(fp);
		    return;
		}

		openFile = fileSystem->Open(to);
		ASSERT(openFile != NULL);

		// Copy the data in TransferSize chunks
		buffer = new char[TransferSize];
		while ((amountRead = fread(buffer, sizeof(char), TransferSize, fp)) > 0)
		openFile->Write(buffer, amountRead);
		delete [] buffer;

		// Close the UNIX and the Nachos files
		delete openFile;
		fileSystem->ChangeDir(curName);
    	fclose(fp);
    }
}

//----------------------------------------------------------------------
// Print
// Print the contents of the Nachos file "name".
//----------------------------------------------------------------------

void Print(char *name)
{
    OpenFile *openFile;
    int i, amountRead;
    char *buffer;

    if ((openFile = fileSystem->Open(name)) == NULL) {
    printf("Print: unable to open file %s\n", name);
    return;
    }

    buffer = new char[TransferSize];
    while ((amountRead = openFile->Read(buffer, TransferSize)) > 0)
    for (i = 0; i < amountRead; i++)
        printf("%c", buffer[i]);
    delete [] buffer;

    delete openFile;        // close the Nachos file
    return;
}

//----------------------------------------------------------------------
// PerformanceTest
//     Stress the Nachos file system by creating a large file, writing
//    it out a bit at a time, reading it back a bit at a time, and then
//    deleting the file.
//
//    Implemented as three separate routines:
//      FileWrite -- write the file
//      FileRead -- read the file
//      PerformanceTest -- overall control, and print out performance #'s
//----------------------------------------------------------------------

#define FileName     "TestFile"
#define Contents     "1234567890"
#define ContentSize     strlen(Contents)
#define FileSize     ((int)(ContentSize * 5000))

static void FileWrite() {
    OpenFile *openFile;
    int i, numBytes;

    printf("Sequential write of %d byte file, in %zd byte chunks\n",
    FileSize, ContentSize);
    if (!fileSystem->Create(FileName, 0)) {
      printf("Perf test: can't create %s\n", FileName);
      return;
    }
    openFile = fileSystem->Open(FileName);
    if (openFile == NULL) {
        printf("Perf test: unable to open %s\n", FileName);
        return;
    }
    for (i = 0; i < FileSize; i += ContentSize) {
        numBytes = openFile->Write(Contents, ContentSize);
        if (numBytes < 10) {
            printf("Perf test: unable to write %s\n", FileName);
            delete openFile;
            return;
        }
    }
    delete openFile;    // close file
}

static void
FileRead()
{
    OpenFile *openFile;
    char *buffer = new char[ContentSize];
    int i, numBytes;

    printf("Sequential read of %d byte file, in %zd byte chunks\n",
    FileSize, ContentSize);

    if ((openFile = fileSystem->Open(FileName)) == NULL) {
        printf("Perf test: unable to open file %s\n", FileName);
        delete [] buffer;
        return;
    }
    for (i = 0; i < FileSize; i += ContentSize) {
        numBytes = openFile->Read(buffer, ContentSize);
        if ((numBytes < 10) || strncmp(buffer, Contents, ContentSize)) {
            printf("Perf test: unable to read %s\n", FileName);
            delete openFile;
            delete [] buffer;
            return;
        }
    }
    delete [] buffer;
    delete openFile;    // close file
}

void
PerformanceTest()
{
    printf("Starting file system performance test:\n");
    stats->Print();
    FileWrite();
    FileRead();
    if (!fileSystem->Remove((char *)FileName)) {
      printf("Perf test: unable to remove %s\n", FileName);
      return;
    }
    stats->Print();
}

