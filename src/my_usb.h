/*
 * svp_usb.h
 *
 * Created: 23.06.2023 15:36:17
 *  Author: qzmn
 */ 


#ifndef my_usb_H_
#define my_usb_H_

#include <asf.h>

#include "my_rtc.h"

#define USB_VBUS_PIN		PIN_PA03
#define USB_VBUS_EIC_LINE	3
#define USB_VBUS_EIC_MUX	MUX_PA03A_EIC_EXTINT3
#define USB_VBUS_EIC_PINMUX	PINMUX_PA03A_EIC_EXTINT3

// OUT - from host, IN - to host
typedef enum { 
	my_usb_REPORT_OUT_TYPE_SET_TIME,
	MY_USB_REPORT_OUT_TYPE_GET_TIME,
	MY_USB_REPORT_OUT_TYPE_START,
	MY_USB_REPORT_OUT_TYPE_STOP,
	my_usb_REPORT_OUT_TYPE_SET_EE_PAGE,
	my_usb_REPORT_OUT_TYPE_GET_EE_PAGE,
	my_usb_REPORT_OUT_TYPE_GET_BATT,
	MY_USB_REPORT_OUT_TYPE_RESET = 0xAB,
	my_usb_REPORT_OUT_TYPE_FFFF = 0xFFFF } e_my_usb_report_out_type;
typedef enum {
	my_usb_REPORT_IN_TYPE_SEND_TIME,
	my_usb_REPORT_IN_TYPE_START,
	my_usb_REPORT_IN_TYPE_STOP,
	my_usb_REPORT_IN_TYPE_SEND_DATA,
	my_usb_REPORT_IN_TYPE_SEND_EE_PAGE,
	my_usb_REPORT_IN_TYPE_SEND_BATT,
	my_usb_REPORT_IN_TYPE_RESET = 0xBA,
	my_usb_REPORT_IN_TYPE_FFFF = 0xFFFF } e_my_usb_report_in_type;

union t_my_usb_time_report_out {
	uint8_t bytes[UDI_HID_REPORT_OUT_SIZE];
	struct {
	};
};

union t_my_usb_report_out {
	union t_my_usb_time_report_out time;
	struct {
		e_my_usb_report_out_type cmd;
		uint16_t param;
	};
	uint8_t bytes[UDI_HID_REPORT_OUT_SIZE];
};

union t_my_usb_data_report_in {
	uint8_t bytes[UDI_HID_REPORT_IN_SIZE];
	struct {
		struct {
			e_my_usb_report_in_type cmd;
			uint16_t param;
		};
		t_rtc_datetime_plus rtc;
	};
};

union t_my_usb_time_report_in {
	uint8_t bytes[UDI_HID_REPORT_IN_SIZE];
	struct {
		struct {
			e_my_usb_report_in_type cmd;
			uint16_t param;
		};
		t_rtc_datetime_plus rtc;
	};
};

union t_my_usb_report_in {
	union t_my_usb_data_report_in data;
	union t_my_usb_time_report_in time;
	struct {
		e_my_usb_report_in_type cmd;
		uint16_t param;
	};
	uint8_t bytes[UDI_HID_REPORT_IN_SIZE];
};

// union t_my_usb_report_out;
// union t_my_usb_data_report_in;
// union t_my_usb_report_in;

// static union t_my_usb_data_report my_usb_time_report;


typedef void (*on_eeprom_set_callback_t)(uint8_t logical_page, uint8_t *bytes);
extern on_eeprom_set_callback_t on_eeprom_set;
typedef void (*on_tele_startstop_callback_t)(bool start);
extern on_tele_startstop_callback_t on_tele_startstop;

bool my_usb_tele_started(void);

bool my_usb_msc_is_enabled(void);
bool my_usb_hid_is_enabled(void);
bool my_usb_all_enabled(void);
void my_usb_vbus_action(bool vbus_is_high);
bool my_usb_vbus_is_high(void);

// void my_usb_vbus_int_handler(void);

/*! \brief Called by MSC interface
 * Callback running when USB Host enable MSC interface
 *
 * \retval true if MSC startup is ok
 */
bool my_usb_msc_enable(void);

/*! \brief Called by MSC interface
 * Callback running when USB Host disable MSC interface
 */
void my_usb_msc_disable(void);

/*! \brief Called by HID interface
 * Callback running when USB Host enable generic interface
 *
 * \retval true if generic startup is ok
 */
bool my_usb_generic_enable(void);

/*! \brief Called by HID interface
 * Callback running when USB Host disable generic interface
 */
void my_usb_generic_disable(void);

/*! \brief Called by UDI HID generic when USB Host send a feature request
 */
void my_usb_hid_set_feature(uint8_t* report);

//! This function is called when a HID report is received
//!
// void my_usb_hid_report_received(uint8_t *report);
// void my_usb_hid_report_received(union t_my_usb_report_out *report_out);
void my_usb_hid_report_received(uint8_t rep_out[UDI_HID_REPORT_OUT_SIZE]);

/*! \brief Called when a start of frame is received on USB line
 */
void my_usb_sof_action(void);

/*! \brief Enters the application in low power mode
 * Callback called when USB host sets USB line in suspend state
 */
void my_usb_suspend_action(void);

/*! \brief Called by UDD when the USB line exit of suspend state
 */
void my_usb_resume_action(void);

/*! \brief Called by UDC when USB Host request to enable remote wakeup
 */
void main_remotewakeup_enable(void);

/*! \brief Called by UDC when USB Host request to disable remote wakeup
 */
void main_remotewakeup_disable(void);



#endif /* my_usb_H_ */