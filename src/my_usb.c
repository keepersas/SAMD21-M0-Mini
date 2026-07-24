/*
 * svp_usb.c
 *
 * Created: 23.06.2023 15:36:34
 *  Author: qzmn
 */ 

#include <asf.h>
// #include "svp_usb.h"
#include "conf_usb.h"

static volatile bool my_usb_b_generic_enable = false;
static volatile bool my_usb_b_msc_enable = false;

static volatile bool my_usb_b_vbus = false;
static volatile bool my_usb_b_tele_started = false;


// static uint8_t _data_report_ind;
// static union t_my_usb_data_report_in _data_report[2] = { { .cmd = my_usb_REPORT_IN_TYPE_SEND_DATA }, { .cmd = my_usb_REPORT_IN_TYPE_SEND_DATA } };
	
static union t_my_usb_report_in report_in = { .bytes = {} };
static union t_my_usb_report_out report_out;
	
bool my_usb_tele_started(void)
{
	return my_usb_b_tele_started;
}

bool my_usb_msc_is_enabled(void)
{
	return my_usb_b_msc_enable;
}

bool my_usb_hid_is_enabled(void)
{
	return my_usb_b_generic_enable;
}

bool my_usb_all_enabled(void)
{
	return my_usb_b_msc_enable && my_usb_b_generic_enable;
}

void my_usb_vbus_action(bool vbus_is_high)
{
	my_usb_b_vbus = vbus_is_high;
}

bool my_usb_vbus_is_high(void)
{
// 	return my_usb_b_vbus;
	return port_pin_get_input_level(USB_VBUS_PIN);
}

// void my_usb_vbus_int_handler(void)
// {
// 	my_usb_b_vbus = port_pin_get_input_level(USB_VBUS_PIN);
// 	
// // port_pin_set_output_level(LED_1_PIN, !my_usb_vbus_is_high());
// 
// // 	extint_chan_disable_callback(USB_VBUS_EIC_LINE,	EXTINT_CALLBACK_TYPE_DETECT);
// // # ifdef UDC_VBUS_EVENT
// // 	UDC_VBUS_EVENT(port_pin_get_input_level(USB_VBUS_PIN));
// // # endif
// // 	extint_chan_enable_callback(USB_VBUS_EIC_LINE, EXTINT_CALLBACK_TYPE_DETECT);	
// }

void my_usb_hid_set_feature(uint8_t* report)
{
// 	uint32_t t = report[0] | ((uint32_t)report[1] << 8) | ((uint32_t)report[2] << 16) | ((uint32_t)report[3] << 24);
// 	rtc_set_timestamp(t);
}

// void my_usb_hid_report_received(uint8_t *report)
// void my_usb_hid_report_received(union t_my_usb_report_out *report_out)
void my_usb_hid_report_received(uint8_t rep_out[UDI_HID_REPORT_OUT_SIZE])
{
	memcpy(report_out.bytes, rep_out, UDI_HID_REPORT_OUT_SIZE);
	memset(report_in.bytes, 0, UDI_HID_REPORT_IN_SIZE);
	switch (report_out.cmd) {
	case my_usb_REPORT_OUT_TYPE_SET_TIME:
// 		rtc_get_time(&dtp);
		// no break - so, go to next case, send time
	case MY_USB_REPORT_OUT_TYPE_GET_TIME:
// 		report_in.time.cmd = my_usb_REPORT_IN_TYPE_SEND_TIME;
// 		t_rtc_datetime_plus dtp;
// 		if (ds3231_get_samd_datetime(&dtp)) {
// 			report_in.time.rtc.datetime.timestamp = dtp.datetime.timestamp;
// 		} else {
// 			report_in.time.rtc.datetime.timestamp = 0;
// 		}
// 		udi_hid_generic_send_report_in(report_in.time.bytes);
		break;
	case MY_USB_REPORT_OUT_TYPE_START:
// 		report_in.cmd = my_usb_REPORT_IN_TYPE_START;
// 		udi_hid_generic_send_report_in(report_in.bytes);
// 		my_usb_b_tele_started = true;
// 		if (on_tele_startstop != NULL) {
// 			on_tele_startstop(true);
// 		}
		break;
	case MY_USB_REPORT_OUT_TYPE_STOP:
// 		report_in.cmd = my_usb_REPORT_IN_TYPE_STOP;
// 		udi_hid_generic_send_report_in(report_in.bytes);
// 		my_usb_b_tele_started = false;
// 		if (on_tele_startstop != NULL) {
// 			on_tele_startstop(false);
// 		}
		break;
	case MY_USB_REPORT_OUT_TYPE_RESET:
		{
			report_in.cmd = my_usb_REPORT_IN_TYPE_RESET;
// 			uint8_t report_reset[UDI_HID_REPORT_IN_SIZE] = {my_usb_REPORT_IN_TYPE_RESET};
			if (udi_hid_generic_send_report_in(report_in.bytes)) {
				cpu_irq_enter_critical();
				delay_ms(100);
				NVIC_SystemReset();
			}
			// 		udi_hid_generic_send_report_in(0xBA);
		}
		break;
	default:
		break;
	};
// 	e_my_usb_report_out_type report_type = report[0];
// 	uint32_t t;
// 	switch (report_type) {
// 	case my_usb_REPORT_OUT_TYPE_SET_TIME:
// 		t = report[1] | ((uint32_t)report[2] << 8) | ((uint32_t)report[3] << 16) | ((uint32_t)report[4] << 24);
// 		rtc_set_timestamp(t);
// 		rtc_set_alarm_in_next_n_seconds(2);
// 		break;
// 	case my_usb_REPORT_OUT_TYPE_GET_EE_PAGE:
// 		break;
// 	case my_usb_REPORT_OUT_TYPE_SET_EE_PAGE:
// // 		if (ee_write_page())
// 		break;
// 	case my_usb_REPORT_OUT_TYPE_RESET:
// 	{
// 		uint8_t report_reset[UDI_HID_REPORT_IN_SIZE] = {my_usb_REPORT_IN_TYPE_RESET};
// 		if (udi_hid_generic_send_report_in(report_reset)) {
// 			cpu_irq_enter_critical();
// 			delay_ms(100);			
// 			NVIC_SystemReset();
// 		}
// // 		udi_hid_generic_send_report_in(0xBA);
// 	}
// 		break;
// 	default:
// 		break;
// 	};
}

