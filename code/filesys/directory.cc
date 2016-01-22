// directory.cc 
//	Routines to manage a directory of file names.
//
//	The directory is a table of fixed length entries; each
//	entry represents a single file, and contains the file name,
//	and the location of the file header on disk.  The fixed size
//	of each directory entry means that we have the restriction
//	of a fixed maximum size for file names.
//
//	The constructor initializes an empty directory of a certain size;
//	we use ReadFrom/WriteBack to fetch the contents of the directory
//	from disk, and to write back any modifications back to disk.
//
//	Also, this implementation has the restriction that the size
//	of the directory cannot expand.  In other words, once all the
//	entries in the directory are used, no more files can be created.
//	Fixing this is one of the parts to the assignment.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "utility.h"
#include "filehdr.h"
#include  "directory.h"

//----------------------------------------------------------------------
// Directory::Directory
// 	cf header file
//----------------------------------------------------------------------

Directory::Directory(int size, int sec, int fatherSec)
{
    table = new DirectoryEntry[size];
    tableSize = size;
    
    //Setup of the hierarchy
    
    // for the currentDirectory "."
	table[currentDirectory].isDirectory = 1;
	table[currentDirectory].inUse = true;
	table[currentDirectory].sector = sec;
	
	// for the fatherDirectory ".."
	table[fatherDirectory].isDirectory = 1;
	table[fatherDirectory].inUse = true;
	table[fatherDirectory].sector = fatherSec;
	int i;
	for (i = specialEntry; i < tableSize; i++)
	{
		table[i].inUse = FALSE;
		
	}
}

Directory::Directory(int size)
{
   table = new DirectoryEntry[size];
    tableSize = size;
    
    //Setup of the hierarchy
    
    // for the currentDirectory "."
	table[currentDirectory].isDirectory = 1;
	table[currentDirectory].inUse = true;
	table[currentDirectory].sector = 1;
	
	// for the fatherDirectory ".."
	table[fatherDirectory].isDirectory = 1;
	table[fatherDirectory].inUse = true;
	table[fatherDirectory].sector = 1;
	int i;
	for (i = specialEntry; i < tableSize; i++)
	{
		table[i].inUse = FALSE;
		
	}
}

//----------------------------------------------------------------------
// Directory::~Directory
// 	De-allocate directory data structure.
//----------------------------------------------------------------------

Directory::~Directory()
{ 
    
    delete [] table;
} 

//----------------------------------------------------------------------
// Directory::FetchFrom
// 	Read the contents of the directory from disk.
//
//	"file" -- file containing the directory contents
//----------------------------------------------------------------------

void
Directory::FetchFrom(OpenFile *file)
{
    (void) file->ReadAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
}

//----------------------------------------------------------------------
// Directory::WriteBack
// 	Write any modifications to the directory back to disk
//
//	"file" -- file to contain the new directory contents
//----------------------------------------------------------------------

void
Directory::WriteBack(OpenFile *file)
{
    (void) file->WriteAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
}

//----------------------------------------------------------------------
// Directory::FindIndex
// 	Look up file name in directory, and return its location in the table of
//	directory entries.  Return -1 if the name isn't in the directory.
//	Works for subdirectories as well as files.
//	"name" -- the file name to look up
//----------------------------------------------------------------------

int
Directory::FindIndex(char *name)
{
    for (int i = 0; i < tableSize; i++)
        if (table[i].inUse && !strncmp(table[i].name, name, FileNameMaxLen))
			return i;
    return -1;		// name not in directory
}

//----------------------------------------------------------------------
// Directory::Find
// 	Look up file name in directory, and return the disk sector number
//	where the file's header is stored. Return -1 if the name isn't 
//	in the directory.
//
//	"name" -- the file name to look up
//----------------------------------------------------------------------

int
Directory::Find(char *name)
{
    int i = FindIndex(name);

    if (i != -1)
		return table[i].sector;
    return -1;
}


bool Directory::AddFile(char *name, int newSector)
{ 
    for (int i = 0; i < tableSize; i++)
        if (!table[i].inUse) {
            table[i].inUse = true;
            strncpy(table[i].name, name, FileNameMaxLen); 
            table[i].sector = newSector;
            table[i].isDirectory = 0;
	}
    return true;	
}


bool Directory::AddDirectory(char *name, int newSector)
{ 
    

    for (int i = 0; i < tableSize; i++)
        if (!table[i].inUse) {
            table[i].inUse = true;
            strncpy(table[i].name, name, FileNameMaxLen); 
            table[i].sector = newSector;
            table[i].isDirectory = 1;
	}
    return true;	
}


bool Directory::Add(char *name, int newSector, int isDirectory)
{ 
    if (FindIndex(name) != -1)
		return false;

	if(DirectoryIsFull())
		return false;
	else
	{
		if(isDirectory == 0)
			return AddFile(name, newSector);
		else
			return AddDirectory(name, newSector);
	}
    return false;	//for compilation
}

bool
Directory::RemoveFile(char *nameFi)
{ 
    int i = FindIndex(nameFi);
    
	if (table[i].isDirectory == 1)
	{
		errorno = EWTYPE;
		return false;
	}
    table[i].inUse = false;
    return true;	
}



bool Directory::RemoveDirectory(char *nameDir)
{
	int i = FindIndex(nameDir);
	
	
	if (table[i].isDirectory == 0)
	{
		errorno = EWTYPE;
		return false;
	}
	
	if(DirectoryIsEmpty() == TRUE)
	{
		table[i].inUse = false;
		return true;
	}
	errorno = EDIRNOTEMPTY;
	return false;
	
	
}

bool Directory::Remove(char *name)	
{
	int i = FindIndex(name);
	if (i == -1)
    {
		errorno = EDIRNOTEXIST;
		return false; 		// name not in directory
	}
	if (table[i].isDirectory == 0)
	{
		return RemoveFile(name);
	}
	else
	{
		return RemoveDirectory(name);
	}
	
}

bool Directory::DirectoryIsEmpty()
{
	for(int i = specialEntry; i < tableSize; i++)
	{
		if(table[i].inUse == true)
			return false;
	}
	return true;
}

bool Directory::DirectoryIsFull()
{
	for(int i = specialEntry; i < tableSize; i++)
	{
		if(table[i].inUse == false)
			return false;
	}
	return true;
}

//----------------------------------------------------------------------
// Directory::List
// 	List all the entry names in the directory. 
//----------------------------------------------------------------------

void
Directory::List()
{
   for (int i = 0; i < tableSize; i++)
	if (table[i].inUse)
	    printf("%s\n", table[i].name);
}

//----------------------------------------------------------------------
// Directory::Print
// 	List all the file names in the directory, their FileHeader locations,
//	and the contents of each file.  For debugging.
//----------------------------------------------------------------------

void
Directory::Print()
{ 
    FileHeader *hdr = new FileHeader;

    printf("Directory contents:\n");
    for (int i = 0; i < tableSize; i++)
	if (table[i].inUse) {
		if(table[i].isDirectory == 1){
			printf("Directory : %s, Sector : %d\n", table[i].name, table[i].sector);
			//TODO : call Print in the subdirectory to list all its own contents (with an indent ?)
		}
		else{
			printf("Name: %s, Sector: %d\n", table[i].name, table[i].sector);
			hdr->FetchFrom(table[i].sector);
			hdr->Print();
		}
	}
    printf("\n");
    delete hdr;
}

