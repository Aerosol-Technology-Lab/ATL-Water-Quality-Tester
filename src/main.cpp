#include "EC.h"
#include "PH.h"
#include "utils.h"
#include <Arduino.h>

#define USE_WATER_TEMPERATURE

char buffer[256];
uint16_t bufferPtr = 0;

// Sensors
PH ph(A3);
EC ec(A2);
const uint8_t turb = A0;
const uint8_t TDS = A1;

// Water Temperature
#ifdef USE_WATER_TEMPERATURE
WaterTemperature waterTemperature(1, false);
#endif

/**
 * @brief Checks if a string contains the following sequence "\r\n". This function
 *          returns the first index where this sequence is found (specifically the
 *          index of '\r'). If not found, -1 is returned.
 * 
 * @param str The string to process
 * @param start The starting index
 * @param size The size of the buffer
 * @return int The index where the first sequence "\r\n" is found. -1 is return if
 *          not found
 */
int scanForTerminator(const char *str, size_t start=0, size_t size=-1)
{
    for (size_t i = start; i < size - 1 && str[i] != 0; ++i) {

        if (str[i] == '\r' && str[i+1] == '\n') return i;
    }

    return -1;
}

int nextWord(const char *str,
              size_t str_size,
              char *buffer,
              size_t buffer_size,
              size_t start,
              size_t end=-1
              )
{
    auto isDelimiter = [](char c) {
        return c == '\r' || c == '\n' || c == ' ';
    };

    end = min(str_size, end);

    // moves pointer to white space
    while (isDelimiter(str[start]) && start < end) {
        ++start;
    }
    
    // begins scanning for delimiter
    size_t i = start;
    while (i < end && str[i] != 0) {
        ++i;
    }

    memcpy(buffer, str + start, min(i - start, buffer_size));

    return ++i; // returns the pointer to the next character
}

bool echo(const char *command, size_t size)
{
    char word[32] = { 0 };
    size_t ptr = nextWord(command, size, word, sizeof(word), 0);

    if (strcmp("/echo", word)) return false;

    char message[64];
    strncpy(message, command + ptr, min(sizeof(message), size - ptr));
    Serial.print("/echo: ");
    Serial.print(message);
    Serial.print("\r\n");

    return true;
}

uint8_t help(const char *command, size_t size)
{
    char nxtWord[32] = { 0 };
    int ptr = nextWord(command, size, nxtWord, sizeof(nxtWord), 0);
    if (!ptr || strcmp(nxtWord, "/help")) {
        return false;
    }
}

// @deprecated
/*
uint8_t phCommand(const char *command, size_t size)
{
    char nxtWord[32] = { 0 };
    int ptr = nextWord(command, size, nxtWord, sizeof(nxtWord), 0);
    if (strcmp(nxtWord, "/ph")) return false;

    char number[16];
    sprintf(number, "/res ph %.4f\r\n");
    Serial.print(number);
}
*/

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(9600);
    Serial.println("-> Initialized");

#ifdef USE_WATER_TEMPERATURE
    waterTemperature.init();
    ec.setWaterTemperatureSensor(&waterTemperature);    
#endif

}

