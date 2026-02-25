#  **Parkinson’s Movement Detection – Backend Firmware (ATmega32U4)**




This project implements the **backend processing pipeline** for a wearable device that detects **tremor**, **dyskinesia**, and later **gait abnormalities** using an accelerometer on an ATmega32U4 microcontroller.

The backend is modular and follows the exact structure planned for the full system:
IMU → Buffer → Preprocessing → FFT → Feature Extraction → Classification → UI.

This repository contains everything up to the **FFT module**, fully implemented and working.

---

# **Current Progress**

###  1. IMU Hardware Interface

* SPI setup with manual register configuration
* ADXL345 accelerometer initialization
* Functions to read X/Y/Z acceleration
* Combined read helper (optional)

###  2. Circular Buffer System

* Stores incoming IMU samples in real time
* Maintains a rolling window (156 samples)
* Handles overwrite logic
* Provides a ready-to-process block of samples
* Allows the pipeline to always work on a full window

###  3. Preprocessing Module

Converts raw X/Y/Z samples into a clean 1D signal:

* Compute magnitude: `sqrt(x² + y² + z²)`
* Remove DC offset (gravity + bias)
* Apply moving average smoothing filter
* Output is ready for frequency analysis

###  4. FFT Module

Runs a **128-point FFT** on the preprocessed magnitude signal:

* Uses arduinoFFT library
* Applies a Hamming window
* Computes frequency magnitudes
* Outputs the spectrum from **0–26 Hz** (0–64 bins)
* This will be used to detect tremor (3–5 Hz) and dyskinesia (5–7 Hz)

### **5. Real-Time TFT UI with Touch Navigation**

* TFT screen (ILI9341) integrated with full rendering pipeline
* Touch gesture engine implemented using ΔX/ΔY analysis
* **Swipe Up → Diagnostics Screen**
* Future swipes (Down/Left/Right) reserved for next UI pages

---

### **6. Diagnostics Screen (Technical Mode)**

The swipe-up diagnostics page now displays **two real-time plots**:

#### **a. Processed Magnitude (Time-Domain Plot)**

* Updated in real time using `plotNextSample()`
* Shows ~3 seconds of smoothed magnitude data
* Fixed Y-axis (0 → 200 units) for stability
* Grid + border drawn once, samples streamed efficiently

#### **b. FFT Spectrum (Frequency-Domain Plot)**

* New `drawSpectrumBars()` implementation
* **1 bar per 1 Hz** for high-resolution visualization
* Frequency range capped to **0–10 Hz** (optimal for Parkinson’s tremor bands)
* Uses fixed amplitude scaling (no auto-scaling, no clipping)
* Updates only when the FFT module signals `fftReady`

This creates a doctor-grade analytical page for observing tremor frequency peaks in real time.

---

### **7. CSV + Python Verification of Preprocessing**

To validate correctness of the microcontroller’s preprocessing pipeline:

* Raw XYZ and MCU-processed magnitudes exported as CSV
* A Python script recomputes

  * magnitude
  * DC-offset removal
  * moving-average smoothing (N=4)
* Python outputs compared to MCU output
* Plots confirm **bit-accurate preprocessing**

This ensures the firmware’s data pipeline is mathematically correct.

---

### **8. RAM Optimization & Memory Issue Fixes**

The ATmega32U4 has only **2.5 KB of RAM**, causing issues with:

* `BUFFER_SIZE` (defined by `SAMPLE_RATE_HZ × WINDOW_SECONDS`)
* `FFT_SIZE` array allocation

Fixes applied:

* Preprocessed data is now written **directly into FFT input buffer**
* Removed temporary intermediate buffers
* Reduced large local arrays inside functions
* Ensured FFT arrays stay static and minimal
* Confirmed stable RAM usage during UI rendering + FFT execution


###  System Now Processes:
IMU → Buffer → Magnitude → Mean Removal → Smoothing → FFT Spectrum → UIManager → Real-Time TFT UI (Processed Magnitude Plot + 0–10 Hz FFT Bar Spectrum + Touch Swipe Navigation)
---

#  **File Overview**

### `src/IMU.h` / `src/IMU.cpp`

* Handles SPI communication with the ADXL345
* Initializes the sensor
* Reads acceleration values
* Provides data in the `imu_sample_t` structure

### `src/Buffer.h` / `src/Buffer.cpp`

* Implements a circular buffer for IMU samples
* Stores the last 156 samples
* Provides:

  * `imuBuffer_addSample()`
  * `imuBuffer_isWindowReady()`
  * `imuBuffer_getWindow()`

