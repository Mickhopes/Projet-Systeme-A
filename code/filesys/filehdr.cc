// filehdr.cc 
//	Routines for managing the disk file header (in UNIX, this
//	would be called the i-node).
//
//	The file header is used to locate where on disk the 
//	file's data is stored.  We implement this as a fixed size
//	table of pointers -- each entry in the table points to the 
//	disk sector containing that portion of the file data
//	(in other words, there are no indirect or doubly indirect 
//	blocks). The table size is chosen so that the file header
//	will be just big enough to fit in one disk sector, 
//
//      Unlike in a real system, we do not keep track of file permissions, 
//	ownership, last modification date, etc., in the file header. 
//
//	A file header can be initialized in two ways:
//	   for a new file, by modifying the in-memory data structure
//	     to point to the newly allocated data blocks
//	   for a file already on disk, by reading the file header from disk
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"

#include "system.h"
#include "filehdr.h"



bool FileHeader::Allocate(BitMap *freeMap, int fileSize)
{ 
    numBytes = fileSize;
    numSectors  = divRoundUp(fileSize, SectorSize);
    if (freeMap->NumClear() < numSectors)
    {
    	
    	return false;		// not enough space
    }
	int *indirectList;
	int i, j, allocatedSector = 0;
	for(i = 0; i < (int)NumDirect && allocatedSector < numSectors; i++)
	{
		dataSectors[i] = freeMap->Find();
		indirectList = new int[IndirectNumber];
		for(j = 0; (j < (int)IndirectNumber) && allocatedSector < numSectors; j++)
		{
			indirectList[j] = freeMap->Find();
			allocatedSector++;
		}
		synchDisk->WriteSector(dataSectors[i], (char *)indirectList);
	}
		
    return true;
}


void FileHeader::Deallocate(BitMap *freeMap)
{
    int i,j, desallocatedSector = 0;
    int *indirectList;
    for(i = 0; i < (int)NumDirect && desallocatedSector < numSectors; i++)
	{
		indirectList = new int[IndirectNumber];
		synchDisk->ReadSector(dataSectors[i], (char *)indirectList);
		for(j = 0; j < (int)IndirectNumber && desallocatedSector < numSectors; j++)
		{
			desallocatedSector++;
		}
		freeMap->Clear(dataSectors[i]);
	}
}

//----------------------------------------------------------------------
// FileHeader::FetchFrom
// 	Fetch contents of file header from disk. 
//
//	"sector" is the disk sector containing the file header
//----------------------------------------------------------------------

void
FileHeader::FetchFrom(int sector)
{
    synchDisk->ReadSector(sector, (char *)this);
}

//----------------------------------------------------------------------
// FileHeader::WriteBack
// 	Write the modified contents of the file header back to disk. 
//
//	"sector" is the disk sector to contain the file header
//----------------------------------------------------------------------

void
FileHeader::WriteBack(int sector)
{
    synchDisk->WriteSector(sector, (char *)this); 
}



int FileHeader::ByteToSector(int offset)
{
	int sectorOfByte = offset / SectorSize;
	int placeInData = sectorOfByte / IndirectNumber;
	int indirectListPosition = sectorOfByte % IndirectNumber;
	int *indirectList = new int[IndirectNumber];
	
	synchDisk->ReadSector(dataSectors[placeInData], (char *)indirectList);
    return indirectList[indirectListPosition];
}

int FileHeader::FileLength()
{
    return abs(numBytes);
}


int FileHeader::FileDirectory()
{
	if (numBytes < 0)
	{
		return 1;
	}
	else 
	{
		return 0;
	}
}

//----------------------------------------------------------------------
// FileHeader::Print
// 	Print the contents of the file header, and the contents of all
//	the data blocks pointed to by the file header.
//----------------------------------------------------------------------

void
FileHeader::Print()
{
    int i, j, k;
    char *data = new char[SectorSize];
    int numByte = abs(numBytes);
    

    printf("FileHeader contents.  File size: %d.  File blocks:\n", numByte);
    for (i = 0; i < numSectors; i++)
		printf("%d ", dataSectors[i]);
    printf("\nFile contents:\n");
    for (i = k = 0; i < numSectors; i++)
    {
		synchDisk->ReadSector(dataSectors[i], data);
        for (j = 0; (j < SectorSize) && (k < numByte); j++, k++) 
        {
			if ('\040' <= data[j] && data[j] <= '\176')   // isprint(data[j])
				printf("%c", data[j]);
			else
				printf("\\%x", (unsigned char)data[j]);
		}
        printf("\n"); 
    }
    delete [] data;
}
