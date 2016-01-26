// fstest.cc 
//	Simple test routines for the file system.  
//
//	We implement:
//	   Copy -- copy a file from UNIX to Nachos
//	   Print -- cat the contents of a Nachos file 
//	   Perftest -- a stress test for the Nachos file system
//		read and write a really large file in tiny chunks
//		(won't work on baseline system!)
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

#define TransferSize 	10 	// make it small, just to be difficult

#define MaxLenCmd 300

#define MaxArg 5

//----------------------------------------------------------------------
// Copy
// 	Copy the contents of the UNIX file "from" to the Nachos file "to"
//----------------------------------------------------------------------

void
Copy(const char *fromARG,const char *toARG)
{
	char *from = (char*)fromARG;
	char *to = (char*)toARG;
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
    if (!fileSystem->Create(to, fileLength)) {	 // Create Nachos file
		DEBUG('f', "dsfxgfghhjlkmmjhkhgqhdfkhqfdhsGFDHGFSHdfHQDFHKFdkfdkfkDFK\n");
		printf("Copy: couldn't create output file %s\n", to);
		fclose(fp);
		return;
    }
    
    
    openFile = fileSystem->Open(to);
    ASSERT(openFile != NULL);
     
   
// Copy the data in TransferSize chunks
    buffer = new char[TransferSize];
    DEBUG('w', "dsfxgfghhjlkmmjhkhgqhdfkhqfdhsGFDHGFSHdfHQDFHKFdkfdkfkDFK\n");
    while ((amountRead = fread(buffer, sizeof(char), TransferSize, fp)) > 0)
		openFile->Write(buffer, amountRead);
	DEBUG('w', "dsfxgfghhjlkmmjhkhgqhdfkhqfdhsGFDHGFSHdfHQDFHKFdkfdkfkDFK\n");
    delete [] buffer;

// Close the UNIX and the Nachos files
    delete openFile;
    fclose(fp);
    
}

//----------------------------------------------------------------------
// Print
// 	Print the contents of the Nachos file "name".
//----------------------------------------------------------------------

void
Print(char *name)
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

    delete openFile;		// close the Nachos file
    return;
}

//----------------------------------------------------------------------
// PerformanceTest
// 	Stress the Nachos file system by creating a large file, writing
//	it out a bit at a time, reading it back a bit at a time, and then
//	deleting the file.
//
//	Implemented as three separate routines:
//	  FileWrite -- write the file
//	  FileRead -- read the file
//	  PerformanceTest -- overall control, and print out performance #'s
//----------------------------------------------------------------------

#define FileName 	"TestFile"
#define Contents 	"1234567890"
#define ContentSize 	strlen(Contents)
#define FileSize 	((int)(ContentSize * 5000))

static void 
FileWrite()
{
	const char *fileNa = FileName;
    OpenFile *openFile;    
    int i, numBytes;

    printf("Sequential write of %d byte file, in %zd byte chunks\n", 
	FileSize, ContentSize);
	
    if (!fileSystem->Create((char*)fileNa, 0)) 
    {
		printf("Perf test: can't create %s\n", fileNa);
		return;
    }
    openFile = fileSystem->Open((char*)fileNa);
    if (openFile == NULL) 
    {
		printf("Perf test: unable to open %s\n", fileNa);
		return;
    }
    for (i = 0; i < FileSize; i += ContentSize) 
    {
        numBytes = openFile->Write(Contents, ContentSize);
		if (numBytes < 10) 
		{
			printf("Perf test: unable to write %s\n", fileNa);
			delete openFile;
			return;
		}
    }
    delete openFile;	// close file
}

static void 
FileRead()
{
	const char *fileNa = FileName;
    OpenFile *openFile;    
    char *buffer = new char[ContentSize];
    int i, numBytes;

    printf("Sequential read of %d byte file, in %zd byte chunks\n", 
	FileSize, ContentSize);

    if ((openFile = fileSystem->Open((char*)fileNa)) == NULL) {
	printf("Perf test: unable to open file %s\n", (char*)fileNa);
	delete [] buffer;
	return;
    }
    for (i = 0; i < FileSize; i += ContentSize) {
        numBytes = openFile->Read(buffer, ContentSize);
	if ((numBytes < 10) || strncmp(buffer, Contents, ContentSize)) {
	    printf("Perf test: unable to read %s\n", (char*)fileNa);
	    delete openFile;
	    delete [] buffer;
	    return;
	}
    }
    delete [] buffer;
    delete openFile;	// close file
}

void
PerformanceTest()
{
	const char *fileNa = FileName;
    printf("Starting file system performance test:\n");
    stats->Print();
    FileWrite();
    FileRead();
    if (!fileSystem->Remove((char*)fileNa)) {
      printf("Perf test: unable to remove %s\n", FileName);
      return;
    }
    stats->Print();
}

/*-------------------------------------------
				SHELL
-----------------------------------------*/		

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
    printf("nachos %s $ ", fileSystem->GetWorkingNameDirectory());
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
}




void ShellFileSys() {
    char buffer[MaxLenCmd];
    size_t nargs;
    char *args[MaxArg];
    show_help();
    while(1) {
        prompt();
        if (fgets(buffer, MaxLenCmd, stdin) == NULL) break;
        parse_args(buffer, args, MaxArg, &nargs);

        if (nargs==0) continue;
        else if (!strcmp(args[0], "exit" )) break;
        else if (!strcmp(args[0], "ls" ) && (nargs == 1)) {
            fileSystem->ListAll();
        }
        else if (!strcmp(args[0], "ls" ) && (nargs == 2)) {
            fileSystem->ListName(args[1]);
        }
        else if (!strcmp(args[0], "cd") && (nargs == 1) ) {
            fileSystem->CdDir((char *)"/");
        }
        else if (!strcmp(args[0], "cd") && (nargs == 2) ) {
            fileSystem->CdDir(args[1]);
        }
        else if (!strcmp(args[0], "touch") && (nargs == 2) ) {
            fileSystem->Create(args[1], 0);
        }
        else if (!strcmp(args[0], "print") && (nargs == 2) ) {
            Print (args[1]);
        }
        else if (!strcmp(args[0], "cp") && (nargs == 3) ) {
            Copy(args[1], args[2]);
        }
        else if (!strcmp(args[0], "rm") && (nargs == 2) ) {
            fileSystem->Remove(args[1]);
        }
        else if (!strcmp(args[0], "mkdir") && (nargs == 2) ) {
            fileSystem->CreateDirectory(args[1]);
        }
        else if (!strcmp(args[0], "pwd") && (nargs == 1) ) {
            printf("%s\n", fileSystem->GetWorkingNameDirectory());
        }
        else if (!strcmp(args[0], "format")) {
            fileSystem = new FileSystem (true);
        }
        else {
            show_help();
        }
    }

    printf("\nBye\n");
    interrupt->Halt();
}

