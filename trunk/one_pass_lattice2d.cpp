#include "one_pass_lattice2d.h"

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

	inline real_t getFEq(real_t rho, real_t ux, real_t uy, real_t uSqr, int i)
	{
		real_t dotProd = ux * propD[i][0] + uy * propD[i][1];
		real_t eqF = scD[i] * rho *
			(1.0f + 3.0f * dotProd + 4.5f * dotProd * dotProd - 1.5f * uSqr);
		return eqF;
	}

	inline real_t sqr(real_t x)
	{
		return x * x;
	}
}

OnePassLattice2D::OnePassLattice2D(int n,
								   void (*loader)(int x, int y, real_t& rho,
												  real_t& ux, real_t& uy)) :
n_(n)
{
	f_ = new real_t[n * n * Q];
	fAlt_ = new real_t[n * n * Q];
	accumBuffer_ = new real_t[(DIM + 1) * n * n];

	for (int i = 0; i < Q; i++)
		for (int x = 0; x < n; x++)
			for (int y = 0; y < n; y++)
			{
				real_t rho, ux, uy, uSqr;
				loader(x, y, rho, ux, uy);
				uSqr = ux * ux + uy * uy;
				f_[x + y * n + i * n * n] = getFEq(rho, ux, uy, uSqr, i);
			};

	for (int i = 0; i < Q; i++)
		offsets_[i] = 0;
}

OnePassLattice2D::~OnePassLattice2D()
{
	delete[] f_;
	delete[] fAlt_;
	delete[] accumBuffer_;
}

void OnePassLattice2D::makeCollisions()
{
	// FIXME: Fill
}

void OnePassLattice2D::makePropagation()
{
	const int blockSize = n_ * n_;

	for (int i = 0; i < Q; i++)
	{
		offsets_[i] += propD[i][0] + n_ * propD[i][1];
		offsets_[i] %= blockSize;
		if (offsets_[i] < 0)
			offsets_[i] += blockSize;
	}
}

void OnePassLattice2D::step()
{
	makeCollisions();
	makePropagation();
}

const real_t* OnePassLattice2D::getData() const
{	
	return accumBuffer_;	
}
