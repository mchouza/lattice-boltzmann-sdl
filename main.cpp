#include <cmath>
#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include "fast_lattice2d.h"
#include "one_pass_lattice2d.h"
#include "slow_lattice2d.h"

// TODO: Replace the multiple "ifdefs" by a more OO approach

#define USE_FAST_LATTICE 0
#define USE_SLOW_LATTICE 0
#define USE_ONE_PASS_LATTICE 1

#define FLUID_BALL_LOADER 0
#define UNIFORM_YVEL_LOADER 0
#define TWO_FLUID_BALLS_LOADER 1

#define DRAW_WITH_POINT_CLOUD 1
#define DRAW_WITH_ADVECTED_STRIPS 0

namespace
{
	const int N = 512;

	const int NPX = 200;
	const int NPY = 15;
	const int NPOINTS = NPX * NPY;

	const real_t SPEED = (real_t)0.6;

	template <typename T>
	inline T sqr(T x)
	{
		return x * x;
	}
}

#if FLUID_BALL_LOADER
void myLoader(int x, int y, real_t& rho, real_t& ux, real_t& uy)
{
	rho = (real_t)0.5;
	uy = (real_t)0.0;

	if (sqr(x - N / 2) + sqr(y - N / 2) < sqr(N / 8))
		ux = (real_t)SPEED;
	else
		ux = (real_t)0.0;
}
#elif UNIFORM_YVEL_LOADER
void myLoader(int x, int y, real_t& rho, real_t& ux, real_t& uy)
{
	rho = (real_t)0.5;
	ux = (real_t)0.5;
	uy = (real_t)0.5;
}
#elif TWO_FLUID_BALLS_LOADER
void myLoader(int x, int y, real_t& rho, real_t& ux, real_t& uy)
{
	rho = (real_t)0.5;
	uy = (real_t)0.0;

	if (sqr(x - N / 5) + sqr(y - N / 2) < sqr(N / 8))
		ux = (real_t)SPEED;
	else if (sqr(x - 4 * N / 5) + sqr(y - N / 2) < sqr(N / 8))
	{
		ux = (real_t)-SPEED;
		uy = (real_t)(SPEED / 6.0);
	}
	else
		ux = (real_t)0.0;
}
#else
#error One loader must be selected.
#endif

#if DRAW_WITH_POINT_CLOUD
namespace
{
	float pointsVec[NPOINTS][2];
	SDL_Surface* pScreen;
}

void initDrawLattice()
{
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	pScreen = SDL_SetVideoMode(N, N, 32, SDL_OPENGL);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, N, N, 0);

	for (int i = 0; i < NPX; i++)
	{
		for (int j = 0; j < NPY; j++)
		{
			pointsVec[i + NPX * j][0] = ((real_t)i / NPX) * N;
			pointsVec[i + NPX * j][1] = ((real_t)j / NPY) * N;
		}
	}
}

void finishDrawLattice()
{
}

void drawLattice(const Lattice2D& l)
{
	// move points with fluid velocity
	for (int i = 0; i < NPOINTS; i++)
	{
		int x = (int)pointsVec[i][0] % N;
		int y = (int)pointsVec[i][1] % N;
		if (x < 0)
			x += N;
		if (y < 0)
			y += N;
		
		pointsVec[i][0] += l.getData()[3 * (x + y * N) + 1];
		pointsVec[i][1] += l.getData()[3 * (x + y * N) + 2];
	}

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBegin(GL_POINTS);
		for (int i = 0; i < NPOINTS; i++)
		{
			int x = (int)pointsVec[i][0] % N;
			int y = (int)pointsVec[i][1] % N;
			if (x < 0)
				x += N;
			if (y < 0)
				y += N;
			glVertex2i(x, y);
		}
	glEnd();

	SDL_GL_SwapBuffers();
}
#elif DRAW_WITH_ADVECTED_STRIPS
namespace
{
	SDL_Surface* pScreen;
	float* smokeDensity;
	float* altSmokeDensity;
}

void initDrawLattice()
{
	pScreen = SDL_SetVideoMode(N, N, 32, SDL_DOUBLEBUF);
	smokeDensity = new float[N * N];
	altSmokeDensity = new float[N * N];

	for (int y = 0; y < N; y++) 
		for (int x = 0; x < N; x++)
			smokeDensity[x + y * N] = (float)((sin(y / 1.5) + 1.0) / 2.0);
}

void finishDrawLattice()
{
	delete[] smokeDensity;
	delete[] altSmokeDensity;
}

void drawLattice(const Lattice2D& l)
{
	// advection step
	const real_t* lb = l.getData();
	for (int y = 0; y < N; y++)
		for (int x = 0; x < N; x++)
		{
			float srcX = x - lb[3 * (x + y * N) + 1];
			float srcY = y - lb[3 * (x + y * N) + 2];
			int srcXFloor = (int)floor(srcX);
			int srcXCeil = (int)ceil(srcX);
			int srcYFloor = (int)floor(srcY);
			int srcYCeil = (int)ceil(srcY);
			altSmokeDensity[x + y * N] =
				lb[3 * (srcXFloor + srcYFloor * N)] +
				lb[3 * (srcXFloor + srcYCeil * N)] +
				lb[3 * (srcXCeil + srcYFloor * N)] +
				lb[3 * (srcXCeil + srcYCeil * N)];
			altSmokeDensity[x + y * N] /= 4.0;
			std::swap(smokeDensity, altSmokeDensity);
		};

	// density visualization
	for (int y = 0; y < N; y++) 
		for (int x = 0; x < N; x++)
		{
			Uint8 intSmokeDensity = (Uint8)(smokeDensity[x + y * N] * 255.0);
			((Uint32*)pScreen->pixels)[x + y * pScreen->pitch / 4] =
				(intSmokeDensity << 16) | (intSmokeDensity << 8) |
				(intSmokeDensity);
		};

	SDL_Flip(pScreen);
}
#else
#error A valid draw method must be selected.
#endif

int main(int argc, char* argv[])
{
#if USE_FAST_LATTICE
	FastLattice2D l(N, myLoader);
#elif USE_SLOW_LATTICE
	SlowLattice2D l(N, myLoader);
#elif USE_ONE_PASS_LATTICE
	OnePassLattice2D l(N, myLoader);
#else
#error One lattice type must be selected.
#endif

	SDL_Init(SDL_INIT_EVERYTHING);

	initDrawLattice();

	SDL_Event ev;
	int i = 0;
	while(true)
	{
		Uint32 start = SDL_GetTicks();
		if (SDL_PollEvent(&ev) && ev.type == SDL_QUIT)
			break;

		drawLattice(l);

		l.step();

		Uint32 end = SDL_GetTicks();
		if (i++ % 10 == 0)
		{
			std::cout << "FPS: " << 1000.0 / (double)(end - start) << "\n";
		}
	}

	finishDrawLattice();

	SDL_Quit();

	return 0;
}
