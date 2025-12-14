#include "SignalUIBridge.h"
#include "IMU_I2C.h"
#include "SignalProcessor.h"
#include "MotionClassifier.h"

// --------------------------------------------------
// Static objects
// --------------------------------------------------
static SignalProcessor  signalProc;
static MotionClassifier motionClassifier;

// --------------------------------------------------
// Static UI state
// --------------------------------------------------
float SignalUIBridge::latestMag = 0.0f;
bool  SignalUIBridge::fftReady  = false;
float SignalUIBridge::fftPeak   = 0.0f;

// Link to the Data Model and track state locally for edge detection
UIDataModel* SignalUIBridge::uiData = nullptr;
MotionState  SignalUIBridge::currentMotionState = NORMAL;

float    SignalUIBridge::latestSpectrum[(FFT_SIZE / 2) + 1];
uint16_t SignalUIBridge::latestSpectrumBins = 0;

// --------------------------------------------------
// Timing
// --------------------------------------------------
static unsigned long lastSample = 0;
static const unsigned long SAMPLE_MS = 50;

// --------------------------------------------------
// Initialization
// --------------------------------------------------
void SignalUIBridge::begin(UIDataModel* model) {
    // 1. Store the pointer to the UI Data Model
    uiData = model;

    // 2. Initialize hardware and logic
    IMU_I2C_init();
    signalProc.begin();
    motionClassifier.begin();
}

// --------------------------------------------------
// Main update loop
// --------------------------------------------------
void SignalUIBridge::update() {

    unsigned long now = millis();
    if (now - lastSample < SAMPLE_MS)
        return;

    lastSample = now;

    // --------------------------------------------------
    // 1. Read IMU
    // --------------------------------------------------
    int16_t x, y, z;
    IMU_I2C_readAccel(&x, &y, &z);

    // --------------------------------------------------
    // 2. Signal processing
    // --------------------------------------------------
    float raw, processed;
    bool  fftReadyLocal = false;

    signalProc.update(x, y, z, raw, processed, fftReadyLocal);
    latestMag = processed;

    // --- SYNC MAGNITUDE TO DATA MODEL ---
    if (uiData != nullptr) {
        uiData->magnitude = processed;
        
        // Update average intensity (Exponential Moving Average)
        // 95% history, 5% new value - smooths out the number for the UI
        uiData->avgIntensityToday = (uiData->avgIntensityToday * 0.95f) + (processed * 0.05f);
    }

    if (!fftReadyLocal)
        return;

    // --------------------------------------------------
    // 3. Get FFT spectrum
    // --------------------------------------------------
    float spectrum[(FFT_SIZE / 2) + 1];
    uint16_t bins = 0;

    signalProc.getSpectrum(spectrum, bins);

    // Store spectrum for UI Visualization (Diagnostics Screen)
    latestSpectrumBins = bins;
    for (uint16_t i = 0; i < bins; i++)
        latestSpectrum[i] = spectrum[i];

    // Compute FFT peak
    float peak = 0.0f;
    for (uint16_t i = 0; i < bins; i++) {
        if (spectrum[i] > peak)
            peak = spectrum[i];
    }

    fftPeak  = peak;
    fftReady = true;

    // --------------------------------------------------
    // 4. Motion classification (energy accumulation)
    // --------------------------------------------------
    bool classificationDone =
        motionClassifier.accumulateSpectrum(spectrum, bins);

    // --------------------------------------------------
    // 5. Handle Classification Results & Update UI Model
    // --------------------------------------------------
    if (classificationDone) {

        MotionState newState = motionClassifier.getClassification();

        // --- UPDATE HISTORY COUNTS ---
        // if (uiData != nullptr) {
        //     // Detect transition from Normal -> Tremor to count "Events"
        //     if (currentMotionState == NORMAL && newState == TREMOR) {
        //         uiData->tremorEventsToday++;
        //     }
        // }

        // --- SYNC STATE TO DATA MODEL ---
        if (uiData != nullptr) {
            switch (newState) {
                case NORMAL:     
                    uiData->state = UIDataModel::STABLE;
                    uiData->intensity = 0.0f;
                    break;
                case TREMOR:     
                    uiData->state = UIDataModel::TREMOR;
                    uiData->intensity = latestMag;
                    break;
                case DYSKINESIA: 
                    uiData->state = UIDataModel::DYSKINESIA;
                    uiData->intensity = latestMag;
                    break;
            }
        }

        // Update local state
        currentMotionState = newState;

        // --- SYNC STATE TO DATA MODEL ---
        if (uiData != nullptr) {
            switch (newState) {
                case NORMAL:     uiData->state = UIDataModel::STABLE; break;
                case TREMOR:     uiData->state = UIDataModel::TREMOR; break;
                case DYSKINESIA: uiData->state = UIDataModel::DYSKINESIA; break;
            }
        }
    }
}


// --------------------------------------------------
// UI accessors
// --------------------------------------------------
float SignalUIBridge::getProcessedMagnitude() {
    return latestMag;
}

MotionState SignalUIBridge::getMotionState() {
    return currentMotionState;
}

bool SignalUIBridge::hasNewFFT() {
    if (!fftReady)
        return false;

    fftReady = false;
    return true;
}

float SignalUIBridge::getFFTPeak() {
    return fftPeak;
}

// --------------------------------------------------
// Return last FFT spectrum (for plots / debug)
// --------------------------------------------------
bool SignalUIBridge::getFFTSpectrum(float *outSpectrum,
                                    uint16_t maxBins,
                                    uint16_t &numBins)
{
    if (latestSpectrumBins == 0) {
        numBins = 0;
        return false;
    }

    uint16_t n = (latestSpectrumBins < maxBins)
                   ? latestSpectrumBins
                   : maxBins;

    for (uint16_t i = 0; i < n; i++)
        outSpectrum[i] = latestSpectrum[i];

    numBins = n;
    return true;
}