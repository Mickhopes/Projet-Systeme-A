#ifndef FRAMEPROVIDER_H
#define FRAMEPROVIDER_H

#include "bitmap.h"

class FrameProvider{
	public:
		FrameProvider (int numpages);
		~FrameProvider ();

		int GetEmptyFrame();
		void ReleaseFrame(int frame);
		unsigned int NumAvailFrame();
	private:
		BitMap *map;
		Semaphore *mutex; // Mutex
};

#endif //FRAMEPROVIDER_H
