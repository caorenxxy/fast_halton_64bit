// Copyright (c) 2012 Leonhard Gruenschloss (leonhard@gruenschloss.org)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "halton_enum.h"
#include "halton_sampler.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <random>
#include <string>
using namespace std;

// Random number generator wrapper for drand48.
struct Drand48
{	
	Drand48() : rng(123) {}
    template <typename Integer>
    Integer operator()(const Integer n) 
    {
        return static_cast<Integer>(uniform(rng) * n);
    }
	std::mt19937_64 rng;
	std::uniform_real_distribution<float> uniform;
};

void test(const char* const filename, const unsigned width, const unsigned height, const bool faure)
{
    ofstream out(filename);
    assert(out);

    // Initialize the sampler, either with Faure permutations or randomized digit permutations.
    Halton_sampler halton_sampler;
    if (faure)
    {
        halton_sampler.init_faure();
    }
    else
    {
        Drand48 rng;
        halton_sampler.init_random(rng);
    }

    // Enumerate samples per pixel for the given resolution.
    const Halton_enum halton_enum(width, height);
    const unsigned samples_per_pixel = 4;
    assert(samples_per_pixel < halton_enum.get_max_samples_per_pixel());
    for (unsigned y = 0; y < height; ++y) // Iterate over rows.
    {
        for (unsigned x = 0; x < width; ++x) // Iterate over columns.
        {
            for (unsigned i = 0; i < samples_per_pixel; ++i) // Iterate over samples in the pixel.
            {
                // Retrieve the index of the corresponding sample.
                const unsigned index = halton_enum.get_index(i, x, y);
                // Draw three components.
                const float sx = halton_sampler.sample(0, index);
                const float sy = halton_sampler.sample(1, index);
                const float sz = halton_sampler.sample(2, index);
                // Rescale the first two components to match the pixel raster.
                const float rx = halton_enum.scale_x(sx);
                const float ry = halton_enum.scale_y(sy);
                // Validate that we're inside the pixel, taking floating-point inaccuracies
                // into account.
                assert(x <= rx + 1e-3f && rx - 1e-3f < x + 1);
                assert(y <= ry + 1e-3f && ry - 1e-3f < y + 1);
                // Finally, write the samples to the output stream, in gnuplot format.
                out << rx << " " << ry << " " << sz << endl;
            }
        }
    }

    out.close();
}

int main(int, char**)
{
    //test("faure.dat", 640, 480, true);
    //srand48(5784); // Make reproducible.
    //test("random.dat", 2048, 1152, false);
	
	Halton_sampler sampler;
	sampler.init_faure();
	for (unsigned long long i = 0; i < 100; ++i) {
		std::cout << sampler.sample(0, i) << std::endl;
	}
	/*
	unsigned index = 123456789;

	std::cout << 0x3f800000ul << std::endl << 0x3f800000u << std::endl;
	union Result
	{
		unsigned u;
		float f;
	} result; // Write reversed bits directly into floating-point mantissa.

	union Result2
	{
		unsigned long long u;
		float f;
	} result2; // Write reversed bits directly into floating-point mantissa.
	result.u = 0x3f800000u | (index >> 9);
	result2.u = 0x3f800000ul | (index >> 9);
	std::cout << result.f - 1.f << std::endl << result2.f - 1.f << std::endl;*/
	
	
	/*
	std::string bit = "";
	while (x > 0) {
		bit += (x & 1) + '0';
		x >>= 1;
	}
	std::reverse(bit.begin(), bit.end());
	for (int i = 0; i < bit.size(); ++i) {
		std::cout << bit[i];
		if (i == 0 || i == 8) std::cout << "|";
	}std::cout << std::endl;
	std::cout << sampler.sample(0, 1235) << std::endl;

	union Result
	{
		unsigned u;
		float f;
	} test;

	*/
    return 0;
}

