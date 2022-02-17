#include "PH.h"

#include "Arduino.h"
#include "utils.h"

PH::PH(uint8_t pin)
    : pin(pin)
{ }

void PH::init()
{
    // nothing to do
}

float PH::read(uint8_t _)
{
    // based on DFRobot PH Driver
    float slope = (7.0f - 4.0f )/ ((this->neutralVoltage-1500.0f) / 3.0f - (this->acidVoltage-1500.0f) / 3.0f);  // two point: (_neutralVoltage,7.0),(_acidVoltage,4.0)
    float intercept =  7.0f - slope * (this->neutralVoltage-1500.0f) / 3.0f;

    for (int i = 0; i < 5; ++i) rawRead();  // get accurate reading
    float voltage = rawRead();
    return slope * (voltage - 1500.0f) / 3.0f + intercept;
}

bool PH::calibrate()
{
    for (int i = 0; i < 5; ++i) rawRead();  // get accurate reading
    
    float voltage = rawRead();
    
    if (voltage > 1322.0f && voltage < 1678.0f){        // buffer solution:7.0{
        neutralVoltage = voltage;
    }else if (voltage > 1854 && voltage<2210){  //buffer solution:4.0
        acidVoltage = voltage;
    }
    else {
        return false;
    }

    return true;
}

void PH::getCalibration(float &neutralVoltage, float &acidicVoltage)
{
    neutralVoltage = this->neutralVoltage;
    acidicVoltage = this->acidVoltage;
}

void PH::setCalibration(float neutralVoltage, float acidVoltage)
{
    this->neutralVoltage = neutralVoltage;
    this->acidVoltage    = acidVoltage;
}

float PH::readTemp()
{
    return 25.0f;
}

size_t PH::write(char *buffer, uint8_t idx)
{
    sprintf(buffer, "\"ph\": %.6f,", read());
    return strlen(buffer);
}
