/**
 * \file
 *
 * \brief USB configuration file
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

// #include <asf.h>
// #include "compiler.h"

/**
 * USB Device Configuration
 * @{
 */

#define	UDD_CLOCK_GEN		GCLK_GENERATOR_1
#define	UDD_CLOCK_SOURCE	SYSTEM_CLOCK_SOURCE_DFLL

#define USB_DEVICE_ATTACH_AUTO_DISABLE

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_ASF_MSC
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 200 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                  \
	(USB_CONFIG_ATTR_SELF_POWERED)
// 	(USB_CONFIG_ATTR_BUS_POWERED)
// 	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_BUS_POWERED)
// 	(USB_CONFIG_ATTR_REMOTE_WAKEUP|USB_CONFIG_ATTR_SELF_POWERED)

//! USB Device string definitions (Optional)
#define  USB_DEVICE_MANUFACTURE_NAME      "IBSS_LGA"
#define  USB_DEVICE_PRODUCT_NAME          "WaveRec"
#define  USB_DEVICE_SERIAL_NAME           "42"	// Disk SN for MSC

/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
// #define  UDC_VBUS_EVENT(b_vbus_high)	svp_usb_vbus_action(b_vbus_high)
// #define  UDC_SOF_EVENT()				svp_usb_sof_action()
#define  UDC_SUSPEND_EVENT()			my_usb_suspend_action()
#define  UDC_RESUME_EVENT()				my_usb_resume_action()
//! Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
// #define  UDC_REMOTEWAKEUP_ENABLE()        main_remotewakeup_enable()
// #define  UDC_REMOTEWAKEUP_DISABLE()       main_remotewakeup_disable()
//! When a extra string descriptor must be supported
//! other than manufacturer, product and serial string
// #define  UDC_GET_EXTRA_STRING()
//@}

/**
 * USB Device low level configuration
 * When only one interface is used, these configurations are defined by the class module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE       64

//! Two interfaces for this device (MSC + HID mouse)
#define  USB_DEVICE_NB_INTERFACE       2

//! 4 endpoints used by HID generic and MSC interfaces
#define  USB_DEVICE_MAX_EP             4
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of MSC interface
 * @{
 */
//! Vendor name and Product version of MSC interface
#define UDI_MSC_GLOBAL_VENDOR_ID            \
   'I', 'B', 'S', 'S', ' ', 'L', 'G', 'A'
#define UDI_MSC_GLOBAL_PRODUCT_VERSION            \
   '1', '.', '0', '0'

//! Interface callback definition
#define  UDI_MSC_ENABLE_EXT()          my_usb_msc_enable()
#define  UDI_MSC_DISABLE_EXT()         my_usb_msc_disable()

/**
 * USB MSC low level configuration
 * In standalone these configurations are defined by the MSC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#define  UDI_MSC_EP_IN                 (1 | USB_EP_DIR_IN)
#define  UDI_MSC_EP_OUT                (2 | USB_EP_DIR_OUT)

//! Interface number
#define  UDI_MSC_IFACE_NUMBER          0
//@}
//@}


/**
 * Configuration of HID Generic interface
 * @{
 */
//! Interface callback definition
#define  UDI_HID_GENERIC_ENABLE_EXT()        my_usb_generic_enable()
#define  UDI_HID_GENERIC_DISABLE_EXT()       my_usb_generic_disable()
#define  UDI_HID_GENERIC_REPORT_OUT(ptr)     my_usb_hid_report_received(ptr)
#define  UDI_HID_GENERIC_SET_FEATURE(report) my_usb_hid_set_feature(report)

//! Sizes of I/O reports
#define  UDI_HID_REPORT_IN_SIZE             64
#define  UDI_HID_REPORT_OUT_SIZE            64
#define  UDI_HID_REPORT_FEATURE_SIZE        8

//! Sizes of I/O endpoints
#define  UDI_HID_GENERIC_EP_SIZE            8

/**
 * USB HID Mouse low level configuration
 * In standalone these configurations are defined by the HID Mouse module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
// #define  UDI_HID_MOUSE_EP_IN           (3 | USB_EP_DIR_IN)
#define  UDI_HID_GENERIC_EP_OUT   (4 | USB_EP_DIR_OUT)
#define  UDI_HID_GENERIC_EP_IN    (3 | USB_EP_DIR_IN)

//! Interface number
// #define  UDI_HID_MOUSE_IFACE_NUMBER    1
#define  UDI_HID_GENERIC_IFACE_NUMBER     1
//@}
//@}

//@}


/**
 * Description of Composite Device
 * @{
 */
//! USB Interfaces descriptor structure
#define	UDI_COMPOSITE_DESC_T				\
	udi_msc_desc_t udi_msc;					\
	udi_hid_generic_desc_t udi_hid_generic

//! USB Interfaces descriptor value for Full Speed
#define	UDI_COMPOSITE_DESC_FS			\
	.udi_msc                   = UDI_MSC_DESC_FS,	\
	.udi_hid_generic                 = UDI_HID_GENERIC_DESC

//! USB Interfaces descriptor value for High Speed
#define	UDI_COMPOSITE_DESC_HS			\
	.udi_msc                   = UDI_MSC_DESC_HS,	\
	.udi_hid_generic                 = UDI_HID_GENERIC_DESC

//! USB Interface APIs
#define	UDI_COMPOSITE_API					\
	&udi_api_msc,								\
	&udi_api_hid_generic
//@}


/**
 * USB Device Driver Configuration
 * @{
 */
//@}

#include <asf.h>
//! The includes of classes and other headers must be done at the end of this file to avoid compile error

#include "udi_hid_generic.h"
#include "udi_msc.h"
#include "main.h"
#include "my_rtc.h"
#include "my_usb.h"


#endif // _CONF_USB_H_
