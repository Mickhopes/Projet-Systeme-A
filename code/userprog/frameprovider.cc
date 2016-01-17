#include "frameprovider.h"
#include "system.h"
#include "machine.h"
#include <strings.h> //For bzero

FrameProvider::FrameProvider (int numpages)
{
    map = new BitMap(numpages);
    mutex = new Semaphore("mutex", 1);

}

//----------------------------------------------------------------------
// FrameProvider::~FrameProvider
//      De-allocate a FrameProvider.
//----------------------------------------------------------------------
FrameProvider::~FrameProvider ()
{
  //  delete map;
  delete map;
  delete mutex;
  // End of modification
}

//----------------------------------------------------------------------
// FrameProvider::GetEmptyFrame
//      Get an empty frame available and initialize it to 0
//----------------------------------------------------------------------
int
FrameProvider::GetEmptyFrame ()
{
	mutex->P();
	int frame = map->Find();
	bzero (machine->mainMemory+frame*PageSize, PageSize);
	mutex->V();
	return frame;
}

//----------------------------------------------------------------------
// FrameProvider::ReleaseFrame
//      Release the frame given in parameter
//
//		"frame" - The frame number to release  
//----------------------------------------------------------------------
void
FrameProvider::ReleaseFrame (int frame)
{
	mutex->P();
	map->Clear(frame);
	mutex->V();
}

//----------------------------------------------------------------------
// FrameProvider::NumAvailFrame
//      Return the number of frames available  
//----------------------------------------------------------------------
unsigned int
FrameProvider::NumAvailFrame ()
{
	mutex->P();
	unsigned int numClear = map->NumClear();;
	mutex->V();
   return numClear;
}
