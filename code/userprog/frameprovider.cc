#include "frameprovider.h"
#include "system.h"
#include "machine.h"
#include <strings.h> //For bzero

FrameProvider::FrameProvider (int numpages)
{
    map = new BitMap(numpages);

}

//----------------------------------------------------------------------
// FrameProvider::~FrameProvider
//      De-allocate a FrameProvider.
//----------------------------------------------------------------------
FrameProvider::~FrameProvider ()
{
  //  delete map;
  delete map;
  // End of modification
}

//----------------------------------------------------------------------
// FrameProvider::GetEmptyFrame
//      Get an empty frame available and initialize it to 0
//----------------------------------------------------------------------
int
FrameProvider::GetEmptyFrame ()
{
	int frame = map->Find();
	bzero (machine->mainMemory+frame*PageSize, PageSize);
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
   map->Clear(frame);
}

//----------------------------------------------------------------------
// FrameProvider::NumAvailFrame
//      Return the number of frames available  
//----------------------------------------------------------------------
unsigned int
FrameProvider::NumAvailFrame ()
{
   return map->NumClear();
}