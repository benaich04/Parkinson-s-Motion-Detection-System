#ifndef UIDATAMODEL_H
#define UIDATAMODEL_H

#include <stdint.h>   

//this is the data model that holds all the UI relevant data
//it is read and written by various parts of the program


struct UIDataModel {

    uint8_t hour = 12;
    uint8_t minute = 0;
    uint8_t day = 1;
    uint8_t month = 1;
    uint16_t year = 2025;
    uint8_t batteryPercent = 100;

    bool recording = false;
    uint16_t samplingRateHz = 52;

    float ax = 0;
    float ay = 0;
    float az = 0;
    float magnitude = 0;

    float tremorPower = 0;
    float dyskinesiaPower = 0;

    enum MovementState { STABLE, TREMOR, DYSKINESIA } state = STABLE;
    float intensity = 0;

    uint16_t tremorEventsToday = 0;
    float avgIntensityToday = 0;

};

#endif
