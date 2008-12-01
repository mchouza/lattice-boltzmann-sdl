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
	buffer_ = new float[nx + 1];

	for (int i = 0; i < Q; i++)
		for (int y = 0; y < ny_; y++)
			for (int x = 0; x < nx_; x++)
				f_[x + nx_ * (y + ny_ * i)] = loader(x, y, i);
}

Lattice2D::~Lattice2D()
{
	delete[] f_;
	delete[] buffer_;
}

void Lattice2D::getSummedFs(float* buffer) const
{
	int n = nx_ * ny_;
	for (int j = 0; j < n; j++)
			buffer[j] = f_[j];
	for (int i = n; i < Q * n; i += n)
		for (int j = 0; j < n; j++)
			buffer[j] += f_[i + j];
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

// Old way: 19 FPS
void Lattice2D::makePropagation()
{
	int n = nx_ * ny_;
	for (int i = 0; i < Q; i++)
	{
		int offset = propD[i][0] + propD[i][1] * nx_;
		float* base = f_ + i * n;
		
		// Now, the segment of the data vector [n * i, n * (i + 1))
		// needs to be rotated by 'offset' places
		if (offset < 0)
		{
			// If the offset is negative, the first 'offset' values need to be
			// saved
			memcpy(buffer_, base, -offset * sizeof(float));

			// Makes the movement
			memmove(base, base + offset, n + offset);

			// Adds the rotated section
			memcpy(base + n + offset, buffer_, -offset * sizeof(float));
		}
		else
		{
			// If the offset is positive, the lase 'offset' values need to be
			// saved
			memcpy(buffer_, base + n - offset, offset * sizeof(float));

			// Makes the movement
			memmove(base + offset, base, n - offset);

			// Adds the rotated section
			memcpy(base, buffer_, offset * sizeof(float));
		}
	}
}
