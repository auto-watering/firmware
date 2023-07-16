#ifndef __TIMEDATE_H__
#define __TIMEDATE_H__

void time_init(void);
String time_get_formatted(void);
bool time_get(int *hour, int *minute);

#endif
