// directory.cc
//    Routines to manage a directory of file names.
//
//    The directory is a table of fixed length entries; each
//    entry represents a single file, and contains the file name,
//    and the location of the file header on disk.  The fixed size
//    of each directory entry means that we have the restriction
//    of a fixed maximum size for file names.
//
//    The constructor initializes an empty directory of a certain size;
//    we use ReadFrom/WriteBack to fetch the contents of the directory
//    from disk, and to write back any modifications back to disk.
//
//    Also, this implementation has the restriction that the size
//    of the directory cannot expand.  In other words, once all the
//    entries in the directory are used, no more files can be created.
//    Fixing this is one of the parts to the assignment.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "utility.h"
#include "filehdr.h"
#include "directory.h"

#define MaxNameDirectoryLen 150
//----------------------------------------------------------------------
// Directory::Directory
//     Initialize a directory; initially, the directory is completely
//    empty.  If the disk is being formatted, an empty directory
//    is all we need, but otherwise, we need to call FetchFrom in order
//    to initialize it from disk.
//
//    "size" is the number of entries in the directory
//----------------------------------------------------------------------

Directory::Directory(int size) {
    table = new DirectoryEntry[size];
    tableSize = size;
    for (int i = 0; i < tableSize; i++)
        table[i].inUse = false;
   
   
    table[0].inUse = true;
    table[0].sector = 1;
    strcpy(table[0].name, ".");

    table[1].inUse = true;
    table[1].sector = 1;
    strcpy(table[1].name, "..");
}

Directory::Directory(int size, int sector, int fatherSector) {
    table = new DirectoryEntry[size];
    tableSize = size;
    for (int i = 2; i < tableSize; i++)
        table[i].inUse = false;
   
   
    table[0].inUse = true;
    table[0].sector = sector;
    strcpy(table[0].name, ".");

    table[1].inUse = true;
    table[1].sector = fatherSector;
    strcpy(table[1].name, "..");
}


//----------------------------------------------------------------------
// Directory::~Directory
// De-allocate directory data structure.
//----------------------------------------------------------------------

Directory::~Directory()
{
    delete [] table;
}

//----------------------------------------------------------------------
// Directory::FetchFrom
// Read the contents of the directory from disk.
//
// "file" -- file containing the directory contents
//----------------------------------------------------------------------

void
Directory::FetchFrom(OpenFile *file)
{
    (void) file->ReadAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
}

//----------------------------------------------------------------------
// Directory::WriteBack
//     Write any modifications to the directory back to disk
//
//    "file" -- file to contain the new directory contents
//----------------------------------------------------------------------

void
Directory::WriteBack(OpenFile *file)
{
    (void) file->WriteAt((char *)table, tableSize * sizeof(DirectoryEntry), 0);
}

//----------------------------------------------------------------------
// Directory::FindIndex
//     Look up file name in directory, and return its location in the table of
//    directory entries.  Return -1 if the name isn't in the directory.
//
//    "name" -- the file name to look up
//----------------------------------------------------------------------

int
Directory::FindIndex(const char *name)
{
    for (int i = 0; i < tableSize; i++)
        if (table[i].inUse && !strncmp(table[i].name, name, FileNameMaxLen))
            return i;
    return -1;        // name not in directory
}

//----------------------------------------------------------------------
// Directory::Find
//     Look up file name in directory, and return the disk sector number
//    where the file's header is stored. Return -1 if the name isn't
//    in the directory.
//
//    "name" -- the file name to look up
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
//     Add a file into the directory.  Return TRUE if successful;
//    return FALSE if the file name is already in the directory, or if
//    the directory is completely full, and has no more space for
//    additional file names.
//
//    "name" -- the name of the file being added
//    "newSector" -- the disk sector containing the added file's header
//----------------------------------------------------------------------

bool
Directory::Add(const char *name, int newSector)
{
    if (FindIndex(name) != -1)
    {
    	errorno = ENAMEEXIST;
    	return FALSE;
    }

    for (int i = 2; i < tableSize; i++)
        if (!table[i].inUse) {
            table[i].inUse = TRUE;
            strncpy(table[i].name, name, FileNameMaxLen);
            table[i].sector = newSector;
            return TRUE;
        }
    return FALSE;    // no space.  Fix when we have extensible files.
}

//----------------------------------------------------------------------
// Directory::Remove
//		Remove a file name from the directory.  
//		Return TRUE if successful;
//		return FALSE if the file isn't in the directory and update errorno.
//
//		name -- the file name to be removed
//		sector -- the file sector to be remove 
//----------------------------------------------------------------------

bool Directory::Remove(const char *name)
{
    int i = FindIndex(name);

    if (i == -1)
    {
    	errorno = ENAMENOTEXIST;
        return FALSE;         // name not in directory
    }
    table[i].inUse = FALSE;
    return TRUE;
}

bool Directory::Remove(int sector)
{
    for (int i = 2; i < tableSize; i++)
        if (table[i].sector == sector) 
        {
            table[i].inUse = false;
            return true;
        }
    return false;
}

//----------------------------------------------------------------------
// Directory::List
//     List all the file names in the directory.
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
//     List all the file names in the directory, their FileHeader locations,
//    and the contents of each file.  For debugging.
//----------------------------------------------------------------------

void
Directory::Print()
{
    FileHeader *hdr = new FileHeader;

    printf("Directory contents:\n");
    for (int i = 0; i < tableSize; i++)
    if (table[i].inUse) {
        printf("Name: %s, Sector: %d\n", table[i].name, table[i].sector);
        hdr->FetchFrom(table[i].sector);
        hdr->Print();
    }
    printf("\n");
    delete hdr;
}

bool Directory::DirectoryisFull() 
{

  for (int i = 2; i < tableSize; i++) 
  {
    if(table[i].inUse == false)
      return false;
  }
  return true;
}

//return true if directory is empty
bool Directory::DirectoryisEmpty() 
{

  for (int i = 2; i < tableSize; i++) 
  {
    if (table[i].inUse == true)
      return false;
  }
  return true;
}

// return true if directory is root(directory setor = father directory sector) 
bool Directory::DirectoryisRoot() 
{
    return (table[0].sector == table[1].sector);
}

char * Directory::GetNameFromSector(int sector) 
{
    for (int i = 2; i < tableSize; i++)
        if (table[i].sector == sector) 
        {
            return table[i].name;
        }
    return NULL;
}

int Directory::GetSector(int position) 
{
    return this->table[position].sector;
}

int Directory::GetCurrentSector() 
{
    return this->GetSector(0);
}

int Directory::GetFatherSector() 
{
    return this->GetSector(1);
}

char * Directory::GetDirName() 
{
    Directory * currentDir = this;
    Directory * fatherDir;
    int fatherSector;
    char * fullname = new char[MaxNameDirectoryLen];
    char * tmp = new char[MaxNameDirectoryLen];
    strcpy(fullname, "/");
    while (!currentDir->DirectoryisRoot()) 
    {
		fatherSector = currentDir->GetFatherSector();
        OpenFile * fatherDirFile = new OpenFile(fatherSector);
        fatherDir = new Directory(this->tableSize);
        fatherDir->FetchFrom(fatherDirFile);
       

        strcpy(tmp, "/");
        strcat(tmp, fatherDir->GetNameFromSector(currentDir->GetCurrentSector()));
        strcat(tmp, fullname);
        strcpy(fullname, tmp);
        currentDir = fatherDir;
    }
    return fullname;
}

