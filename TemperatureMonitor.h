// Only modify this file to include
// - function definitions (prototypes)
// - include files
// - extern variable definitions
// In the appropriate section

#ifndef _Temperature_H_
#define _Temperature_H_
#include "Arduino.h"

#include "TemperatureDevice.h"

#define RESOLUTION 12      // the desired resolution for the measurement
#define MAX_NUM_DEVICES 16 // the maximum number of devices, adjust if necessary

//some stuff for eclipse plugin/cc
#ifdef __cplusplus
extern "C" {
#endif
void loop();
void setup();
#ifdef __cplusplus
} // extern "C"
#endif

//Do not add code below this line
#endif /* _Temperature_H_ */
