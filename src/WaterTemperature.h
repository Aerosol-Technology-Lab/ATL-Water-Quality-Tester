#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <OneWire.h>
#include <DallasTemperature.h>

/**
 * @brief OneWire Temperature Sensor wrapper
 * 
 */
class WaterTemperature
{
public:
    const char *DEFAULT_ID = "_";

private:

    OneWire oneWire;
    DallasTemperature sensor;
    bool initialized = false;

public:
    /**
     * @brief Unsafe construction of WaterTemperature object
     * 
     */
    WaterTemperature();
    /**
     * @brief Construct a new Water Temperature object
     * 
     * @param pin digital pin connected to data bus of temperature sensor
     * @param initialize should constructor initialize the object
     */
    WaterTemperature(uint8_t pin, bool initialize);

    void init();
    
    float read(uint8_t idx=0);

    size_t write(char *buffer, uint8_t idx);
};
