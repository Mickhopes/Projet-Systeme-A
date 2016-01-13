#include "frameprovider.h"


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
   
}

//----------------------------------------------------------------------
// FrameProvider::NumAvailFrame
//      
//
//      
//----------------------------------------------------------------------
int
FrameProvider::NumAvailFrame ()
{
   
}