#include "EC.h"
#include "WaterTemperature.h"
#include "utils.h"
#include <Arduino.h>

EC::EC(uint8_t pin, WaterTemperature *waterTemperature):
    pin(pin),
    waterTemperature(waterTemperature)
{
    
}

void EC::init()
{
    // nothing to do
}

void EC::setWaterTemperatureSensor(WaterTemperature *sensor)
{
    this->waterTemperature = sensor;
}

float EC::read(uint8_t _)
{
    // algorithm based on DFRobot EC library
    static float kValue = 1.0f;
    
    for (int i = 0; i < 5; ++i) analogRead(pin);
    float voltage = rawRead();
    float temperature = waterTemperature ? waterTemperature->read() : 25.0f;

    float rawEC = 1000.0f * voltage / RES2 / ECREF;
    float valTmp = rawEC * kValue;

    if (valTmp > 2.5f)      kValue = kValueHigh;
    else if (valTmp < 2.0f) kValue = kValueLow;

    return (rawEC * kValue) / (1.0f + 0.0185f * (temperature - 25.0f));
}

size_t EC::write(char *buffer, uint8_t idx)
{

}

bool EC::calibrate()
{
    for (int i = 0; i < 5; ++i) analogRead(pin);
    float voltage = rawRead();
    float temperature = waterTemperature ? waterTemperature->read() : 25.0f;
    float rawEC = 1000.0f * voltage / RES2 / ECREF;

    float compECSolution;
    if (rawEC > 0.9f && rawEC < 1.9f) {                         
        compECSolution = 1.413f * (1.0f + 0.0185f * (temperature - 25.0f));  
    }
    else if (rawEC > 9 && rawEC < 16.8f) {                
        compECSolution = 12.88f * (1.0 + 0.0185f * (temperature - 25.0f));
    }
    else{
        return false;
    }

    float tempKValue = RES2 * ECREF * compECSolution / 1000.0f / voltage;
    if (tempKValue > 0.5f && tempKValue < 1.5f) {

        if (rawEC > 0.9f && rawEC < 1.9f)       kValueLow  = tempKValue;
        else if (rawEC > 9.0f && rawEC < 16.8f) kValueHigh = tempKValue;
    }
    else {
        return false;
    }

    return true;
}

void EC::getCalibration(float &lowValue, float &highValue)
{
    lowValue  = kValueLow;
    highValue = kValueHigh;
}

void EC::setCalibration(float lowValue, float highValue)
{
    kValueLow  = lowValue;
    kValueHigh = highValue;
}
