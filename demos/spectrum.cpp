#include "graphics.h"
#include "audio.h"

#include <complex>
#include <valarray>

#define WAVELENGTH_SUB 2048

const double PI = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

// Cooley-Tukey FFT (in-place, breadth-first, decimation-in-frequency)
// Better optimized but less intuitive
void fft(CArray& x)
{
	// DFT
	unsigned int N = x.size(), k = N, n;
	double thetaT = 3.14159265358979323846264338328L / N;
	Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
	while (k > 1)
	{
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < N; a += n)
			{
				unsigned int b = a + k;
				Complex t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}
	// Decimate
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++)
	{
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a)
		{
			Complex t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}
}

void fftMag(CArray& x)
{
    fft(x);
    for (int i = 0; i < x.size(); i++)
    {
        x[i] = sqrt(x[i].real() * x[i].real() + x[i].imag() * x[i].imag());
    }
}

#define BARS 120
Complex sub[WAVELENGTH_SUB];
int s;
Render r[BARS];

int spectrum(int argc, char** argv) 
{
	if (argc <= 1){
		perror("no wav file given\n");
		exit(1);
	}
	
	startAudio();
	WAV wav;
	wav.load(argv[1]);
	AudioSource a;
	a.init(wav);
	
	createWindow();
	
	initGraphics();

	Texture t;
	t.create("white.png");

	for (int i = 0; i < BARS; i++)
	{
		r[i].create(t);
		r[i].scale = V3( 1 / (float)BARS, 1.0f, 0.0f );
		r[i].pos = V3( (-0.5f + 0.5 / (float)BARS) + (i * (1 / (float)BARS)), 0.75f , 1.0f);
		r[i].color = {0.0f, 1.0f, 1.0f, 1.0f};
	}
	
	a.play();
	double timeStart = currTime();

	while (renderFrame())
	{
		double cTime = currTime() - timeStart;
		s = (int)(44100.0 * cTime);

		if (wav.data.size() / 2 > WAVELENGTH_SUB + s) {
			
			for (int i = 0; i < WAVELENGTH_SUB; i++)
			{
				sub[i] = wav.data[(i + s) * 2] / 32767.0f;
			}
			
			CArray data(sub, WAVELENGTH_SUB);
			fftMag(data);

			for (int i = 0; i < BARS; i++)
			{
				float ampl = data[i+1].real();
				//ampl = 20 * log10(ampl);
				r[i].pos.y = 0.75f - (float)(ampl * 0.0005f);
			}

		}
		
	}

	cleanGraphics();
}

