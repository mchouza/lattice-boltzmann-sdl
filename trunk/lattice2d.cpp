#include "lattice2d.h"
#include <cstring>

namespace
{
	int propD[][2] =
	{
		{ 0,  0},
		{ 1,  0},
		{ 1,  1},
		{ 0,  1},
		{-1,  1},
		{-1,  0},
		{-1, -1},
		{ 0, -1},
		{ 1, -1},
	};
}

Lattice2D::Lattice2D(int nx, int ny, float (*loader)(int, int, int)) :
nx_(nx),
ny_(ny)
{
	f_ = new float[nx * ny * Q];
	shadowF_ = new float[nx * ny * Q];
	for (int x = 0; x < (int)nx_; x++)
		for (int y = 0; y < (int)ny_; y++)
			for (int i = 0; i < Q; i++)
				f_[i + Q * x + Q * nx_ * y] = loader(x, y, i);
}

Lattice2D::~Lattice2D()
{
	delete[] f_;
	delete[] shadowF_;
}

void Lattice2D::step()
{
	makeCollisions();
	makePropagation();
}

void Lattice2D::normalize(int& x, int& y) const
{
	x %= nx_;
	y %= ny_;
	if (x < 0)
		x += nx_;
	if (y < 0)
		y += ny_;
}

void Lattice2D::makeCollisions()
{
	// FIXME: Null collision model
}

#if 0 /* 12 FPS */
void Lattice2D::makePropagation()
{
	for (int x = 0; x < nx_; x++)
		for (int y = 0; y < ny_; y++)
			for (int i = 0; i < Q; i++)
				getShadowF(x + propD[i][0], y + propD[i][1], i) =
					getF(x, y, i);
				
	memcpy(f_, shadowF_, Q * nx_ * ny_ * sizeof(float));
}
#elif 0 /* 15 FPS */
void Lattice2D::makePropagation()
{
	for (int x = 0; x < nx_; x++)
		for (int y = 0; y < ny_; y++)
			for (int i = 0; i < Q; i++)
			{
				int xp = x + propD[i][0];
				int yp = y + propD[i][1];
				normalize(xp, yp);
				shadowF_[i + Q * (xp + nx_ * yp)] =
					f_[i + Q * (x + nx_ * y)];
			}
			
	memcpy(f_, shadowF_, Q * nx_ * ny_ * sizeof(float));
}
#else /* 19 FPS */
void Lattice2D::makePropagation()
{
	int offset[sizeof(propD) / sizeof(propD[0])];
	for (int i = 0; i < sizeof(propD) / sizeof(propD[0]); i++)
		offset[i] = Q * (propD[i][0] + propD[i][1] * nx_);
	for (int i = 0; i < Q * nx_ * ny_; i++)
	{
		int ip = (i + offset[i % Q]) % (Q * nx_ * ny_);
		if (ip < 0)
			ip += Q * nx_ * ny_;
		shadowF_[ip] = f_[i];
	}
			
	memcpy(f_, shadowF_, Q * nx_ * ny_ * sizeof(float));
}
#endif