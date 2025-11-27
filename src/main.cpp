#include <SPI.h>
#include "IMU.h"
#include <arduinoFFT.h>


void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Initializing IMU...");

    uint8_t status = IMU_init();

    if (status != IMU_OK) {
        Serial.print("IMU NOT FOUND! Error code: ");
        Serial.println(status);
        while (1); // stop here
    }

    Serial.println("IMU initialized successfully.\n");
}

void loop() {
    int16_t ax, ay, az;

    IMU_readAccel(&ax, &ay, &az);

    Serial.print("AX: ");
    Serial.print(ax);
    Serial.print("   AY: ");
    Serial.print(ay);
    Serial.print("   AZ: ");
    Serial.println(az);

    delay(100); // 10 Hz print rate (sampling is still 100 Hz internally)
}
