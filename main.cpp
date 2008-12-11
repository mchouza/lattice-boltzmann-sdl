#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>
#include "fast_lattice2d.h"
#include "slow_lattice2d.h"

//#define USE_FAST_LATTICE

namespace
{
	const int W = 512;
	const int H = 512;

	const int NX = 50;
	const int NY = 50;
	const int NPOINTS = NX * NY;

	const real_t SPEED = 0.7;

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
	ux = (real_t)(SPEED / (scSqDist + 1.0));
}

void drawLattice(const Lattice2D& l, float pointsVec[NPOINTS][2])
{
	// move points with fluid velocity
	for (int i = 0; i < NPOINTS; i++)
	{
		int x = (int)pointsVec[i][0] % W;
		int y = (int)pointsVec[i][1] % H;
		if (x < 0)
			x += W;
		if (y < 0)
			y += H;
		
		pointsVec[i][0] += l.getData()[3 * (x + y * W) + 1];
		pointsVec[i][1] += l.getData()[3 * (x + y * W) + 2];
	}
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glBegin(GL_POINTS);
		for (int i = 0; i < NPOINTS; i++)
		{
			int x = (int)pointsVec[i][0] % W;
			int y = (int)pointsVec[i][1] % H;
			if (x < 0)
				x += W;
			if (y < 0)
				y += H;
			glVertex2i(x, y);
		}
	glEnd();
}

int main(int argc, char* argv[])
{
#ifdef USE_FAST_LATTICE
	FastLattice2D l(W, H, myLoader);
#else
	SlowLattice2D l(W, H, myLoader);
#endif

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_Surface* pScreen =
		SDL_SetVideoMode(W, H, 32, SDL_OPENGL);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0, W, H, 0);

	float pointsVec[NPOINTS][2];
	for (int i = 0; i < NX; i++)
	{
		for (int j = 0; j < NY; j++)
		{
			pointsVec[i + NX * j][0] = ((real_t)i / NX) * W;
			pointsVec[i + NX * j][1] = ((real_t)j / NY) * H;
		}
	}

	SDL_Event ev;
	int i = 0;
	while(true)
	{
		Uint32 start = SDL_GetTicks();
		if (SDL_PollEvent(&ev) && ev.type == SDL_QUIT)
			break;

		drawLattice(l, pointsVec);

		SDL_GL_SwapBuffers();

		l.step();
		Uint32 end = SDL_GetTicks();
		if (i++ % 10 == 0)
		{
			std::cout << "FPS: " << 1000.0 / (double)(end - start) << "\n";
		}
	}

	SDL_Quit();

	return 0;
}
