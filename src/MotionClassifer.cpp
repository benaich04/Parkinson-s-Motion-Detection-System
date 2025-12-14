#include "MotionClassifier.h"
#include <Arduino.h>

// --------------------------------------------------
// Constructor
// --------------------------------------------------
MotionClassifier::MotionClassifier()
: spectrumCount(0),
  startTime(0),
  isAccumulating(false),
  currentState(NORMAL),
  lastE_tremor(0.0f),
  lastE_dysk(0.0f),
  lastE_total(0.0f),
  lastTremorRatio(0.0f),
  lastDyskRatio(0.0f)
{
    reset();
}

// --------------------------------------------------
// Public API
// --------------------------------------------------
void MotionClassifier::begin() {
    reset();
}

bool MotionClassifier::accumulateSpectrum(const float* spectrum,
                                          uint16_t numBins)
{
    // Start accumulation window
    if (!isAccumulating) {
        isAccumulating = true;
        startTime = millis();
    }

    // Safe bound on FFT bins
    uint16_t n = (numBins < NUM_SPECTRUM_BINS)
                   ? numBins
                   : NUM_SPECTRUM_BINS;

    // Accumulate ENERGY per bin: |X(f)|^2
    for (uint16_t i = 0; i < n; i++) {
        float mag = spectrum[i];
        accumulatedSpectrum[i] += mag * mag;
    }

    spectrumCount++;

    // End of accumulation window
    if (millis() - startTime >= ACCUMULATION_TIME_MS) {
        currentState = classify();
        reset();
        return true;
    }

    return false;
}

MotionState MotionClassifier::getClassification() const {
    return currentState;
}

// --------------------------------------------------
// Debug / tuning accessors
// --------------------------------------------------
void MotionClassifier::getEnergies(float &E_tremor,
                                   float &E_dysk,
                                   float &E_total) const
{
    E_tremor = lastE_tremor;
    E_dysk   = lastE_dysk;
    E_total  = lastE_total;
}

void MotionClassifier::getEnergyRatios(float &tremorRatio,
                                       float &dyskRatio) const
{
    tremorRatio = lastTremorRatio;
    dyskRatio   = lastDyskRatio;
}

// --------------------------------------------------
// Internal classification logic
// --------------------------------------------------
MotionState MotionClassifier::classify() {

    // --------------------------------------------------
    // Band definitions (BIN INDICES, not Hz)
    // Assumes Fs / FFT_SIZE ≈ 1 Hz
    // --------------------------------------------------
    const uint16_t TREMOR_START = 3;   // 3 Hz
    const uint16_t TREMOR_END   = 5;   // [3,5)
    const uint16_t DYSK_START   = 5;   // 5 Hz
    const uint16_t DYSK_END     = 7;   // [5,7)

    float E_tremor = 0.0f;
    float E_dysk   = 0.0f;
    float E_total  = 0.0f;

    // Skip DC bin (0 Hz)
    for (uint16_t i = 1; i < NUM_SPECTRUM_BINS; i++) {

        float e = accumulatedSpectrum[i];
        E_total += e;

        if (i >= TREMOR_START && i < TREMOR_END)
            E_tremor += e;
        else if (i >= DYSK_START && i < DYSK_END)
            E_dysk += e;
    }

    // --------------------------------------------------
    // Store energies for printing / tuning
    // --------------------------------------------------
    lastE_tremor = E_tremor;
    lastE_dysk   = E_dysk;
    lastE_total  = E_total;

    // Noise / idle guard
    if (E_total < 1e-6f) {
        lastTremorRatio = 0.0f;
        lastDyskRatio   = 0.0f;
        return NORMAL;
    }

    // Ratios (logging + bias logic)
    lastTremorRatio = E_tremor / E_total;
    lastDyskRatio   = E_dysk   / E_total;

    // --------------------------------------------------
    // Absolute energy thresholds (YOU TUNE THESE)
    // --------------------------------------------------
    const float TREMOR_ENERGY_THRESH = 80000.0f;
    const float DYSK_ENERGY_THRESH   = 70000.0f;

    bool tremorDetected = (E_tremor > TREMOR_ENERGY_THRESH);
    bool dyskDetected   = (E_dysk   > DYSK_ENERGY_THRESH);

    // --------------------------------------------------
    // Primary decision logic
    // --------------------------------------------------
    if (!tremorDetected && !dyskDetected)
        return NORMAL;

    if (tremorDetected && !dyskDetected)
        return TREMOR;

    if (!tremorDetected && dyskDetected)
        return DYSKINESIA;

    // --------------------------------------------------
    // BOTH exceeded thresholds → biased ratio decision
    // Dyskinesia wins unless tremor is clearly stronger
    // --------------------------------------------------
    float ratio = E_tremor / (E_dysk + 1e-6f);

    //!!!!!!!!!!
    const float DYSK_BIAS_RATIO = 2.0f;   // 1.2–1.5 typical

    if (ratio < DYSK_BIAS_RATIO)
        return DYSKINESIA;
    else
        return TREMOR;
}

// --------------------------------------------------
// Reset internal state
// --------------------------------------------------
void MotionClassifier::reset() {

    for (uint16_t i = 0; i < NUM_SPECTRUM_BINS; i++)
        accumulatedSpectrum[i] = 0.0f;

    spectrumCount  = 0;
    isAccumulating = false;
}
