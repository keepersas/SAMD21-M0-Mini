/*
 * conf_serial_number.h
 *
 * Created: 20.12.2024 15:19:02
 *  Author: qzmn
 */ 


#ifndef CONF_SERIAL_NUMBER_H_
#define CONF_SERIAL_NUMBER_H_

#define SERIAL	2

#define xstr(s) str(s)
#define str(s) #s

#if SERIAL < 10
#  define SERIAL_HELPER_(x) 0 ## x
#else
#  define SERIAL_HELPER_(x) x
#endif

#define SERIAL_STR1(x)  SERIAL_HELPER_(x)
#define SERIAL_STR  xstr(SERIAL_STR1(SERIAL))


#endif /* CONF_SERIAL_NUMBER_H_ */