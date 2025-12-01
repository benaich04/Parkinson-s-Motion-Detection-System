/*

#include "SignalUIBridge.h"
#include "IMU_I2C.h"
#include "SignalProcessor.h"

static SignalProcessor signalProc;

float SignalUIBridge::latestMag = 0.0f;
bool  SignalUIBridge::fftReady  = false;
float SignalUIBridge::fftPeak   = 0.0f;

static unsigned long lastSample = 0;
static const unsigned long SAMPLE_MS = 50;

void SignalUIBridge::begin() {
    IMU_I2C_init();
    signalProc.begin();
}

void SignalUIBridge::update() {
    unsigned long now = millis();
    if (now - lastSample < SAMPLE_MS) return;
    lastSample = now;

    int16_t x, y, z;
    IMU_I2C_readAccel(&x, &y, &z);

    float raw, proc;
    bool fftFlag = false;
    signalProc.update(x, y, z, raw, proc, fftFlag);

    latestMag = proc;

    if (fftFlag) {
        float spectrum[(FFT_SIZE/2)+1];
        uint16_t bins;
        signalProc.getSpectrum(spectrum, bins);

        float peak = 0;
        for (uint16_t i = 0; i < bins; i++)
            if (spectrum[i] > peak) peak = spectrum[i];

        fftPeak = peak;
        fftReady = true;
    }
}

float SignalUIBridge::getProcessedMagnitude() {
    return latestMag;
}

bool SignalUIBridge::hasNewFFT() {
    if (fftReady) {
        fftReady = false;
        return true;
    }
    return false;
}

float SignalUIBridge::getFFTPeak() {
    return fftPeak;
}
*/








#include "SignalUIBridge.h"
#include "IMU_I2C.h"
#include "SignalProcessor.h"

static SignalProcessor signalProc;

// Existing static members
float SignalUIBridge::latestMag = 0.0f;
bool  SignalUIBridge::fftReady  = false;
float SignalUIBridge::fftPeak   = 0.0f;

// NEW static buffers
float SignalUIBridge::latestSpectrum[(FFT_SIZE/2)+1];
uint16_t SignalUIBridge::latestSpectrumBins = 0;

static unsigned long lastSample = 0;
static const unsigned long SAMPLE_MS = 50;

void SignalUIBridge::begin() {
    IMU_I2C_init();
    signalProc.begin();
}

void SignalUIBridge::update() {
    unsigned long now = millis();
    if (now - lastSample < SAMPLE_MS) return;
    lastSample = now;

    int16_t x, y, z;
    IMU_I2C_readAccel(&x, &y, &z);

    float raw, proc;
    bool fftFlag = false;
    signalProc.update(x, y, z, raw, proc, fftFlag);

    latestMag = proc;

    if (fftFlag) {
        float spectrum[(FFT_SIZE/2)+1];
        uint16_t bins;
        signalProc.getSpectrum(spectrum, bins);

        // Save full spectrum internally
        latestSpectrumBins = bins;
        for (uint16_t i = 0; i < bins; i++) {
            latestSpectrum[i] = spectrum[i];
        }

        // Compute peak (kept for compatibility with old UI)
        float peak = 0;
        for (uint16_t i = 0; i < bins; i++)
            if (spectrum[i] > peak) peak = spectrum[i];

        fftPeak = peak;
        fftReady = true;
    }
}

float SignalUIBridge::getProcessedMagnitude() {
    return latestMag;
}

bool SignalUIBridge::hasNewFFT() {
    if (fftReady) {
        fftReady = false;   // reset flag
        return true;
    }
    return false;
}

float SignalUIBridge::getFFTPeak() {
    return fftPeak;
}

// =========================================================
// NEW FUNCTION – Return the stored FFT spectrum to the UI
// =========================================================
bool SignalUIBridge::getFFTSpectrum(float *outSpectrum,
                                    uint16_t maxBins,
                                    uint16_t &numBins)
{
    if (latestSpectrumBins == 0) {
        numBins = 0;
        return false;
    }

    uint16_t n = (latestSpectrumBins < maxBins) ?
                  latestSpectrumBins : maxBins;

    for (uint16_t i = 0; i < n; i++)
        outSpectrum[i] = latestSpectrum[i];

    numBins = n;
    return true;
}