void my_usb_sof_action(void)
{
	return;
	
	if ((!my_usb_b_generic_enable) || (!my_usb_b_msc_enable))
		return;

// 	uint16_t framenumber = udd_get_frame_number();
// 	uint32_t fn1000 = framenumber % 1000;
// 	if (fn1000 == 0) {
// // 		LED_On(LED_0_PIN);
// 		port_pin_set_output_level(LED_1_PIN, false);
// 		cpu_irq_enter_critical();
// 		my_usb_time_report.rtc = rtc_dtp;
// // 		uint32_t timestamp = rtc_dtp.datetime.timestamp;
// // 		timestamp = rtc_get_timestamp();
// // 		my_usb_hid_report_in[0] = my_usb_REPORT_IN_TYPE_SEND_TIME;
// // 		my_usb_hid_report_in[1] = timestamp & 0xFF; timestamp >>= 8;
// // 		my_usb_hid_report_in[2] = timestamp & 0xFF; timestamp >>= 8;
// // 		my_usb_hid_report_in[3] = timestamp & 0xFF; timestamp >>= 8;
// // 		my_usb_hid_report_in[4] = timestamp & 0xFF;
// 		cpu_irq_leave_critical();
// // 		udi_hid_generic_send_report_in(my_usb_hid_report_in);
// 		udi_hid_generic_send_report_in(my_usb_time_report.bytes);
// 	}
// 	if (fn1000 == 100) {
// // 		LED_Off(LED_0_PIN);
// 		port_pin_set_output_level(LED_1_PIN, true);
// 	}
// 	if (fn1000 % 100 == 0) {
// 		my_usb_data_report_ind ^= 1;
// 		cpu_irq_enter_critical();
// 		my_usb_data_report[my_usb_data_report_ind ^ 1].rtc = rtc_dtp;
// 		cpu_irq_leave_critical();
// 		udi_hid_generic_send_report_in(my_usb_data_report[my_usb_data_report_ind ^ 1].bytes);
// 	}
}

bool my_usb_generic_enable(void)
{
	my_usb_b_generic_enable = true;
	return true;
}

void my_usb_generic_disable(void)
{
	my_usb_b_generic_enable = false;
	my_usb_b_tele_started = false;
}

bool my_usb_msc_enable(void)
{
	my_usb_b_msc_enable = true;
	return true;
}

void my_usb_msc_disable(void)
{
	my_usb_b_msc_enable = false;
}

void my_usb_suspend_action(void)
{
	// 	ui_powerdown();
}

void my_usb_resume_action(void)
{
	// 	ui_wakeup();
}

// void main_remotewakeup_enable(void)
// {
// 	ui_wakeup_enable();
// }
// 
// void main_remotewakeup_disable(void)
// {
// 	ui_wakeup_disable();
// }

