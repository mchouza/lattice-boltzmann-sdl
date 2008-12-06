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
	f_ = new float[nx * ny * (Q + 1)];

	for (int i = 0; i < Q; i++)
		for (int y = 0; y < ny_; y++)
			for (int x = 0; x < nx_; x++)
				f_[x + nx_ * (y + ny_ * i)] = loader(x, y, i);

	for (int i = 0; i < Q; i++)
		offset[i] = 0;
}

Lattice2D::~Lattice2D()
{
	delete[] f_;
}

const float* Lattice2D::getSummedFs() const
{
	int n = nx_ * ny_;
	float* buffer = f_ + nx_ * ny_ * Q;
	for (int j = 0; j < n; j++)
			buffer[j] = f_[j];
	for (int i = 1; i < Q; i ++)
	{
		float* base = f_ + i * n;
		int j;
		if (offset[i] < 0)
		{
			for (j = 0; j < n + offset[i]; j++)
				buffer[j] += base[j - offset[i]];
			base -= n;
			for (; j < n; j++)
				buffer[j] += base[j - offset[i]];
		}
		else
		{
			for (j = 0; j < n - offset[i]; j++)
				buffer[j + offset[i]] += base[j];
			for (; j < n; j++)
				buffer[j + offset[i] - n] += base[j];
		}
	}
	return buffer;
}

void Lattice2D::step()
{
	makeCollisions();
	makePropagation();
}

void Lattice2D::makeCollisions()
{
	// FIXME: Null collision model
}

void Lattice2D::makePropagation()
{
	for (int i = 0; i < Q; i++)
	{
		offset[i] += propD[i][0] + nx_ * propD[i][1];
		offset[i] %= nx_ * ny_;
		if (offset[i] < 0)
			offset[i] += nx_* ny_;
	}
}

