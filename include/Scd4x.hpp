#ifndef SCD4X_HPP
#define SCD4X_HPP

#include <SensirionI2cScd4x.h>
#include <Wire.h>

#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

class MySCD4X{
private:
    SensirionI2cScd4x scd4x;
    const int SDA_PIN = 0;
    const int SCL_PIN = 1;

    char errorMessage[64];
    int16_t error;

    void PrintUint64(uint64_t& value) {
        Serial.print("0x");
        Serial.print((uint32_t)(value >> 32), HEX);
        Serial.print((uint32_t)(value & 0xFFFFFFFF), HEX);
    }

public:
    void setup(){
        Wire.begin(SDA_PIN, SCL_PIN);
        scd4x.begin(Wire, SCD41_I2C_ADDR_62);

        uint64_t serialNumber = 0;
        delay(30);
        // Ensure sensor is in clean state
        error = scd4x.wakeUp();
        if (error != NO_ERROR) {
            Serial.print("Error trying to execute wakeUp(): ");
            errorToString(error, errorMessage, sizeof errorMessage);
            Serial.println(errorMessage);
        }
        error = scd4x.stopPeriodicMeasurement();
        if (error != NO_ERROR) {
            Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
            errorToString(error, errorMessage, sizeof errorMessage);
            Serial.println(errorMessage);
        }
        error = scd4x.reinit();
        if (error != NO_ERROR) {
            Serial.print("Error trying to execute reinit(): ");
            errorToString(error, errorMessage, sizeof errorMessage);
            Serial.println(errorMessage);
        }
        // Read out information about the sensor
        error = scd4x.getSerialNumber(serialNumber);
        if (error != NO_ERROR) {
            Serial.print("Error trying to execute getSerialNumber(): ");
            errorToString(error, errorMessage, sizeof errorMessage);
            Serial.println(errorMessage);
            return;
        }
        Serial.print("serial number: ");
        PrintUint64(serialNumber);
        Serial.println();
        //
        // If temperature offset and/or sensor altitude compensation
        // is required, you should call the respective functions here.
        // Check out the header file for the function definitions.
        // Start periodic measurements (5sec interval)
        error = scd4x.startPeriodicMeasurement();
        if (error != NO_ERROR) {
            Serial.print("Error trying to execute startPeriodicMeasurement(): ");
            errorToString(error, errorMessage, sizeof errorMessage);
            Serial.println(errorMessage);
            return;
        }
        //
        // If low-power mode is required, switch to the low power
        // measurement function instead of the standard measurement
        // function above. Check out the header file for the definition.
        // For SCD41, you can also check out the single shot measurement example.
        //
    }

    void readData(uint16_t& co2Concentration, float& temperature, float& relativeHumidity) {
        bool dataReady = false;
        //
        // Slow down the sampling to 0.2Hz.
        //
        delay(5000);
        error = scd4x.getDataReadyStatus(dataReady);
        if (error != NO_ERROR) {
            Serial.print("Error trying to execute getDataReadyStatus(): ");
            errorToString(error, errorMessage, sizeof errorMessage);
            Serial.println(errorMessage);
            return;
        }
        while (!dataReady) {
            delay(100);
            error = scd4x.getDataReadyStatus(dataReady);
            if (error != NO_ERROR) {
                Serial.print("Error trying to execute getDataReadyStatus(): ");
                errorToString(error, errorMessage, sizeof errorMessage);
                Serial.println(errorMessage);
                return;
            }
        }
        //
        // If ambient pressure compenstation during measurement
        // is required, you should call the respective functions here.
        // Check out the header file for the function definition.
        error =
            scd4x.readMeasurement(co2Concentration, temperature, relativeHumidity);
        if (error != NO_ERROR) {
            Serial.print("Error trying to execute readMeasurement(): ");
            errorToString(error, errorMessage, sizeof errorMessage);
            Serial.println(errorMessage);
            return;
        }
    }

};

#endif