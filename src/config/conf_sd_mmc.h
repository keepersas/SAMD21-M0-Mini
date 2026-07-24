/**
 * \file
 *
 * \brief SD/MMC stack configuration file.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef CONF_SD_MMC_H_INCLUDED
#define CONF_SD_MMC_H_INCLUDED

#include "conf_sd.h"

/* Define to enable the SPI mode instead of Multimedia Card interface mode */
#define SD_MMC_SPI_MODE

/* Define to enable the SDIO support */
//#define SDIO_SUPPORT_ENABLE

/* Define to enable the debug trace to the current standard output (stdio) */
//#define SD_MMC_DEBUG

/* Define to memory count */
#define SD_MMC_SPI_MEM_CNT          1

#define SD_SPI_SERCOM_MUX_SETTING				SPI_SIGNAL_MUX_SETTING_E
#define SD_SPI_SERCOM_PINMUX_PAD0_MISO			PINMUX_PA08C_SERCOM0_PAD0
#define SD_SPI_SERCOM_PINMUX_PAD1_CS			PINMUX_UNUSED
#define SD_SPI_SERCOM_PINMUX_PAD2_MOSI			PINMUX_PA10C_SERCOM0_PAD2
#define SD_SPI_SERCOM_PINMUX_PAD3_SCK			PINMUX_PA11C_SERCOM0_PAD3
#define SD_SPI_TRANSFER_MODE					SPI_TRANSFER_MODE_0
#define SD_SPI_BAUDRATE							400000UL
#define SD_SPI_BAUDRATE_FAST					8000000UL

#define SD_MMC_SPI					SD_SERCOM
#define SD_MMC_SPI_PINMUX_SETTING	SD_SPI_SERCOM_MUX_SETTING
#define SD_MMC_SPI_PINMUX_PAD0		SD_SPI_SERCOM_PINMUX_PAD0_MISO
#define SD_MMC_SPI_PINMUX_PAD1		SD_SPI_SERCOM_PINMUX_PAD1_CS
#define SD_MMC_SPI_PINMUX_PAD2		SD_SPI_SERCOM_PINMUX_PAD2_MOSI
#define SD_MMC_SPI_PINMUX_PAD3		SD_SPI_SERCOM_PINMUX_PAD3_SCK
// MUX_SETTING_E: PAD0 - MISO, PAD1 - CS, PAD2 - MOSI, PAD3 - SCK

#define SD_MMC_CS					SD_CS_PIN

// #  define SD_MMC_0_CD_GPIO           (EXT1_PIN_10)
// #  define SD_MMC_0_CD_DETECT_VALUE   0
#define SD_MMC_0_CD__SUPPRESS_WARNING

/* Define the SPI clock source */
#define SD_MMC_SPI_SOURCE_CLOCK		SD_SPI_GENERATOR_SOURCE

/* Define the SPI max clock */
// values from 8000001 to 12000000 will result in 12000000 if using ASF spi_init(,,)
#define SD_MMC_SPI_MAX_CLOCK		SD_SPI_BAUDRATE_FAST

#endif /* CONF_SD_MMC_H_INCLUDED */

