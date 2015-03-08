/*
 * TemperatureDevice.h
 *
 *  Created on: 06.03.2015
 *      Author: Michael Neuweiler
 */

#ifndef TEMPERATUREDEVICE_H_
#define TEMPERATUREDEVICE_H_

#include <OneWire.h>

#define SENSOR_PIN 24      // on which pin the 1-wire sensors are connected

class TemperatureDevice
{
public:
    enum DeviceType {
        UNKNOWN,
        DS18S20,
		DS18B20,
		DS1822
    };

    TemperatureDevice(byte address[]);
    static void prepareData();
    static void resetSearch();
    static TemperatureDevice *search();
    DeviceType getType();
    char *getTypeStr();
    byte *getAddress();
    void setResolution(byte resolution);
    void retrieveData();
    float getTemperatureCelsius();
    float getTemperatureFahrenheit();
protected:

private:
    byte address[8];
    DeviceType type;
    int16_t temperature; // integer representation of temperature
};

#endif /* TEMPERATUREDEVICE_H_ */
