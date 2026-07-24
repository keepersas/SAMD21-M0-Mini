/*
 * main.h
 *
 * Created: 02.10.2024 13:21:26
 *  Author: qzmn
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <asf.h>

#include <delay.h>

// #define NO_LEDS

#include "conf_serial_number.h"

#include "gpio_helper.h"
#include "my_led.h"
#include "my_reg.h"

#include "conf_clocks.h"

#include "sd.h"
#include "my_rtc.h"
#define PIP_PIN					PIN_PA08
#define LEDPIP_PIN				PIN_PA25
#define SIGNAL_TO_PROBE_PIN		PIN_PA09

#include "my_extints.h"

#include "conf_usb.h"
// #include "my_usb_generic_hid.h"

// #include "my_serfl_disk.h"
// #include "my_eeprom.h"
// #include "my_timers.h"

void sd_power_on(void);
// void RTC_alarm_handler(void);

void callback_rtc(void);

#define FILE_HEADER_LEN	44
// 8 name + 2 serial + 3 time + 3 date + 6*2 p_coefs = 28
union t_file_header {
	uint8_t bytes[FILE_HEADER_LEN];
	struct {
		uint8_t riff[4];												// chunkId
		uint32_t filesize;												// chunkSize = size of file - 8
		uint8_t file_type_header[4];									// format = 'WAVE'
		uint8_t format_chunk_marker[4];									// subchunk1Id = 'fmt '
		uint32_t length_of_format_data;									// subchunk1Size = 16 for PCM
		uint16_t type_of_format;										// audioFormat = 1 for PCM
		uint16_t number_of_channels;									// numChannels
		uint32_t sample_rate;											// sampleRate
		uint32_t sample_rate_x_bits_per_sample_x_channels_div_8;		// byteRate
		uint16_t bits_per_sample_x_channels_div_8;						// blockAlign
		uint16_t bits_per_sample;										// bitsPerSample
		uint8_t data_chunk_header[4];										// subchunk2Id = 'data'
		uint32_t size_of_data;											// subchunk2Size = size of data
	};
};

// union t_file_header file_header = {0};
	

#endif /* MAIN_H_ */