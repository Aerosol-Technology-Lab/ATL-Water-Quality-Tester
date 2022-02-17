#pragma once

#include "SensorInterface.h"
#include <stdint.h>
#include "utils.h"
// #include "DFRobot_PH.h"
#include "WaterTemperature.h"

class PH : public SensorInterface
{
private:
    uint8_t pin;
    float neutralVoltage = 1500.0f;
    float acidVoltage = 2032.44f;
    
public:
    PH(uint8_t pin);

    void init();
    
    /**
     * @brief returns ph reading
     * 
     * @param _ unused
     * @return float ph reading
     */
    float read(uint8_t _=0);

    size_t write(char *buffer, uint8_t idx=0);

    bool calibrate();

    void getCalibration(float &neutralVoltage, float &acidicVoltage);

    void setCalibration(float neutralVoltage, float acidicVoltage);

    /**
     * @brief Deprecated - non functioning
     */
    virtual float readTemp();

private:
    inline float rawRead()
    {
        return analogRead(pin) / ANALOG_RESOLUTION * VREF * 1000.0f;
    }
};