### `src/Preprocess.h` / `src/Preprocess.cpp`

* Converts XYZ samples → 1D clean signal
* Removes DC offset
* Applies moving average smoothing
* Output prepares the data for FFT

### `src/FFTModule.h` / `src/FFTModule.cpp`

* Performs a 128-point FFT
* Applies a Hamming window
* Computes magnitude spectrum
* Outputs 65 usable bins (0–26 Hz)

### `src/main.cpp`

* Demonstrates pipeline usage
* Reads IMU → pushes into buffer → preprocess window → FFT
* Will later be replaced with full system integration & classification

### PlatformIO structure (`platformio.ini`, include/, lib/, test/`)

Standard PlatformIO project layout.

---
30 November: 

### **🆕 Added System Functionality**

We have now fully implemented the **FFT + UI integration** with a **touch-enabled navigation system**:

#### **1. Touchscreen Gesture Navigation**

The TFT detects:

* **Swipe Up → Diagnostics page**
* **Swipe Down / Left / Right → reserved for future screens**

The gesture engine analyzes ΔX/ΔY to determine the direction reliably.

#### **2. Diagnostics Page (Swipe-Up Screen)**

This page now displays **two real-time plots**:

##### **(a) Processed Magnitude Plot (Time Domain)**

* Streams in real-time using `plotNextSample()`
* Shows the filtered magnitude over the last ~100 seconds
* Fixed Y-axis (e.g., **0 → 200 units**) for consistent scaling
* Grid + borders handled by `TFTHelpers`

##### **(b) FFT Spectrum Plot (Frequency Domain)**

* Uses the new `drawSpectrumBars()`
* 1 bar per **1 Hz** for high resolution
* Frequency range restricted to **0–10 Hz** for Parkinson’s detection
* Uses a fixed amplitude scaling factor (no auto-scaling)
* Updates only when a new FFT is available

This creates a **technical “doctor mode”** page showing movement intensity and spectral peaks in real time.

---

### ** CSV + Python Preprocessing Validation**

To verify correctness of the preprocessing stage:

* We recorded raw XYZ data + MCU-processed magnitude into CSV
* A Python script re-computes:

  * magnitude
  * DC offset removal
  * moving average smoothing
* It then plots:

  * raw magnitudes
  * MCU processed
  * Python processed
 
  <img width="3000" height="2400" alt="imu_mag_comparison" src="https://github.com/user-attachments/assets/ab90d4fa-c895-4106-9131-7909c5af010e" />

**The Python outputs match the MCU**, confirming correct preprocessing.



---

### **🐛 RAM Issues + Fixes**

We encountered RAM pressure due to the ATmega32U4’s **2.5 KB RAM limit**.
The main RAM consumers were:

1. **`BUFFER_SIZE`** (from `SAMPLE_RATE_HZ × WINDOW_SECONDS` in `Buffer.h`)
2. **`FFT_SIZE`** (in `FFTModule.h`)

Fixes applied:

* Removed intermediate “buckets”
* Wrote preprocessed data **directly** into FFT input buffer
* Reduced unnecessary local arrays
* Optimized FFT scratch buffers
* Ensured all arrays are static and sized minimally

System now runs FFT + UI in real time without memory overflow.


---

#  **Next Steps (In Order)**

### Tremor Feature Extraction (3–5 Hz)

* Scan FFT bins corresponding to:
  `3.25–4.87 Hz` → bins `8–12`
* Compute intensity based on magnitude

### Dyskinesia Feature Extraction (5–7 Hz)

* Scan FFT bins:
  `5.68–6.90 Hz` → bins `14–17`
* Extract intensity and stability

### State Classification Module

Combine features to output:

* **NO_ISSUE**
* **TREMOR**
* **DYSKINESIA**
* (Later: **FREEZING_OF_GAIT**)

With a stable “non-flicker” decision.

### Integration Layer

Combine all modules into a unified firmware loop.

### Interface with TFT Screen (future)

Expose:

* Current state
* Intensity level
* Optional spectrum visualization
* Battery / status indicators

---

# 🔍 **How to Read This Project**

Start in this order:

1. **IMU.cpp** → understand how data enters the system
2. **Buffer.cpp** → see how windows are collected
3. **Preprocess.cpp** → see how the signal is cleaned
4. **FFTModule.cpp** → see how the spectrum is produced
5. **main.cpp** → see the pipeline logic

This gives a complete understanding of the backend so far.

---

#  Contribution

This is an ongoing project. Future contributions include:

* Adding detection logic
* Optimizing performance
* Integrating gait detection
* Expanding preprocessing filters
* Building the TFT UI frontend

---
