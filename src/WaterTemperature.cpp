#include "WaterTemperature.h"

#include <stdlib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

WaterTemperature::WaterTemperature() {}

WaterTemperature::WaterTemperature(uint8_t pin, bool initialize)
    : oneWire(pin), sensor(&oneWire)
{
    if (initialize) {
        init();
        // sensor.requestTemperatures();
        // Serial.println();
        // Serial.println(sensor.getTempFByIndex(0));

        //         sensor.requestTemperatures();
        // Serial.println();
        // Serial.println(sensor.getTempFByIndex(0));
        //         sensor.requestTemperatures();
        // Serial.println();
        // Serial.println(sensor.getTempFByIndex(0));
    }
}

void WaterTemperature::init()
{
    if (!initialized) {
        sensor.begin();
        initialized = true;
    }
}

float WaterTemperature::read(uint8_t idx)
{
    sensor.requestTemperatures();
    return sensor.getTempFByIndex(0);
}

size_t WaterTemperature::write(char *buffer, uint8_t idx)
{
    sprintf(buffer, "%d:%.4f,", idx, read());
    return strlen(buffer);
}
