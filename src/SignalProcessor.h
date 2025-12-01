#ifndef SIGNAL_PROCESSOR_H
#define SIGNAL_PROCESSOR_H

#include <Arduino.h>
#include "IMU_I2C.h"
#include "Buffer.h"
#include "Preprocess.h"
#include "FFTModule.h"

// This helper owns the buffer, preprocessing, and FFT.
// You call update(...) once per new IMU sample.
// It gives you:
//   - raw magnitude for that sample
//   - processed magnitude for that sample
//   - a flag telling you when a NEW FFT spectrum is ready
class SignalProcessor {
public:
    SignalProcessor();

    // Call once in setup()
    void begin();

    // Call this every time you read a new IMU sample.
    //
    // Inputs:  x, y, z  = raw accelerometer readings
    // Outputs:
    //   rawMagOut        = sqrt(x^2 + y^2 + z^2)
    //   processedMagOut  = last value of preprocessed 1D signal
    //   fftReadyOut      = true only when a NEW full-window FFT was computed
    void update(int16_t x, int16_t y, int16_t z,
                float &rawMagOut,
                float &processedMagOut,
                bool &fftReadyOut);

    // When fftReadyOut was true on the last update(),
    // you can call this to fetch the spectrum.
    //
    // outSpectrum must have space for at least (FFT_SIZE/2 + 1) floats.
    // numBins will be set to (FFT_SIZE/2 + 1).
    void getSpectrum(float *outSpectrum, uint16_t &numBins) const;

private:
    imu_buffer_t imuBuffer;                    // circular buffer of IMU samples
    imu_sample_t windowSamples[BUFFER_SIZE];   // current window of samples
    float        magnitudeSignal[BUFFER_SIZE]; // preprocessed magnitudes
    float        spectrum[(FFT_SIZE / 2) + 1]; // last FFT spectrum
    uint16_t     spectrumBins;                 // usually (FFT_SIZE/2 + 1)
};

#endif // SIGNAL_PROCESSOR_H



/*
#ifndef SIGNALPROCESSOR_H
#define SIGNALPROCESSOR_H

#include <Arduino.h>
#include "IMU_I2C.h"
#include "Buffer.h"
#include "Preprocess.h"
#include "FFTModule.h"

class SignalProcessor {
public:
    SignalProcessor();

    void begin();

    void update(int16_t x, int16_t y, int16_t z,
                float &rawMagOut,
                float &processedMagOut,
                bool &fftReadyOut);

    void getSpectrum(float *outSpectrum, uint16_t &numBins) const;

private:
    imu_buffer_t imuBuffer;

    // magnitudeSignal is needed for FFT — keep it, but RAM friendly
    float magnitudeSignal[BUFFER_SIZE];

    // FFT output size = (FFT_SIZE/2 + 1)
    const uint16_t spectrumBins;
    float spectrum[(FFT_SIZE / 2) + 1];
};

#endif

*/