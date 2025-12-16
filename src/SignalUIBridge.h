#ifndef SIGNAL_UI_BRIDGE_H
#define SIGNAL_UI_BRIDGE_H

#include <Arduino.h>
#include "UIDataModel.h"      
#include "MotionClassifier.h" 

// Ensure FFT_SIZE matches your SignalProcessor settings (usually 128)
#ifndef FFT_SIZE
#define FFT_SIZE 128 
#endif

class SignalUIBridge {
public:
    // Initialization now requires the Data Model pointer
    static void begin(UIDataModel* model);
    
    // Main update loop
    static void update();

    // ===== GETTERS (For Graphs) =====
    static float  getProcessedMagnitude();
    static bool   hasNewFFT();
    static float  getFFTPeak();
    
    // Get full FFT spectrum data
    static bool   getFFTSpectrum(float *outSpectrum,
                                 uint16_t maxBins,
                                 uint16_t &numBins);

    // ===== GETTERS (For Patient Screen) =====
    static MotionState getMotionState(); 

private:
    // Signal Data
    static float latestMag;
    static bool  fftReady;
    static float fftPeak;
    
    // FFT Data Storage
    static float latestSpectrum[(FFT_SIZE/2)+1];
    static uint16_t latestSpectrumBins;

    // Internal link to the Data Model (The "Memory")
    static UIDataModel* uiData;
    
    // Track previous state to detect edges (Normal -> Tremor)
    static MotionState currentMotionState;
};

#endif