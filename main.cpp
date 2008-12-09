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

void myLoader(int x, int y, real_t& rho, real_t& ux, real_t& uy)
{
	rho = (real_t)0.5;
	uy = (real_t)0.0;

	real_t scSqDist = (real_t)(sqr(x - W / 2) + sqr(y - H / 2)) / sqr(W / 8);
	ux = (real_t)(0.7 / (scSqDist + 1.0));
}

// FIXME: Returns mass only as a means of debugging
real_t drawLattice(const Lattice2D& l, SDL_Surface* pLockedSurf)
{
	const real_t* data = l.getData();
	real_t mass = 0.0;
	for (int y = 0; y < H; y++)
		for (int x = 0; x < W; x++)
		{
			real_t rho = data[3 * (x + W * y)];
			real_t ux = data[3 * (x + W * y) + 1];
			real_t uy = data[3 * (x + W * y) + 2];
			if (ux < (real_t)0.0)
				ux = 0.0;
			if (uy < (real_t)0.0)
				uy = 0.0;
			mass += rho;
			Uint8 ux8b = (Uint8)(ux * 255.0);
			Uint8 uy8b = (Uint8)(uy * 255.0);
			Uint32 valueToStore = (ux8b << 16) | (uy8b << 8);
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
