#ifndef __CONFIG_H__
#define __CONFIG_H__

#define VALVE_NUMBER 5 // 5 valves installed
#define FORCE_ON_PERSISTENT false // is valves force ON setting persistent accross reboots?
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3" // Europe/Paris
int VALVE_GPIO[VALVE_NUMBER] = {
  12, // valve 0
  14, // valve 1
  27, // valve 2
  26, // valve 3
  25, // valve 4
  // 33 // valve 5
};

#endif
