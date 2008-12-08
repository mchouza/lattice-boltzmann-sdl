#include <algorithm>
#include "slow_lattice2d.h"

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

SlowLattice2D::SlowLattice2D(int nx, int ny, float (*loader)(int, int, int)) :
nx_(nx), ny_(ny)
{
	f_ = new float[nx * ny * Q];
	fTmp_ = new float[nx * ny * Q];
	accumBuffer_ = new float[(DIM + 1) * nx * ny];

	for (int x = 0; x < nx; x++)
		for (int y = 0; y < ny; y++)
			for (int i = 0; i < Q; i++)
				f_[i + Q * x + nx * Q * y] = loader(x, y, i);
}

SlowLattice2D::~SlowLattice2D()
{
	delete[] f_;
	delete[] fTmp_;
	delete[] accumBuffer_;
}

void SlowLattice2D::makeCollisions()
{
	// FIXME: EMPTY
}

void SlowLattice2D::makePropagation()
{
	for (int x = 0; x < nx_; x++)
		for (int y = 0; y < ny_; y++)
			for (int i = 0; i < Q; i++)
			{
				int newX = (x + propD[i][0]) % nx_;
				int newY = (y + propD[i][1]) % ny_;
				if (newX < 0)
					newX += nx_;
				if (newY < 0)
					newY += ny_;
				
				fTmp_[i + Q * newX + nx_ * Q * newY] =
					f_[i + Q * x + nx_ * Q * y];
			};

	std::swap(f_, fTmp_);
}

void SlowLattice2D::step()
{
	makeCollisions();
	makePropagation();
}

const float* SlowLattice2D::getData() const
{
	for (int x = 0; x < nx_; x++)
		for (int y = 0; y < ny_; y++)
		{
			accumBuffer_[3 * (x + nx_ * y)] = 0.0;
			for (int i = 0; i < Q; i++)
			{
				accumBuffer_[3 * (x + nx_ * y)] += f_[i + Q * x + Q * nx_ * y];
				accumBuffer_[3 * (x + nx_ * y) + 1] +=
					propD[i][0] * f_[i + Q * x + Q * nx_ * y];
				accumBuffer_[3 * (x + nx_ * y) + 2] +=
					propD[i][1] * f_[i + Q * x + Q * nx_ * y];
			}
		};
	
	return accumBuffer_;	
}
