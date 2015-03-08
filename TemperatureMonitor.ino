// Do not remove the include below
#include "TemperatureMonitor.h"
#include <LiquidCrystal.h>  // has to be in the .ino file so the Arduino IDE works properly
#include <OneWire.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
TemperatureDevice *devices[MAX_NUM_DEVICES];

/**
 * Print the contents of an array as hex values
 */
void printHexArray(byte array[], byte size) {
	for (byte i = 0; i < size; i++) {
		Serial.print(array[i], HEX);
		Serial.print(" ");
	}
}

/**
 * Scan for all devices and store found devices in an array.
 */
void findDevices() {
	byte i;

	Serial.println("locating devices...");
	lcd.setCursor(0, 0);
	lcd.print("locating devices");
	lcd.setCursor(0, 1);

	TemperatureDevice::resetSearch();
	for (i = 0; i < MAX_NUM_DEVICES; i++) {
		devices[i] = TemperatureDevice::search();
		if (devices[i] == NULL)
			break;

		Serial.print("found device #");
		Serial.print(i);
		Serial.print(": addr=");
		printHexArray(devices[i]->getAddress(), 8);
		Serial.print(", ");
		Serial.print(devices[i]->getTypeStr());
		Serial.println(" family");

		lcd.print(i);
		lcd.print(" ");
	}
	devices[i] = 0;
}

/**
 * Set resolution for all found devices
 */
void setResolution() {
	TemperatureDevice *device;

	for (byte i = 0; i < MAX_NUM_DEVICES && devices[i] != 0; i++) {
		device = devices[i];

		Serial.print("setting resolution to ");
		Serial.print(RESOLUTION);
		Serial.print(" bits for device ");
		printHexArray(device->getAddress(), 8);
		Serial.println();

		device->setResolution(RESOLUTION);
	}
}

/*
 * Initialise devices
 */
void setup(void) {
	Serial.begin(115200);
	Serial.println("TemparatureMeter");

	lcd.begin(16, 2);
	lcd.setCursor(0, 0);
	lcd.print("TemparatureMeter");

	findDevices();
//	setResolution();

	lcd.clear();
}

/**
 * main loop
 */
void loop(void) {
	byte i;

	TemperatureDevice::prepareData();

	delay(1000);

	lcd.setCursor(0, 0);
	for (i = 0; i < MAX_NUM_DEVICES && devices[i] != 0; i++) {
		TemperatureDevice *device = devices[i];
		device->retrieveData();

		// print to serial
		Serial.print("Sensor #");
		Serial.print(i);
		Serial.print(": ");
		Serial.print(device->getTemperatureCelsius());
		Serial.print(" ");

		// print to LCD
		if (i == 3) { // only 3 read-outs fit on a 16x2 display, switch to next line
			lcd.setCursor(0, 1);
		}
		lcd.print(device->getTemperatureCelsius());
		lcd.print(" ");
	}
	Serial.println();
}
