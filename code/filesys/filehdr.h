// filehdr.h 
//	Data structures for managing a disk file header.  
//
//	A file header describes where on disk to find the data in a file,
//	along with other information about the file (for instance, its
//	length, owner, etc.)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#ifndef FILEHDR_H
#define FILEHDR_H

#include "disk.h"
#include "bitmap.h"

#define NumDirect 	((SectorSize - 2 * sizeof(int)) / sizeof(int))
#define IndirectNumber (SectorSize / sizeof(int)) 
#define MaxFileSize 	(NumDirect * SectorSize)

// The following class defines the Nachos "file header" (in UNIX terms,  
// the "i-node"), describing where on disk to find all of the data in the file.
// The file header is organized as a simple table of pointers to
// data blocks. 
//
// The file header data structure can be stored in memory or on disk.
// When it is on disk, it is stored in a single sector -- this means
// that we assume the size of this data structure to be the same
// as one disk sector.  Without indirect addressing, this
// limits the maximum file length to just under 4K bytes.
//
// There is no constructor; rather the file header can be initialized
// by allocating blocks for the file (if it is a new file), or by
// reading it from disk.

class FileHeader {
  public:


	/*
	* Allocate
	* Initialize a file header for a new element into the file system
	* allocate a data blocks
	* if all step are good return true
	* return false if the allocation is impossible
	* if false update errorno
	*
	* bitMap is a bit map of free disk sectors
	* fileSize is the size allocated for the file
	*/
    bool Allocate(BitMap *bitMap, int fileSize);
    
	/*
	* Deallocate
	* Unallocate all spaces allocated for this file
	* Update the bitMap of free disk sector
	*
	* bitmap is the bit map of free disk sector
	*/
    void Deallocate(BitMap *bitMap);  		

    void FetchFrom(int sectorNumber); 	// Initialize file header from disk
    void WriteBack(int sectorNumber); 	// Write modifications to file header
					//  back to disk
					
	/*
	* ByteToSector
	* return a particular byte into the file
	*
	* offset is localisation of the bytes research 
	*/
    int ByteToSector(int offset);	

	/*
	* FileLength
	* return the number of bytes int the file
	* absolute value of variable numBytes
	*/
    int FileLength();			// Return the length of the file 
					// in bytes
					
	/*
	* FileDirectory
	* return 0 if is a file header
	* else return 1 (directory header)
	*/
	int FileDirectory();

    void Print();			// Print the contents of the file.

  private:
    int numBytes;			// Number of bytes in the file if Directory header numBytes = - num of bytes 
    int numSectors;			// Number of data sectors in the file
    int dataSectors[NumDirect];		// Disk sector numbers for each data 
					// block in the file
};

#endif // FILEHDR_H
