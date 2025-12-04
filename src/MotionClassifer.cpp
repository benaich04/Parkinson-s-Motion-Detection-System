#include "MotionClassifier.h"

MotionClassifier::MotionClassifier()
: spectrumCount(0),
  startTime(0),
  isAccumulating(false),
  currentState(NORMAL)
{
    // Initialize accumulated spectrum to zero
    for (uint16_t i = 0; i < NUM_SPECTRUM_BINS; i++) {
        accumulatedSpectrum[i] = 0.0f;
    }
}

void MotionClassifier::begin() {
    reset();
}

bool MotionClassifier::accumulateSpectrum(const float* outSpectrum, uint16_t numBins) {
    
    // Start accumulation on first spectrum
    if (!isAccumulating) {
        isAccumulating = true;
        startTime = millis();
    }
    
    // Add spectrum values to accumulator
    uint16_t binsToProcess = (numBins < NUM_SPECTRUM_BINS) ? numBins : NUM_SPECTRUM_BINS;
    for (uint16_t i = 0; i < binsToProcess; i++) {
        accumulatedSpectrum[i] += outSpectrum[i];
    }
    spectrumCount++;
    
    // Check if 3 seconds have elapsed
    unsigned long elapsed = millis() - startTime;
    if (elapsed >= ACCUMULATION_TIME_MS) {
        // Perform classification
        currentState = classify();
        
        // Reset for next window
        reset();
        
        return true;  // Classification complete!
    }
    
    return false;  // Still accumulating
}

MotionState MotionClassifier::classify() {
    
    // Define frequency ranges (assuming 1 Hz per bin)
    const uint16_t TREMOR_START = 3;   // 3 Hz
    const uint16_t TREMOR_END = 5;     // 5 Hz
    const uint16_t DYSK_START = 5;     // 5 Hz  
    const uint16_t DYSK_END = 7;       // 7 Hz
    
    // Sum magnitudes in each range
    float tremorEnergy = 0.0f;
    float dyskinesiaEnergy = 0.0f;
    float totalEnergy = 0.0f;
    
    // Calculate energy in tremor range (3-5 Hz)
    for (uint16_t i = TREMOR_START; i <= TREMOR_END && i < NUM_SPECTRUM_BINS; i++) {
        tremorEnergy += accumulatedSpectrum[i];
    }
    
    // Calculate energy in dyskinesia range (5-7 Hz)
    for (uint16_t i = DYSK_START; i <= DYSK_END && i < NUM_SPECTRUM_BINS; i++) {
        dyskinesiaEnergy += accumulatedSpectrum[i];
    }
    
    // Calculate total energy (excluding DC component at bin 0)
    for (uint16_t i = 1; i < NUM_SPECTRUM_BINS; i++) {
        totalEnergy += accumulatedSpectrum[i];
    }
    
    // Avoid division by zero
    if (totalEnergy < 0.001f) {
        return NORMAL;
    }
    
    // Calculate percentage of energy in each band
    float tremorPercent = (tremorEnergy / totalEnergy) * 100.0f;
    float dyskinesiaPercent = (dyskinesiaEnergy / totalEnergy) * 100.0f;
    
    // Classification thresholds (adjust these based on testing!)
    const float THRESHOLD = 30.0f;  // 30% of energy in band
    
    // Prioritize dyskinesia if both are present
    if (dyskinesiaPercent > THRESHOLD) {
        return DYSKINESIA;
    }
    else if (tremorPercent > THRESHOLD) {
        return TREMOR;
    }
    else {
        return NORMAL;
    }
}

void MotionClassifier::reset() {
    // Clear accumulated spectrum
    for (uint16_t i = 0; i < NUM_SPECTRUM_BINS; i++) {
        accumulatedSpectrum[i] = 0.0f;
    }
    spectrumCount = 0;
    isAccumulating = false;
}

MotionState MotionClassifier::getClassification() const {
    return currentState;
}

void MotionClassifier::getAccumulatedSpectrum(float* outAccumulated, uint16_t& numBins) const {
    numBins = NUM_SPECTRUM_BINS;
    for (uint16_t i = 0; i < NUM_SPECTRUM_BINS; i++) {
        outAccumulated[i] = accumulatedSpectrum[i];
    }
}
//Function then needs to be implemeted in SignalUIBridge.ccp using outspectrum as an input once the buffer is full
//Should add about 80bytes or 0.08kb to the memeory, hopefully that is small enough if not there are some things that can be done
//Classify function I think should be good, same with most of them, may need to test accumulateSpectrum function
//Not sure if getAccumulatedSpectured function is necessary but keeping it for now