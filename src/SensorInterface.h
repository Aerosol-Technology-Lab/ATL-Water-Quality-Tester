#pragma once

#include <stdint.h>
#include <stdlib.h>

class SensorInterface
{
    virtual void init();
    
    virtual float read(uint8_t idx=0);

    virtual size_t write(char *buffer, uint8_t idx);
};
