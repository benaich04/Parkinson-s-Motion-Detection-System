#ifndef MOTIONCLASSIFIER_H
#define MOTIONCLASSIFIER_H

#include <Arduino.h>
#include "FFTModule.h"

// Assuming 1 Hz per bin for planning purposes
#define ACCUMULATION_TIME_MS 3000  // 3 seconds
#define NUM_SPECTRUM_BINS 33       // Adjust based on your actual FFT_SIZE

// Motion states
enum MotionState {
    NORMAL,
    TREMOR,      // 3-5 Hz
    DYSKINESIA   // 5-7 Hz
};

class MotionClassifier {
public:
    MotionClassifier();
    
    void begin();
    
    // Call this each time you have a new FFT spectrum ready
    // Returns true when classification is complete (after 3 seconds)
    bool addSpectrum(const float* outSpectrum, uint16_t numBins);
    
    // Get the classified motion state
    MotionState getClassification() const;
    
    // Get accumulated spectrum (for debugging/visualization)
    void getAccumulatedSpectrum(float* outAccumulated, uint16_t& numBins) const;
    
    // Reset accumulator to start a new 3-second window
    void reset();

private:
    float accumulatedSpectrum[NUM_SPECTRUM_BINS];  // Summed spectrum values
    uint16_t spectrumCount;                        // How many spectrums added
    unsigned long startTime;                       // When accumulation started
    bool isAccumulating;                          // Whether we're currently accumulating
    MotionState currentState;                     // Last classification result
    
    // Internal classification logic
    MotionState classify();
};

#endif