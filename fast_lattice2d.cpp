#include "fast_lattice2d.h"

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

void FastLattice2D::updAggrData() const
{	// FIXME: CLEANUP WITHOUT LOSING FPS
	if (freshAggrData_)
		return;

	int n = nx_ * ny_;
	float* buffer = f_ + n * Q;

	for (int j = 0, k = 0; j < n; j++, k += 3)
			buffer[k] = f_[j];

	for (int i = 1; i < Q; i ++)
	{
		float* base = f_ + i * n;
		int j, k;
		if (offset_[i] < 0)
		{
			for (j = -offset_[i], k = 0; j < n; j++, k += 3)
			{
				buffer[k] += base[j];
				buffer[k + 1] += propD[i][0] * base[j];
				buffer[k + 2] += propD[i][1] * base[j];
			}
			base -= n;
			for (; j < n - offset_[i]; j++, k += 3)
			{
				buffer[k] += base[j];
				buffer[k + 1] += propD[i][0] * base[j];
				buffer[k + 2] += propD[i][1] * base[j];
			}
		}
		else
		{
			for (j = 0, k = 3 * offset_[i]; j < n - offset_[i]; j++, k += 3)
			{
				buffer[k] += base[j];
				buffer[k + 1] += propD[i][0] * base[j];
				buffer[k + 2] += propD[i][1] * base[j];
			}
			k -= 3 * n;
			for (; j < n; j++, k += 3)
			{
				buffer[k] += base[j];
				buffer[k + 1] += propD[i][0] * base[j];
				buffer[k + 2] += propD[i][1] * base[j];
			}
		}
	}

	freshAggrData_ = true;
}

FastLattice2D::FastLattice2D(int nx, int ny, float (*loader)(int, int, int)) :
nx_(nx),
ny_(ny)
{
	f_ = new float[nx * ny * (Q + 1 + DIM)];

	for (int i = 0; i < Q; i++)
		for (int y = 0; y < ny_; y++)
			for (int x = 0; x < nx_; x++)
				f_[x + nx_ * (y + ny_ * i)] = loader(x, y, i);

	for (int i = 0; i < Q; i++)
		offset_[i] = 0;

	updAggrData();
}

FastLattice2D::~FastLattice2D()
{
	delete[] f_;
}

const float* FastLattice2D::getData() const
{
	updAggrData();
	return f_ + nx_ * ny_ * Q;
}

void FastLattice2D::step()
{
	makeCollisions();
	makePropagation();
}

void FastLattice2D::makeCollisions()
{
	// FIXME: EMPTY
}

void FastLattice2D::makePropagation()
{	
	for (int i = 0; i < Q; i++)
	{
		offset_[i] += propD[i][0] + nx_ * propD[i][1];
		offset_[i] %= nx_ * ny_;
		if (offset_[i] < 0)
			offset_[i] += nx_* ny_;
	}

	freshAggrData_ = false;
}
