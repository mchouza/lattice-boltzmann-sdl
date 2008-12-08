#include <iostream>
#include <SDL.h>
#include "fast_lattice2d.h"
#include "slow_lattice2d.h"

//#define USE_FAST_LATTICE

namespace
{
	const int W = 512;
	const int H = 512;

	template <typename T>
	inline T sqr(T x)
	{
		return x * x;
	}
}

#if 0
real_t myLoader(int x, int y, int i)
{
	if (i != ((x/16) + (y/16) * 5) % 9)
		return 0.0;
	if ((x/16) % 2 == 0 && (y/16) % 2 == 0)
		return 0.5;
	else
		return 0.0;
}
#endif
#if 0
real_t myLoader(int x, int y, int i)
{
	if (i > 1)
		return 0.0;
	if (x % 16 < 8)
		return 0.5;
	else
		return 0.0;
}
#endif
real_t myLoader(int x, int y, int i)
{
	if (i > 1)
		return 0.0;
	real_t ratio = (sqr(x - W / 2) + sqr(y - H / 2)) / sqr(W / 8);
	real_t u = 0.4 / (1 + ratio);
	if (i == 1)
		return 0.5 * u;
	else
		return 0.5 * (1 - u);
}

// FIXME: Returns mass only as a means of debugging
real_t drawLattice(const Lattice2D& l, SDL_Surface* pLockedSurf)
{
	const real_t* data = l.getData();
	real_t mass = 0.0;
	for (int y = 0; y < H; y++)
		for (int x = 0; x < W; x++)
		{
			mass += data[3 * (x + W * y)];
			real_t scaledData = data[3 * (x + W * y)] * 255.0f;
			if (scaledData > 255.0f)
				scaledData = 255.0f;
			Uint8 grayLevel = (Uint8)(scaledData);
			Uint32 valueToStore = (grayLevel << 16) | (grayLevel << 8) |
				grayLevel;
			((Uint32*)pLockedSurf->pixels)[x + y * W] = valueToStore;
		};
	return mass;
}

int main(int argc, char* argv[])
{
#ifdef USE_FAST_LATTICE
	FastLattice2D l(W, H, myLoader);
#else
	SlowLattice2D l(W, H, myLoader);
#endif

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Surface* pScreen = SDL_SetVideoMode(W, H, 32, SDL_DOUBLEBUF);

	SDL_Event ev;
	int i = 0;
	while(true)
	{
		Uint32 start = SDL_GetTicks();
		if (SDL_PollEvent(&ev) && ev.type == SDL_QUIT)
			break;
		SDL_LockSurface(pScreen);
		double mass = drawLattice(l, pScreen);
		SDL_UnlockSurface(pScreen);
		SDL_Flip(pScreen);
		l.step();
		Uint32 end = SDL_GetTicks();
		if (i++ % 10 == 0)
		{
			std::cout << "FPS: " << 1000.0 / (double)(end - start) << "\n";
			std::cout << "Mass: " << mass << "\n";
		}
	}

	SDL_Quit();

	return 0;
}
