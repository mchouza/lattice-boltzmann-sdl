#include "fast_lattice2d.h"
#include <algorithm>
#include <cassert>

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

FastLattice2D::FastLattice2D(int n,
							 void (*loader)(int x, int y, real_t& rho, 
											real_t& ux, real_t& uy)) :
n_(n), accumBufferUpdated_(false)
{
	f_ = new real_t[n * n * Q];
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

FastLattice2D::~FastLattice2D()
{
	delete[] f_;
	delete[] accumBuffer_;
}

void FastLattice2D::makeCollisions()
{
	const real_t* accumBuffer = getData();
	const int srcBlockSize = n_ * n_;
	
	for (int i = 0; i < Q; i++)
	{
		real_t* dataBlock = f_ + i * srcBlockSize;
		int j, k;
		real_t uSqr;

		for (j = offsets_[i], k = 0; j < srcBlockSize; j++, k++)
		{
			uSqr = sqr(accumBuffer[3 * j + 1]) + sqr(accumBuffer[3 * j + 2]);
			dataBlock[k] *= (1 - OMEGA);
			dataBlock[k] += getFEq(accumBuffer[3 * j], accumBuffer[3 * j + 1],
				accumBuffer[3 * j + 2], uSqr, i) * OMEGA;
		}

		for (j = 0; k < srcBlockSize; j++, k++)
		{
			uSqr = sqr(accumBuffer[3 * j + 1]) + sqr(accumBuffer[3 * j + 2]);
			dataBlock[k] *= (1 - OMEGA);
			dataBlock[k] += getFEq(accumBuffer[3 * j], accumBuffer[3 * j + 1],
				accumBuffer[3 * j + 2], uSqr, i) * OMEGA;
		}
	}
}

void FastLattice2D::makePropagation()
{
	const int blockSize = n_ * n_;

	for (int i = 0; i < Q; i++)
	{
		offsets_[i] += propD[i][0] + n_ * propD[i][1];
		offsets_[i] %= blockSize;
		if (offsets_[i] < 0)
			offsets_[i] += blockSize;
	}

	accumBufferUpdated_ = false;
}

void FastLattice2D::step()
{
	makeCollisions();
	makePropagation();
}

void FastLattice2D::updAccumBuffer() const
{
	int srcBlockSize = n_ * n_;

	for (int j = 0; j < srcBlockSize; j++)
	{
		accumBuffer_[3 * j] = f_[j];
		accumBuffer_[3 * j + 1] = (real_t)0.0;
		accumBuffer_[3 * j + 2] = (real_t)0.0;
	}

	for (int i = 1; i < Q; i++)
	{
		real_t propDX = (real_t)propD[i][0];
		real_t propDY = (real_t)propD[i][1];
		real_t* srcData = f_ + i * srcBlockSize;

		int j, k;

		for (j = offsets_[i], k = 0; j < srcBlockSize; j++, k++)
		{
			accumBuffer_[3 * j] += srcData[k];
			accumBuffer_[3 * j + 1] += propDX * srcData[k];
			accumBuffer_[3 * j + 2] += propDY * srcData[k];
		}

		for (j = 0; k < srcBlockSize; j++, k++)
		{
			accumBuffer_[3 * j] += srcData[k];
			accumBuffer_[3 * j + 1] += propDX * srcData[k];
			accumBuffer_[3 * j + 2] += propDY * srcData[k];
		}
	}

	for (int j = 0; j < srcBlockSize; j++)
	{
		accumBuffer_[3 * j + 1] /= accumBuffer_[3 * j];
		accumBuffer_[3 * j + 2] /= accumBuffer_[3 * j];
	}

	accumBufferUpdated_ = true;
}

const real_t* FastLattice2D::getData() const
{	
	if (!accumBufferUpdated_)
		updAccumBuffer();

	return accumBuffer_;	
}
