#include <cstdlib>
#include <iostream>
#include <SDL.h>
#include "lattice2d.h"

namespace
{
	const int W = 512;
	const int H = 512;
	float summedFsBuffer[W * H];
}

#if 0
float myLoader(int x, int y, int i)
{
	if (i > 1)
		return 0.0;
	if (i == x % 2)
		return 1.0;
	else
		return 0.0;
}
#elif 1
float myLoader(int x, int y, int i)
{
	if (i != 2 && i != 7)
		return 0.0;
	if ((x/16) % 2 == 0 && (y/16) % 2 == 0)
		return 0.5;
	else
		return 0.0;
}
#endif

void drawLattice(const Lattice2D& l, SDL_Surface* pLockedSurf)
{
	l.getSummedFs(summedFsBuffer);
	for (int y = 0; y < 512; y++)
		for (int x = 0; x < 512; x++)
		{
			Uint8 grayLevel = (Uint8)(summedFsBuffer[x + W * y] * 255.0);
			Uint32 valueToStore = (grayLevel << 16) | (grayLevel << 8) |
				grayLevel;
			((Uint32*)pLockedSurf->pixels)[x + y * 512] = valueToStore;
		}
}

int main(int argc, char* argv[])
{
	Lattice2D l(512, 512, myLoader);

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Surface* pScreen = SDL_SetVideoMode(512, 512, 32, SDL_DOUBLEBUF);

	SDL_Event ev;
	int i = 0;
	while(true)
	{
		Uint32 start = SDL_GetTicks();
		if (SDL_PollEvent(&ev) && ev.type == SDL_QUIT)
			break;
		SDL_LockSurface(pScreen);
		drawLattice(l, pScreen);
		SDL_UnlockSurface(pScreen);
		SDL_Flip(pScreen);
		l.step();
		Uint32 end = SDL_GetTicks();
		if (i++ % 10 == 0)
			std::cout << "FPS: " << 1000.0 / (double)(end - start) << "\n";
	}

	SDL_Quit();

	return 0;
}
