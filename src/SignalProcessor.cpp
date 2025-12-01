#include "SignalProcessor.h"

SignalProcessor::SignalProcessor()
: spectrumBins((FFT_SIZE / 2) + 1)
{
    // Nothing else here; real init happens in begin()
}

void SignalProcessor::begin() {
    imuBuffer_init(&imuBuffer);

    // Optional: clear arrays
    for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
        windowSamples[i].x = 0;
        windowSamples[i].y = 0;
        windowSamples[i].z = 0;
        magnitudeSignal[i] = 0.0f;
    }
    for (uint16_t i = 0; i < spectrumBins; i++) {
        spectrum[i] = 0.0f;
    }
}

void SignalProcessor::update(int16_t x, int16_t y, int16_t z,
                             float &rawMagOut,
                             float &processedMagOut,
                             bool &fftReadyOut)
{
    // 1) Add new sample to the circular buffer
    imu_sample_t newSample;
    newSample.x = x;
    newSample.y = y;
    newSample.z = z;
    imuBuffer_addSample(&imuBuffer, &newSample);

    // 2) Compute raw magnitude for this sample
    float fx = (float)x;
    float fy = (float)y;
    float fz = (float)z;
    rawMagOut = sqrtf(fx * fx + fy * fy + fz * fz);

    // 3) Build current window (whatever number of samples we have so far)
    uint16_t windowLen = imuBuffer.count;
    processedMagOut = 0.0f;
    fftReadyOut = false;

    if (windowLen > 0) {
        // Copy window into windowSamples[0..windowLen-1]
        imuBuffer_getWindow(&imuBuffer, windowSamples);

        // Convert to 1D magnitude signal + DC removal + smoothing
        preprocess_windowToMagnitude(windowSamples, magnitudeSignal, windowLen);

        // Latest processed magnitude corresponds to the newest sample
        processedMagOut = magnitudeSignal[windowLen - 1];
    }

    // 4) If we have a full window, compute FFT on the full BUFFER_SIZE
    if (imuBuffer_isWindowReady(&imuBuffer)) {
        // Ensure we use the full window
        imuBuffer_getWindow(&imuBuffer, windowSamples);
        preprocess_windowToMagnitude(windowSamples, magnitudeSignal, BUFFER_SIZE);

        // Compute FFT spectrum of the processed magnitude signal
        fft_computeSpectrum(magnitudeSignal, spectrum, BUFFER_SIZE);

        fftReadyOut = true;
    }
}

void SignalProcessor::getSpectrum(float *outSpectrum, uint16_t &numBins) const {
    numBins = spectrumBins;
    for (uint16_t i = 0; i < spectrumBins; i++) {
        outSpectrum[i] = spectrum[i];
    }
}



/*
#include "SignalProcessor.h"

SignalProcessor::SignalProcessor()
: spectrumBins((FFT_SIZE / 2) + 1)
{
    // Nothing else here
}

void SignalProcessor::begin() {
    imuBuffer_init(&imuBuffer);

    // Clear buffers (optional but safe)
    for (uint16_t i = 0; i < BUFFER_SIZE; i++) {
        magnitudeSignal[i] = 0.0f;
    }
    for (uint16_t i = 0; i < spectrumBins; i++) {
        spectrum[i] = 0.0f;
    }
}

void SignalProcessor::update(int16_t x, int16_t y, int16_t z,
                             float &rawMagOut,
                             float &processedMagOut,
                             bool &fftReadyOut)
{
    // --- 1) Add IMU sample ---
    imu_sample_t newSample;
    newSample.x = x;
    newSample.y = y;
    newSample.z = z;
    imuBuffer_addSample(&imuBuffer, &newSample);

    // --- 2) Compute raw magnitude instantly ---
    float fx = (float)x;
    float fy = (float)y;
    float fz = (float)z;
    rawMagOut = sqrtf(fx * fx + fy * fy + fz * fz);

    // --- 3) Prepare for processed magnitude ---
    uint16_t windowLen = imuBuffer.count;
    processedMagOut = 0.0f;
    fftReadyOut = false;

    if (windowLen > 0) {
        // Directly preprocess the samples stored in imuBuffer
        preprocess_windowToMagnitude(imuBuffer.samples,
                                     magnitudeSignal,
                                     windowLen);

        // Latest processed magnitude = last entry
        processedMagOut = magnitudeSignal[windowLen - 1];
    }

    // --- 4) When window is full, compute FFT ---
    if (imuBuffer_isWindowReady(&imuBuffer)) {

        // Preprocess full window (BUFFER_SIZE samples)
        preprocess_windowToMagnitude(imuBuffer.samples,
                                     magnitudeSignal,
                                     BUFFER_SIZE);

        // Compute FFT on magnitude signal
        fft_computeSpectrum(magnitudeSignal,
                            spectrum,
                            BUFFER_SIZE);

        fftReadyOut = true;
    }
}

void SignalProcessor::getSpectrum(float *outSpectrum, uint16_t &numBins) const {
    numBins = spectrumBins;
    for (uint16_t i = 0; i < spectrumBins; i++) {
        outSpectrum[i] = spectrum[i];
    }
}

*/