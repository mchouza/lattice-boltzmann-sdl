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

SlowLattice2D::	SlowLattice2D(int n,
							  void (*loader)(int x, int y, real_t& rho, 
							  real_t& ux, real_t& uy)) :
n_(n)
{
	f_ = new real_t[n * n * Q];
	fTmp_ = new real_t[n * n * Q];
	accumBuffer_ = new real_t[(DIM + 1) * n * n];

	for (int x = 0; x < n; x++)
		for (int y = 0; y < n; y++)
		{
			real_t rho, ux, uy, uSqr;
			loader(x, y, rho, ux, uy);
			uSqr = ux * ux + uy * uy;
			for (int i = 0; i < Q; i++)
				f_[i + Q * x + n * Q * y] = getFEq(rho, ux, uy, uSqr, i);
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
	for (int x = 0; x < n_; x++)
		for (int y = 0; y < n_; y++)
		{
			real_t rho = accumBuffer_[3 * (x + n_ * y)];
			if (rho == 0.0f)
				continue;
			real_t ux = accumBuffer_[3 * (x + n_ * y) + 1] / rho;
			real_t uy = accumBuffer_[3 * (x + n_ * y) + 2] / rho;
			real_t uSqr = ux * ux + uy * uy;
		
			for (int i = 0; i < Q; i++)
			{
				real_t eqF = getFEq(rho, ux, uy, uSqr, i);
				f_[i + Q * x + Q * n_ * y] = 
					f_[i + Q * x + Q * n_ * y] * (1.0f - OMEGA) + OMEGA * eqF;
			}
		};
}

void SlowLattice2D::makePropagation()
{
	for (int x = 0; x < n_; x++)
		for (int y = 0; y < n_; y++)
			for (int i = 0; i < Q; i++)
			{
				int newX = (x + propD[i][0]) % n_;
				int newY = (y + propD[i][1]) % n_;
				if (newX < 0)
					newX += n_;
				if (newY < 0)
					newY += n_;
				
				fTmp_[i + Q * newX + n_ * Q * newY] =
					f_[i + Q * x + n_ * Q * y];
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
	for (int x = 0; x < n_; x++)
		for (int y = 0; y < n_; y++)
		{
			accumBuffer_[3 * (x + n_ * y)] = 0.0;
			accumBuffer_[3 * (x + n_ * y) + 1] = 0.0;
			accumBuffer_[3 * (x + n_ * y) + 2] = 0.0;
			for (int i = 0; i < Q; i++)
			{
				accumBuffer_[3 * (x + n_ * y)] += f_[i + Q * x + Q * n_ * y];
				accumBuffer_[3 * (x + n_ * y) + 1] +=
					propD[i][0] * f_[i + Q * x + Q * n_ * y];
				accumBuffer_[3 * (x + n_ * y) + 2] +=
					propD[i][1] * f_[i + Q * x + Q * n_ * y];
			}
		};
}

const real_t* SlowLattice2D::getData() const
{	
	return accumBuffer_;	
}
