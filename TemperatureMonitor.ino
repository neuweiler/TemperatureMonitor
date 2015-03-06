// Do not remove the include below
#include "TemperatureMonitor.h"
#include <OneWire.h>
#include <LiquidCrystal.h>

#define RESOLUTION 11      // the desired resolution for the measurement
#define SENSOR_PIN 30      // on which pin the 1-wire sensors are connected
#define MAX_NUM_DEVICES 16 // maximum number of devices to be supported

OneWire ds(SENSOR_PIN); // DS18B20 Temperature chip i/o
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
byte deviceAdress[MAX_NUM_DEVICES][8]; // array to hold all found sensor addresses
byte deviceType[MAX_NUM_DEVICES]; // array to hold the types of all found sensors (0=unknown, 1=DS18S80, 2=DS18x80)

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
 * Set the resolution of the DS18B20 between 9 or 12 bits.
 */
void setResolution() {
	byte i;

	if (RESOLUTION > 12 || RESOLUTION < 9)
		return;

	for (i = 0; i < MAX_NUM_DEVICES && deviceAdress[i][0] != 0; i++) {
		if (deviceType[i] != 2)
			continue;

		Serial.print("setting resolution to ");
		Serial.print(RESOLUTION);
		Serial.print(" bits for device ");
		printHexArray(deviceAdress[i], 8);
		Serial.println();

		// Get byte for desired resolution
		byte resolutionByte = 0x1F; // 9 bit
		if (RESOLUTION == 12) {
			resolutionByte = 0x7F;
		} else if (RESOLUTION == 11) {
			resolutionByte = 0x5F;
		} else if (RESOLUTION == 10) {
			resolutionByte = 0x3F;
		}

		// set configuration
		ds.reset();
		ds.select(deviceAdress[i]);
		ds.write(0x4E);         // Write scratchpad
		ds.write(0);            // TL
		ds.write(0);            // TH
		ds.write(resolutionByte);      // Configuration Register
		ds.write(0x48);         // Copy Scratchpad
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

	ds.reset_search();
	for (i = 0; i < MAX_NUM_DEVICES && ds.search(deviceAdress[i]); i++) {
		Serial.print("found device #");
		Serial.print(i);
		Serial.print(": addr=");
		printHexArray(deviceAdress[i], 8);
		lcd.print(i);
		lcd.print(" ");

		if (OneWire::crc8(deviceAdress[i], 7) != deviceAdress[i][7]) {
			Serial.print("invalid CRC!\n");
			continue;
		}

		if (deviceAdress[i][0] == 0x10) {
			Serial.print("DS18S20");
			deviceType[i] = 1;
		} else if (deviceAdress[i][0] == 0x28) {
			Serial.print("DS18B20");
			deviceType[i] = 2;
		} else if (deviceAdress[i][0] == 0x22) {
			Serial.print("DS1822");
			deviceType[i] = 3;
		} else {
			Serial.print("not recognized: 0x");
			Serial.println(deviceAdress[i][0], HEX);
			deviceType[i] = 0;
		}
		Serial.println(" family");
	}
	deviceAdress[i][0] = 0; // clear out the last entry
}

/**
 * Calculate the temperature from the two data bytes.
 */
float calculateTemperature(byte *data, byte deviceType) {
	int16_t raw = (data[1] << 8) | data[0];

	if (deviceType == 1) {
		raw = raw << 3; // 9 bit resolution default
		if (data[7] == 0x10) { // "count remain" gives full 12 bit resolution
			raw = (raw & 0xFFF0) + 12 - data[6];
		}
	} else {
		byte cfg = (data[4] & 0x60);

		// at lower res, the low bits are undefined, so let's zero them
		if (cfg == 0x00)
			raw = raw & ~7;  // 9 bit resolution, 93.75 ms
		else if (cfg == 0x20)
			raw = raw & ~3; // 10 bit res, 187.5 ms
		else if (cfg == 0x40)
			raw = raw & ~1; // 11 bit res, 375 ms
	}
	return (float) raw / 16.0; // celsius
//	return  (float)raw / 16.0 * 1.8 + 32.0; //fahrenheit
}

/*
 * Initialize devices
 */
void setup(void) {
	Serial.begin(115200);
	Serial.println("TemparatureMeter");

	lcd.begin(16, 2);
	lcd.setCursor(0, 0);
	lcd.print("TemparatureMeter");

	findDevices();
	setResolution();
	lcd.clear();
}

/**
 * Order the temperature sensors to prepare data.
 */
void prepareData() {
	ds.reset();
	ds.skip(); // skip ROM - send to all devices
	ds.write(0x44); // start conversion
}

/**
 * Retrieve prepared data from temperature sensors
 */
void retrieveData() {
	byte i;
	byte data[RESOLUTION];
	float temperature;

	lcd.setCursor(0, 0);
	for (i = 0; i < MAX_NUM_DEVICES && deviceAdress[i][0] != 0; i++) {
		ds.reset();
		ds.select(deviceAdress[i]);
		ds.write(0xBE); // read scratchpad
		ds.read_bytes(data, 9); // 9 bytes are required

		temperature = calculateTemperature(data, deviceType[i]);

		// print to serial
		Serial.print("Sensor #");
		Serial.print(i);
		Serial.print(": ");
		Serial.print(temperature);
		Serial.print(" ");

		// print to LCD
		if (i == 3) { // only 3 read-outs fit on a 16x2 display, switch to next line
			lcd.setCursor(0, 1);
		}
		lcd.print(temperature);
		lcd.print(" ");
	}
	Serial.println();
}

/**
 * main loop
 */
void loop(void) {
	prepareData();
	delay(1000);
	retrieveData();
}
