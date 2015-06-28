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
	__asm
	{
		finit
	}
	for (double iY = 0; iY < iYmax; iY++)
	{
		__asm
		{
			// Cy = CyMin + iY*PixelHeight;
			fld [CyMin]
			fld [iY]
			fld [PixelHeight]
			fmulp ST(1), ST(0)
			faddp ST(1), ST(0)
			fstp [Cy]
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
				fld[CxMin]
				fld[iX]
				fld[PixelWidth]
				fmulp ST(1), ST(0)
				faddp ST(1), ST(0)
				fstp[Cx_]

				// Zx2 = Zx*Zx;
				fld[Zx]
				fmul ST(0), ST(0)
				fstp[Zx2]

				//Zy2 = Zy*Zy;
				fld[Zy]
				fmul ST(0), ST(0)
				fstp[Zy2]
			}

			for (Iteration = 0; Iteration < IterationMax && ((Zx2 + Zy2) < ER2); Iteration++)
			{
				__asm
				{
					//Zy = 2 * Zx*Zy + Cy;
					fld[Cy]
					fld[Two]
					fld[Zx]
					fld[Zy]
					fmulp ST(1), ST(0)
					fmulp ST(1), ST(0)
					faddp ST(1), ST(0)
					fstp[Zy]

					//Zx = Zx2 - Zy2 + Cx_;
					fld[Zx2]
					fld[Zy2]
					fld[Cx_]
					faddp ST(2), ST(0)
					fsubp ST(1), ST(0)
					fstp[Zx]

					//Zx2 = Zx*Zx;
					fld[Zx]
					fmul ST(0), ST(0)
					fstp[Zx2]

					//Zy2 = Zy*Zy;
					fld[Zy]
					fmul ST(0), ST(0)
					fstp[Zy2]
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
			}
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