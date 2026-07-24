/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "main.h"

static union t_file_header file_header = { .riff = "RIFF", .filesize = 36 /*size_of_data(0)+44(hdrsize)-8, i.e. skolko do kontsa faila*/, 
	.file_type_header = "WAVE", .format_chunk_marker = "fmt ", .length_of_format_data = 16, .type_of_format = 1,
	 };

int main (void)
{
	irq_initialize_vectors();
	cpu_irq_enable();
	system_init();
	sleepmgr_init();
	delay_init();
	
	LED0_INIT();
	LED1_INIT();
	
	sd_power_on();
	LED0_ON();
	LED1_ON();
	delay_ms(500);

	sd_mmc_init();
	sd_init_interface();
	delay_ms(100);

	system_interrupt_enable_global();
	nop();
// 	udc_start();
	udd_enable();
	udd_attach();
	while (9) {
		udi_msc_process_trans();
		sleepmgr_sleep(SLEEPMGR_IDLE_0);
	}

	sd_mmc_init();
	sd_init_interface();
	delay_ms(100);
	sd_disable();
	
	LED0_OFF();
	LED1_OFF();
	
	sd_enable();
CS_H();
	while (9) {
// 		CS_L();
		sd_transmit(0x55);
		CS_H();
		delay_ms(1);
	}
	
	while (!sd_mount()) {
		LED1_OFF();
		delay_ms(50);
		LED1_ON();
		delay_ms(50);
	}
	
	
	file_header.sample_rate = 7680u;
	file_header.bits_per_sample = 24;
	file_header.number_of_channels = 1;
	file_header.sample_rate_x_bits_per_sample_x_channels_div_8 = file_header.sample_rate * file_header.bits_per_sample * file_header.number_of_channels / 8;
	file_header.bits_per_sample_x_channels_div_8 = file_header.bits_per_sample * file_header.number_of_channels / 8;
	
	init_rtc();
	rtc_set_alarm_in_next_n_seconds(1);
	
	system_interrupt_enable_global();
	nop();

	while (9) {
		udi_msc_process_trans();
		sleepmgr_sleep(SLEEPMGR_IDLE_0);
	}
}

void sd_power_on(void)
{
	pin_config(SD_CS_PIN, true, true, GPIO_PMUX_A, GPIO_PMUXEN_DISABLED, GPIO_INEN_DISABLED, GPIO_PULLEN_DISABLED, GPIO_DRVSTR_DISABLED);
	pin_config(SD_MISO_PIN, false, true, GPIO_PMUX_A, GPIO_PMUXEN_DISABLED, GPIO_INEN_DISABLED, GPIO_PULLEN_ENABLED, GPIO_DRVSTR_DISABLED);
	pin_set_default(SD_MOSI_PIN);
	pin_set_default(SD_SCLK_PIN);
	delay_ms(100);
}

void int_handler_RTC_alarm(void)
{
	rtc_set_alarm_in_next_n_seconds(1);
	rtc_get_time(&rtc_dtp.datetime);
	LED1_TGL();
// 	rtc_dtp.decimillis = 0;
// // 	ad7730_standby(!svp_usb_tele_started());
// 	if (opmode == OPMODE_AUTO || (opmode == OPMODE_TELE && svp_usb_hid_is_enabled() && svp_usb_tele_started())) {
// 		if (ad7730_in_standby()) {
// 			ad7730_standby(false);
// 		}
// 		if (rtc_meas_freq_ind() != 0) {
// 			TCC1->INTFLAG.reg = TCC_INTFLAG_OVF;
// 			TCC1->INTENSET.reg = TCC_INTENSET_OVF;
// 			while (TCC1->SYNCBUSY.bit.COUNT) ;
// 			TCC1->COUNT.reg = 0;
// 			while (TCC1->SYNCBUSY.bit.CTRLB) ;
// 			TCC1->CTRLBCLR.reg = TCC_CTRLBCLR_CMD_Msk;
// 			while (TCC1->SYNCBUSY.bit.CTRLB) ;
// 			TCC1->CTRLBSET.reg = TCC_CTRLBSET_CMD_RETRIGGER;
// 		}
// 	} else {
// 		if (!ad7730_in_standby()) {
// 			ad7730_standby(true);
// 		}
// 	}
// 	if (opmode == OPMODE_TELE && svp_usb_hid_is_enabled() && svp_usb_tele_started()) {
// 		data_report.report_type = SVP_USB_REPORT_IN_TYPE_SEND_DATA;
// 		data_report.rtc = rtc_dtp;
// 		udi_hid_generic_send_report_in(data_report.bytes);
// 	}
// 	if (opmode == OPMODE_PC && svp_usb_hid_is_enabled()) {
// 		data_report.report_type = SVP_USB_REPORT_IN_TYPE_SEND_TIME;
// 		data_report.rtc = rtc_dtp;
// 		udi_hid_generic_send_report_in(data_report.bytes);
// 	}
// 	if (opmode == OPMODE_AUTO) {
// 		seconds_el += rtc_decimillis_step();
// 		auto_data_report[auto_data_report_ind].rtc = rtc_dtp;
// 		tick_ready = true;
// 	}
// 	LED_Toggle(LED_1_PIN);
}
