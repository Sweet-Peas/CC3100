/*
 * debug.h
 *
 *  Created on: 6 jul 2014
 *      Author: pontus
 */


#ifndef DEBUGUTILS_H
#define DEBUGUTILS_H

int dprintf(const char *format, ...);
int dsprintf(char *out, const char *format, ...);

#ifdef DEBUG
#define debug_init(baud)      DEBUG_PORT.begin(baud)
#define DEBUG_PRINT(str, ...) dprintf("%010u " __FILE__ "@%d " str "\n", \
                              (int)millis(), __LINE__, ##__VA_ARGS__)
#else
#define debug_init(baud)
#define DEBUG_PRINT(str, ...)
#endif

#endif
