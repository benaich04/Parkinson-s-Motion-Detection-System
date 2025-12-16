#ifndef MOTIONCLASSIFIER_H
#define MOTIONCLASSIFIER_H

#include <Arduino.h>
#include "FFTModule.h"

// Length of analysis window (ms)
// NOTE: Affects absolute energy magnitude (thresholds must scale accordingly)
#define ACCUMULATION_TIME_MS   5000

// Number of FFT bins used by the classifier
// MUST match the FFT output size: (FFT_SIZE / 2) + 1
// FFT_SIZE must be defined elsewhere (e.g., SignalProcessor / FFT module)
#define NUM_SPECTRUM_BINS      ((FFT_SIZE / 2) + 1)

// Motion States
enum MotionState {
    NORMAL,        // No pathological motion detected
    TREMOR,        // Tremor energy (3–5 Hz) dominant
    DYSKINESIA     // Dyskinesia energy (5–7 Hz) dominant
};

// --------------------------------------------------
// Motion Classifier
// --------------------------------------------------
class MotionClassifier {
public:
    MotionClassifier();

    // Initialize / reset internal state
    void begin();

    // Feed one FFT magnitude spectrum into the classifier
    // Returns true when the accumulation window completes
    bool accumulateSpectrum(const float* spectrum,
                            uint16_t numBins);

    // Retrieve last computed motion state
    MotionState getClassification() const;

    // --------------------------------------------------
    // Debug / tuning accessors (YOU REQUESTED THESE)
    // --------------------------------------------------

    // Absolute band energies + total energy
    void getEnergies(float &E_tremor,
                     float &E_dysk,
                     float &E_total) const;

    // Energy ratios (for insight / logging only)
    void getEnergyRatios(float &tremorRatio,
                         float &dyskRatio) const;

    // Reset classifier and start a new accumulation window
    void reset();

private:
    // --------------------------------------------------
    // Internal state
    // --------------------------------------------------

    // Accumulated ENERGY per FFT bin: sum(|X(f)|^2)
    float accumulatedSpectrum[NUM_SPECTRUM_BINS];

    // Number of FFT frames accumulated
    uint16_t spectrumCount;

    // Timestamp when accumulation window started
    unsigned long startTime;

    // Indicates whether accumulation is active
    bool isAccumulating;

    // Last computed classification
    MotionState currentState;

    // --------------------------------------------------
    // Stored outputs for logging / tuning
    // --------------------------------------------------
    float lastE_tremor;
    float lastE_dysk;
    float lastE_total;

    float lastTremorRatio;
    float lastDyskRatio;

    // Internal classification logic
    MotionState classify();
};

#endif // MOTIONCLASSIFIER_H
