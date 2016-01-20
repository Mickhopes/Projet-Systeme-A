// directory.h 
//	Data structures to manage a UNIX-like directory of file names.
// 
//      A directory is a table of pairs: <file name, sector #>,
//	giving the name of each file in the directory, and 
//	where to find its file header (the data structure describing
//	where to find the file's data blocks) on disk.
//
//      We assume mutual exclusion is provided by the caller.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "openfile.h"
#include "errorno.h"

#define FileNameMaxLen 		9	// for simplicity, we assume 
					// file names are <= 9 characters long
#define directoryNameMaxLen 9
					
#define specialEntry 2

#define currentDirectory 0
#define fatherDirectory 1

#define nameCurrentDirectory "."
#define nameFatherDirectory ".."

// The following class defines a "directory entry", representing a file
// in the directory.  Each entry gives the name of the file, and where
// the file's header is to be found on disk.
//
// Internal data structures kept public so that Directory operations can
// access them directly.

class DirectoryEntry {
  public:
    bool inUse;				// Is this directory entry in use?
    int isDirectory;		//if isDirectory = 0 entry is a file else entry is a directory
    int sector;				// Location on disk to find the 
					//   FileHeader for this file 
    char name[FileNameMaxLen + 1];	// Text name for file, with +1 for 
					// the trailing '\0'
};



	

// The following class defines a UNIX-like "directory".  Each entry in
// the directory describes a file, and where to find it on disk.
//
// The directory data structure can be stored in memory, or on disk.
// When it is on disk, it is stored as a regular Nachos file.
//
// The constructor initializes a directory structure in memory; the
// FetchFrom/WriteBack operations shuffle the directory information
// from/to disk. 

class Directory {
  public:
	/*
	* Initialize an empty directory
	* size is the number of entries in directory(2 reserved for "." and "..")
	* nameDirectory is the name of the directory
	* sec is a pointer on header of the current directory 
	* fatherSec is a pointer on header of the father directory 
	*/
    Directory(int size, char *nameDir, int sec, int fatherSec);
    
    ~Directory();			// De-allocate the directory

    void FetchFrom(OpenFile *file);  	// Init directory contents from disk
    void WriteBack(OpenFile *file);	// Write modifications to 
					// directory contents back to disk

    int Find(const char *name);		// Find the sector number of the 
					// FileHeader for file: "name"

    bool Add(const char *name, int newSector, int isDirectory);  // Add a file name into the directory


	/*
	 * remove file with name = nameFile
	 * return TRUE if success
	 * FALSE else
	 * when return FALSE errorno is updated
	*/
    bool RemoveFile(const char *nameFi);	// Remove a file from the directory
    
    /*
	 * remove directory with name = nameDir
	 * return TRUE if success
	 * FALSE else
	 * when return FALSE errorno is updated
	*/
    bool RemoveDirectory(const char *nameDir);
    
    
    /*
     * COMPULSORY dirEn indicate a directory not a File
     * isEmpty returns TRUE if the directory indicated by dirEn is empty
     * else FALSE 
    */
    bool isEmpty(DirectoryEntry dirEn);

    void List();			// Print the names of all the files
					//  in the directory
    void Print();			// Verbose print of the contents
					//  of the directory -- all the file
					//  names and their contents.
	

  private:
  
	char *nameDirectory;
    int tableSize;			// Number of directory entries
    DirectoryEntry *table;		// Table of pairs: 
					// <file name, file header location> 
					
	int sector;		//pointer of the header of current directory
	
	int fatherSector; //pointer on the header of father directory
						//if fatherSector ==  sector currentDirectory is "/"

    int FindIndex(const char *name);	// Find the index into the directory 
					//  table corresponding to "name"
};

#endif // DIRECTORY_H
