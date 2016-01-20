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
	table[currentDirectory].inUse = TRUE;
	table[currentDirectory].sector = sec;
	
	// for the fatherDirectory ".."
	table[fatherDirectory].isDirectory = 1;
	table[fatherDirectory].inUse = TRUE;
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
	table[currentDirectory].inUse = TRUE;
	table[currentDirectory].sector = 1;
	
	// for the fatherDirectory ".."
	table[fatherDirectory].isDirectory = 1;
	table[fatherDirectory].inUse = TRUE;
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
Directory::FindIndex(const char *name)
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
Directory::Find(const char *name)
{
    int i = FindIndex(name);

    if (i != -1)
	return table[i].sector;
    return -1;
}

//----------------------------------------------------------------------
// Directory::Add
// 	Add a file or a sub-directory into the directory.  Return TRUE if successful;
//	return FALSE if the file name is already in the directory, or if
//	the directory is completely full, and has no more space for
//	additional file names.
//
//	"name" -- the name of the file being added
//	"newSector" -- the disk sector containing the added file's header
//	"isDirectory" -- indicates if the new addition is a sub-directory	
//----------------------------------------------------------------------

bool
Directory::Add(const char *name, int newSector, int isDirectory)
{ 
    if (FindIndex(name) != -1)
	return FALSE;

    for (int i = 0; i < tableSize; i++)
        if (!table[i].inUse) {
            table[i].inUse = TRUE;
            strncpy(table[i].name, name, FileNameMaxLen); 
            table[i].sector = newSector;
            table[i].isDirectory = isDirectory;
        return TRUE;
	}
    return FALSE;	// no space.  Fix when we have extensible files.
}

bool
Directory::RemoveFile(const char *nameFi)
{ 
    int i = FindIndex(nameFi);

    if (i == -1)
    {
		errorno = EFILENOTEXIST;
		return FALSE; 		// name not in directory
	}
		
	if (table[i].isDirectory == 1)
	{
		errorno = EWTYPE;
		return FALSE;
	}
    table[i].inUse = FALSE;
    return TRUE;	
}



bool Directory::RemoveDirectory(int sectorDir)
{
	int i = FindIndex(nameDir);
	if (i == -1)
    {
		errorno = EDIRNOTEXIST;
		return FALSE; 		// name not in directory
	}
	
	if (table[i].isDirectory == 0)
	{
		errorno = EWTYPE;
		return FALSE;
	}
	
	if(isEmpty(table[i]) == TRUE)
	{
		table[i].inUse = FALSE;
		return TRUE;
	}
	errorno = EDIRNOTEMPTY;
	return FALSE;
	
	
}

bool Directory::isEmpty(DirectoryEntry dirEn)
{
	return FALSE;
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