void loop()
{
    if (Serial.available()) {

        char c = Serial.read();

        // do nothing if it doesn't start with a backslash
        if (c != '/') {
            return;
        }

        char c_input[64] = { 0 };
        const long timeout = 1000;
        long start = millis();
        size_t bytesBufferRead = 0;
        do {

            int c = Serial.read();
            if (c >= 0) {

                if (c == '\r' || c == '\n') {
                    break;
                }
                else if (bytesBufferRead + 1 == sizeof(c_input)) {

                    char output[64];
                    sprintf(output, "/err: Too many characters received. Maximum command must be %d characters\r\n", sizeof(c_input) - 1);
                    Serial.print(output);
                    return;
                }

                c_input[bytesBufferRead++] = c;
                start = millis();
            }
        } while (millis() - start < timeout);
        c_input[bytesBufferRead++] = '\0';
        
        {
            char output[128];
            sprintf(output, "-> The command received: \"%s\"\n", c_input);
            Serial.print(output);
        }

        /**
         * Read remaing string
         * Get the first word of the string
         */
        // char c_input[64] = { 0 };
        // {
        //     String input = Serial.readStringUntil('\r');

        //     if (input.length() > sizeof(c_input)) {
        //         char output[32];
        //         sprintf(output, "/err: Input too long. Can only read %d characters\r\n", sizeof(c_input));
        //         Serial.print(output);
        //         return;
        //     }
            
        //     input.toCharArray(c_input, sizeof(c_input));
        // }

        const char SPLITTER[] = " \n\r";
        char *pch = strtok(c_input, SPLITTER);
        
        if (pch == nullptr) {
            Serial.println(F("/err: Invalid command"));
            return;
        }

        // if pch is equal to "flush", flush the buffer
        if (strcmp(pch, "flush") == 0) {
            Serial.flush();
        }
        else if (strcmp(pch, "echo") == 0) {
            Serial.print("/echo ");
            
            if (strtok(nullptr, SPLITTER) == nullptr) {
                Serial.print(c_input + 6);
                Serial.print("\r\n");
                return;
            }
        }
        else if (strcmp(pch, "ph") == 0) {

            pch = strtok(nullptr, SPLITTER);
            if (pch == nullptr) {
                double phVal = ph.read();
                Serial.print("/ph ");
                Serial.println(phVal);
            }
            else {

                // if pch is equal to "calibrate", calibrate the sensor
                if (strcmp(pch, "calibrate") == 0) {
                
                    pch = strtok(nullptr, SPLITTER);
                    // if pch is equal to "start", start the calibration
                    // else if pch is equal to "get", get the calibration value
                    // else if pch is equal to "set", set the calibration value with the neutral voltage and acid voltage
                    // else the command is invalid
                    if (pch == nullptr) {
                        Serial.println(F("/err: ph calibrate - unknown command"));
                        return;
                    }
                    else if (strcmp(pch, "start") == 0) {

                        Serial.print(F("/ph calibrate start\r\n"));

                        // MARKER

                        // Get the current ph calibration values.
                        // Then start the ph calibration.
                        // Then check which ph calibration value changed.
                        // Print to the serial which ph setting is changed, the old value, and the new value
                        float old_neutral_voltage, old_acid_voltage;
                        ph.getCalibration(old_neutral_voltage, old_acid_voltage);
                        ph.calibrate();
                        
                        float new_neutral_voltage, new_acid_voltage;
                        ph.getCalibration(new_neutral_voltage, new_acid_voltage);


                        char output[128];
                        if (old_neutral_voltage != new_neutral_voltage) {
                            sprintf(output, "/ph: calibration neutral from %.4f to %.4f\r\n", old_neutral_voltage, new_neutral_voltage);
                            Serial.print(output);
                        }
                        else if (old_acid_voltage != new_acid_voltage) {
                            sprintf(output, "/ph: calibration acid from %.4f to %.4f\r\n", old_acid_voltage, new_acid_voltage);
                            Serial.print(output);
                        }
                        else {
                            Serial.println(F("/ph: calibration values unchanged"));
                        }

                        Serial.println(F("/ph calibration end\r\n"));
                    }
                    else if (strcmp(pch, "get") == 0) {
                        char output[64];
                        float neutralVoltage, acidVoltage;
                        ph.getCalibration(neutralVoltage, acidVoltage);
                        Serial.print(output);
                        Serial.print(F("/ph calibration data "));
                        Serial.print(neutralVoltage);
                        Serial.print(" ");
                        Serial.println(acidVoltage);
                    }
                    else if (strcmp(pch, "set") == 0) {
                        pch = strtok(nullptr, SPLITTER);
                        if (pch == nullptr) {
                            Serial.println(F("/err: ph calibration set missing neutral voltage"));
                            return;
                        }

                        float neutral = atof(pch);
                        pch = strtok(nullptr, SPLITTER);
                        if (pch == nullptr) {
                            Serial.println(F("/err: ph calibration set missing acid voltage"));
                            return;
                        }

                        float acid = atof(pch);
                        ph.setCalibration(neutral, acid);
                        Serial.println(F("/ph calibration set success"));
                    }
                    else {
                        Serial.println(F("/err: Invalid command"));
                        return;
                    }
                }
                else {
                    Serial.print(F("/err: Invalid command"));
                }
            }
        }
        else if (strcmp(pch, "ec") == 0) {
            
            pch = strtok(nullptr, SPLITTER);

            // if pch is null, print the current ec value
            // else if pch is equal to "calibrate", start the ec calibration
            // else if pch is equal to "get", get the ec calibration value
            // else if pch is equal to "set", set the ec calibration value with the neutral voltage and acid voltage
            // else the command is invalid
            if (pch == nullptr) {
                float ecVal = ec.read();
                Serial.print("/ec ");
                Serial.println(ecVal);
            }
            else if (strcmp(pch, "calibrate") == 0) {
                
                pch = strtok(nullptr, SPLITTER);
                // if pch is equal to "start", start the calibration
                // else if pch is equal to "get", get the calibration value
                // else if pch is equal to "set", set the calibration value with the neutral voltage and acid voltage
                // else the command is invalid
                if (pch == nullptr) {
                    Serial.println("/err: ec calibrate - must specify mode");
                    return;
                }
                else if (strcmp(pch, "start") == 0) {

                    {
                        char output[32];
                        sprintf(output, "/ec calibrate start\r\n");
                        Serial.print(output);
                    }

                    float old_low_value, old_high_value;
                    ec.getCalibration(old_low_value, old_high_value);
                    ec.calibrate();

                    float new_low_value, new_high_value;
                    ec.getCalibration(new_low_value, new_high_value);

                    if (old_low_value != new_low_value) {
                        char output[128];
                        sprintf(output, "/ec: calibration low from %.4f to %.4f\r\n", old_low_value, new_low_value);
                    }
                    else if (old_high_value != new_high_value) {
                        char output[128];
                        sprintf(output, "/ec: calibration high from %.4f to %.4f\r\n", old_high_value, new_high_value);
                    }
                    else {
                        char output[128];
                        sprintf(output, "/ec: calibration values unchanged\r\n");
                    }

                    {
                        char output[32];
                        sprintf(output, "/ec calibration end\r\n");
                        Serial.print(output);
                    }
                }
                else if (strcmp(pch, "get") == 0) {
                    char output[64];
                    float low, high;
                    ec.getCalibration(low, high);
                    sprintf(output, "/ec calibration data %.4f %.4f\r\n", low, high);
                    Serial.print(output);
                }
                else if (strcmp(pch, "set") == 0) {

                    pch = strtok(nullptr, SPLITTER);
                    if (pch == nullptr) {
                        Serial.println("/err: ec calibration set missing two parameters");
                        return;
                    }

                    float low = atoi(pch);
                    pch = strtok(nullptr, SPLITTER);
                    if (pch == nullptr) {
                        Serial.println("/err: ec calibration set missing one parameter");
                        return;
                    }

                    float high = atoi(pch);
                    if (high < low) Utils::swap(low, high);
                    ec.setCalibration(low, high);
                    Serial.println("/ec calibration set success");
                }
            }
        }
        else if (strcmp(pch, "turb") == 0) {

            static float m = 1.0f;
            static float b = 0.0f;

            pch = strtok(nullptr, SPLITTER);

            if (!pch) {

                float turbidity = analogRead(turb);
                float turbidityValues[5];
                for (float &val : turbidityValues) {
                    delay(20);
                    val = analogRead(turb);
                }

                // sort the values
                for (int i = 0; i < sizeof(turbidityValues) / sizeof(turbidityValues[0]) - 1; ++i) {

                    int smallestIdx = i;
                    for (int j = i + 1; j < sizeof(turbidityValues) / sizeof(turbidityValues[0]); ++j) {

                        if (turbidityValues[j] < turbidityValues[smallestIdx]) smallestIdx = j;
                    }

                    Utils::swap(turbidityValues[i], turbidityValues[smallestIdx]);
                }

                Serial.print(F("/turb "));
                Serial.println(turbidityValues[2] * m + b);
            }
            else {

                if (!strcmp(pch, "calibrate")) {
                    
                    pch = strtok(nullptr, SPLITTER);

                    if (!strcmp(pch, "set")) {

                        pch = strtok(nullptr, SPLITTER);
                        if (!pch) {
                            Serial.println(F("/err: turb calibration set missing slope and base"));
                            return;
                        }
                        float mNew = atof(pch);
                        
                        pch = strtok(nullptr, SPLITTER);
                        if (!pch) {
                            Serial.println(F("/err: turb calibration set missing base"));
                            return;
                        }
                        
                        b = atof(pch);
                        m = mNew;
                    }
                    else if (!strcmp(pch, "get")) {

                        char output[64];
                        Serial.print("/turb calibration data m:");
                        Serial.print(m);
                        Serial.print(" b:");
                        Serial.println(b);
                    }
                }
                else if (!strcmp(pch, "help")) {

                    Serial.println("/turb Turbidity list of commands");
                    Serial.println("/turb           - show the turbidity value");
                    Serial.println("/turb calibrate - calibrate the turbidity sensor");
                    Serial.println("/turb help      - show this help");
                }
                else {
                    Serial.println("/err: turb invalid command");
                }
            }
        }
        else {
            Serial.print("/err: Invalid command\r\n");
        }
    }
    
/*     if (Serial.available()) {
        size_t init_bufferPtr = bufferPtr;
        bufferPtr += Serial.readBytesUntil('\n', buffer + bufferPtr, sizeof(buffer) - bufferPtr);

        // if exactly two, then flush output and flush buffer
        if (bufferPtr == 2) {
            
            Serial.flush();
            memset(buffer, 0, sizeof(buffer));
            bufferPtr = 0;
        }
        // not enough bytes to process
        else if (bufferPtr < 2) {
            return;
        }
        // check if last two characters contain the termination sequence
        int terminatorIdx = scanForTerminator(buffer, init_bufferPtr, sizeof(buffer));
        if (terminatorIdx >= 0) {

            // process data request here
            char parsedBuffer[64] = { 0 };
            memcpy(parsedBuffer, buffer, min(terminatorIdx, sizeof(parsedBuffer) - 1));
            memcpy(buffer, buffer + terminatorIdx + 1, bufferPtr - terminatorIdx - 1);
            bufferPtr -= terminatorIdx - 1;
            Command_parse(&container, parsedBuffer, sizeof(parsedBuffer));
        }
        // if buffer is full, then clear buffer because this is an error
        else if (bufferPtr >= sizeof(buffer)) {

            memset(buffer, 0, sizeof(buffer));
            bufferPtr = 0;
            Utils::print("/err: buffer-overflow -> You told me way too many things before I can process them :(");
        }
    }
  */   // put your main code here, to run repeatedly:
}