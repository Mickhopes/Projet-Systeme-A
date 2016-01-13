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
//      
//
//      
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
//      
//
//      
//----------------------------------------------------------------------
void
FrameProvider::ReleaseFrame (int frame)
{
   map->Clear(frame);
}

//----------------------------------------------------------------------
// FrameProvider::NumAvailFrame
//      
//
//      
//----------------------------------------------------------------------
unsigned int
FrameProvider::NumAvailFrame ()
{
   return map->NumClear();
}