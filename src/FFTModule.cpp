#include "FFTModule.h"

// Internal FFT buffers (only used in this file)
static double vReal[FFT_SIZE];
static double vImag[FFT_SIZE];

// Create FFT object using these buffers
static arduinoFFT FFT(vReal, vImag, FFT_SIZE, SAMPLING_FREQUENCY);

void fft_computeSpectrum(const float* inSignal,
                         float* outSpectrum,
                         uint16_t length)
{
    // 1) Decide how many samples to use (max = FFT_SIZE)
    uint16_t n = (length < FFT_SIZE) ? length : FFT_SIZE;

    // 2) Copy input signal into vReal and clear vImag
    uint16_t i;
    for (i = 0; i < n; i++) {
        vReal[i] = (double)inSignal[i];  // preprocessed magnitude signal
        vImag[i] = 0.0;                  // no imaginary part
    }

    // 3) Zero-pad the rest if we have fewer than FFT_SIZE samples
    for (i = n; i < FFT_SIZE; i++) {
        vReal[i] = 0.0;
        vImag[i] = 0.0;
    }

    // 4) Apply windowing to reduce spectral leakage
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);

    // 5) Compute the FFT (time-domain -> frequency-domain)
    FFT.Compute(FFT_FORWARD);

    // 6) Convert complex output to magnitude spectrum
    FFT.ComplexToMagnitude();

    // 7) Copy the useful half of the spectrum (0 .. FFT_SIZE/2) to outSpectrum
    //    outSpectrum must be allocated for at least (FFT_SIZE/2 + 1) floats.
    uint16_t maxBin = FFT_SIZE / 2;    // Nyquist bin
    for (i = 0; i <= maxBin; i++) {
        outSpectrum[i] = (float)vReal[i];
    }
}

// outSpectrum has 65 elements (0–64), each is the magnitude at a specific frequency.
//frequency = i * (sampling_frequency / FFT_SIZE)
//frequency = i * (52 / 128) = i * 0.40625 Hz

