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

###  System Now Processes:

IMU → Buffer → Magnitude → Mean Removal → Smoothing → FFT Spectrum

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
