#include <algorithm>
#include <cassert>
#include "slow_lattice2d.h"

namespace
{
	const int propD[][2] =
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

	const real_t scD[] =
	{
		4.0f / 9.0f,
		1.0f / 9.0f,  // E
		1.0f / 36.0f, // NE
		1.0f / 9.0f,  // N
		1.0f / 36.0f, // NW
		1.0f / 9.0f,  // W
		1.0f / 36.0f, // SW
		1.0f / 9.0f,  // S
		1.0f / 36.0f, // SE
	};

	const real_t OMEGA = 0.5f;

	real_t getFEq(real_t rho, real_t ux, real_t uy, real_t uSqr, int i)
	{
		real_t dotProd = ux * propD[i][0] + uy * propD[i][1];
		real_t eqF = scD[i] * rho *
			(1.0f + 3.0f * dotProd + 4.5f * dotProd * dotProd - 1.5f * uSqr);
		return eqF;
	}
}

SlowLattice2D::	SlowLattice2D(int nx, int ny,
							  void (*loader)(int x, int y, real_t& rho, 
							  real_t& ux, real_t& uy)) :
nx_(nx), ny_(ny)
{
	f_ = new real_t[nx * ny * Q];
	fTmp_ = new real_t[nx * ny * Q];
	accumBuffer_ = new real_t[(DIM + 1) * nx * ny];

	for (int x = 0; x < nx; x++)
		for (int y = 0; y < ny; y++)
		{
			real_t rho, ux, uy, uSqr;
			loader(x, y, rho, ux, uy);
			uSqr = ux * ux + uy * uy;
			for (int i = 0; i < Q; i++)
				f_[i + Q * x + nx * Q * y] = getFEq(rho, ux, uy, uSqr, i);
		}

	updAccumBuffer();
}

SlowLattice2D::~SlowLattice2D()
{
	delete[] f_;
	delete[] fTmp_;
	delete[] accumBuffer_;
}

void SlowLattice2D::makeCollisions()
{
	for (int x = 0; x < nx_; x++)
		for (int y = 0; y < ny_; y++)
		{
			real_t rho = accumBuffer_[3 * (x + nx_ * y)];
			if (rho == 0.0f)
				continue;
			real_t ux = accumBuffer_[3 * (x + nx_ * y) + 1] / rho;
			real_t uy = accumBuffer_[3 * (x + nx_ * y) + 2] / rho;
			real_t uSqr = ux * ux + uy * uy;
		
			for (int i = 0; i < Q; i++)
			{
				real_t eqF = getFEq(rho, ux, uy, uSqr, i);
				f_[i + Q * x + Q * nx_ * y] = 
					f_[i + Q * x + Q * nx_ * y] * (1.0f - OMEGA) + OMEGA * eqF;
			}
		};
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
	updAccumBuffer();
	makeCollisions();
	makePropagation();
}

void SlowLattice2D::updAccumBuffer()
{
	for (int x = 0; x < nx_; x++)
		for (int y = 0; y < ny_; y++)
		{
			accumBuffer_[3 * (x + nx_ * y)] = 0.0;
			accumBuffer_[3 * (x + nx_ * y) + 1] = 0.0;
			accumBuffer_[3 * (x + nx_ * y) + 2] = 0.0;
			for (int i = 0; i < Q; i++)
			{
				accumBuffer_[3 * (x + nx_ * y)] += f_[i + Q * x + Q * nx_ * y];
				accumBuffer_[3 * (x + nx_ * y) + 1] +=
					propD[i][0] * f_[i + Q * x + Q * nx_ * y];
				accumBuffer_[3 * (x + nx_ * y) + 2] +=
					propD[i][1] * f_[i + Q * x + Q * nx_ * y];
			}
		};
}

const real_t* SlowLattice2D::getData() const
{	
	return accumBuffer_;	
}
