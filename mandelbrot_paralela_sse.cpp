/*
Modified from: http://rosettacode.org/wiki/Mandelbrot_set#PPM_non_interactive
c program:
--------------------------------
1. draws Mandelbrot set for Fc(z)=z*z +c
using Mandelbrot algorithm ( boolean escape time )
-------------------------------
2. technique of creating ppm file is  based on the code of Claudio Rocchini
http://en.wikipedia.org/wiki/Image:Color_complex_plot.jpg
create 24 bit color graphic file ,  portable pixmap file = PPM
see http://en.wikipedia.org/wiki/Portable_pixmap
to see the file use external application ( graphic viewer)
*/
#include <iostream>
#include <stdio.h>
#include <time.h>

int main()
{
	clock_t start, end;
	double cpu_time_used;

	// Screen (integer) coordinates
	const int iXmax = 16384;
	const int iYmax = 16384;

	// World (double) coordinate = parameter plane
	double Cx_, Cy;
	const double CxMin = -2.5;
	const double CxMax = 1.5;
	const double CyMin = -2.0;
	const double CyMax = 2.0;
	const double Two = 2.0;

	double PixelWidth = (CxMax - CxMin) / iXmax;
	double PixelHeight = (CyMax - CyMin) / iYmax;

	// Color component ( R or G or B) is coded from 0 to 255
	// It is 24 bit color RGB file
	const int MaxColorComponentValue = 255;

	// File
	FILE * fp;
	char *filename = "mandelbrot.ppm";
	static unsigned char color[3];

	// Z = Zx + Zy * i
	// Z0 = 0 
	double Zx, Zy;

	// Zx2 = Zx * Zx
	// Zy2= Zy * Zy
	double Zx2, Zy2;

	int Iteration;
	const int IterationMax = 256;

	// Bail-out value , radius of circle
	const double EscapeRadius = 2;
	double ER2 = EscapeRadius*EscapeRadius;

	// Create a new file, give it a name and open it in binary mode
	fp = fopen(filename, "wb");

	// Write ASCII header to the file
	fprintf(fp, "P6\n %d\n %d\n %d\n", iXmax, iYmax, MaxColorComponentValue);

	// Compute and write image data bytes to the file
	start = clock();
	for (double iY = 0; iY < iYmax; iY++)
	{
		__asm
		{
			// Cy = CyMin + iY*PixelHeight;
			movupd xmm0, [CyMin]
			movupd xmm1, [iY]
			movupd xmm2, [PixelHeight]
			mulpd xmm1, xmm2
			addpd xmm0, xmm1
			movupd[Cy], xmm0
		}

		if (std::fabs(Cy) < PixelHeight / 2)
		{
			// Main antenna
			Cy = 0.0;
		}

		for (double iX = 0; iX < iXmax; iX++)
		{
			// initial value of orbit = critical point Z= 0
			Zx = 0.0;
			Zy = 0.0;

			__asm
			{
				//Cx = CxMin + iX*PixelWidth;
				movupd xmm0, [CxMin]
				movupd xmm1, [iX]
				movupd xmm2, [PixelWidth]
				mulpd xmm1, xmm2
				addpd xmm0, xmm1
				movupd[Cx_], xmm0

				// Zx2 = Zx*Zx;
				movupd xmm0, [Zx]
				mulpd xmm0, xmm0
				movupd[Zx2], xmm0

				//Zy2 = Zy*Zy;
				movupd xmm0, [Zy]
				mulpd xmm0, xmm0
				movupd[Zy2], xmm0
			}
			for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++)
			{
				__asm
				{
					//Zy = 2 * Zx*Zy + Cy;
					movupd xmm0, [Two]
					movupd xmm1, [Zx]
					movupd xmm2, [Zy]
					movupd xmm3, [Cy]
					mulpd xmm0, xmm1
					mulpd xmm0, xmm2
					addpd xmm0, xmm3
					movupd[Zy], xmm0

					//Zx = Zx2 - Zy2 + Cx_;
					movupd xmm0, [Zx2]
					movupd xmm1, [Zy2]
					movupd xmm2, [Cx_]
					subpd xmm0, xmm1
					addpd xmm0, xmm2
					movupd[Zx], xmm0

					//Zx2 = Zx*Zx;
					movupd xmm0, [Zx]
					mulpd xmm0, xmm0
					movupd[Zx2], xmm0

					//Zy2 = Zy*Zy;
					movupd xmm0, [Zy]
					mulpd xmm0, xmm0
					movupd[Zy2], xmm0
				}
			}

			if (Iteration == IterationMax)
			{ 
				// Interior of Mandelbrot set = black
				color[0] = 0;
				color[1] = 0;
				color[2] = 0;
			}
			else
			{ 
				// Exterior of Mandelbrot set = white
				color[0] = ((IterationMax - Iteration) % 8) * 63; 
				color[1] = ((IterationMax - Iteration) % 4) * 127;
				color[2] = ((IterationMax - Iteration) % 2) * 255;
			};
			fwrite(color, 1, 3, fp);

		}
	}
	end = clock();

	cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
	std::cout << "time = " << cpu_time_used << " seconds\n";
	fclose(fp);
	int TESTE;
	std::cin >> TESTE;
	return 0;
}