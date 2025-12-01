/*
#ifndef SIGNAL_UI_BRIDGE_H
#define SIGNAL_UI_BRIDGE_H

#include <Arduino.h>
#include "FFTModule.h"

class SignalUIBridge {
public:
    static void begin();
    static void update();

    // getters for DiagnosticScreen
    static float  getProcessedMagnitude();
    static bool   hasNewFFT();
    static float  getFFTPeak();

private:
    static float latestMag;
    static bool  fftReady;
    static float fftPeak;
};

#endif
*/



#ifndef SIGNAL_UI_BRIDGE_H
#define SIGNAL_UI_BRIDGE_H

#include <Arduino.h>
#include "FFTModule.h"

class SignalUIBridge {
public:
    static void begin();
    static void update();

    // ===== GETTERS for DiagnosticScreen =====
    static float  getProcessedMagnitude();
    static bool   hasNewFFT();
    static float  getFFTPeak();

    // NEW → get full FFT spectrum
    static bool   getFFTSpectrum(float *outSpectrum,
                                 uint16_t maxBins,
                                 uint16_t &numBins);

private:
    static float latestMag;
    static bool  fftReady;
    static float fftPeak;

    // NEW: store full FFT bins here
    static float latestSpectrum[(FFT_SIZE/2)+1];
    static uint16_t latestSpectrumBins;
};

#endif
