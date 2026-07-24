/*
 * sd.h
 *
 * Created: 20.07.2023 14:28:57
 *  Author: qzmn
 */ 


#ifndef SD_H_
#define SD_H_

#include "my_spi.h"
#include "conf_sd.h"
#include "conf_sd_mmc.h"
#include "sd_mmc_protocol.h"
#include "my_rtc.h"
// #include "ds3231.h"
#include "diskio.h"

TCHAR sd_filename[13];
TCHAR sd_dirname[9];
bool sd_eof(void);
uint16_t sd_bytes_read(void);

#define SD_CS_BIT				(1UL << (SD_CS_PIN % 32))
#define SD_CS_PORTGROUP			(SD_CS_PIN / 32UL & 1)
#define SD_CS_DIR_SET()			(PORT->Group[SD_CS_PORTGROUP].DIRSET.reg = SD_CS_BIT)
#define SD_CS_DIR_CLR()			(PORT->Group[SD_CS_PORTGROUP].DIRCLR.reg = SD_CS_BIT)
#define SD_CS_SET()				(PORT->Group[SD_CS_PORTGROUP].OUTSET.reg = SD_CS_BIT)
#define SD_CS_CLR()				(PORT->Group[SD_CS_PORTGROUP].OUTCLR.reg = SD_CS_BIT)

#define SD_MOSI_BIT				(1UL << (SD_MOSI_PIN % 32))
#define SD_MOSI_PORTGROUP		(SD_MOSI_PIN / 32UL & 1)
#define SD_MOSI_DIR_SET()		(PORT->Group[SD_MOSI_PORTGROUP].DIRSET.reg = SD_MOSI_BIT)
#define SD_MOSI_DIR_CLR()		(PORT->Group[SD_MOSI_PORTGROUP].DIRCLR.reg = SD_MOSI_BIT)
#define SD_MOSI_SET()			(PORT->Group[SD_MOSI_PORTGROUP].OUTSET.reg = SD_MOSI_BIT)
#define SD_MOSI_CLR()			(PORT->Group[SD_MOSI_PORTGROUP].OUTCLR.reg = SD_MOSI_BIT)
#define SD_MOSI_PMUXEN_SET()	(PORT->Group[SD_MOSI_PORTGROUP].PINCFG[SD_MOSI_PIN].bit.PMUXEN = 1)
#define SD_MOSI_PMUXEN_CLR()	(PORT->Group[SD_MOSI_PORTGROUP].PINCFG[SD_MOSI_PIN].bit.PMUXEN = 0)

#define SD_MISO_BIT				(1UL << (SD_MISO_PIN % 32))
#define SD_MISO_PORTGROUP		(SD_MISO_PIN / 32UL & 1)
#define SD_MISO_DIR_SET()		(PORT->Group[SD_MISO_PORTGROUP].DIRSET.reg = SD_MISO_BIT)
#define SD_MISO_DIR_CLR()		(PORT->Group[SD_MISO_PORTGROUP].DIRCLR.reg = SD_MISO_BIT)
#define SD_MISO_SET()			(PORT->Group[SD_MISO_PORTGROUP].OUTSET.reg = SD_MISO_BIT)
#define SD_MISO_CLR()			(PORT->Group[SD_MISO_PORTGROUP].OUTCLR.reg = SD_MISO_BIT)
#define SD_MISO_PMUXEN_SET()	(PORT->Group[SD_MISO_PORTGROUP].PINCFG[SD_MISO_PIN].bit.PMUXEN = 1)
#define SD_MISO_PMUXEN_CLR()	(PORT->Group[SD_MISO_PORTGROUP].PINCFG[SD_MISO_PIN].bit.PMUXEN = 0)

#define SD_SCLK_BIT				(1UL << (SD_SCLK_PIN % 32))
#define SD_SCLK_PORTGROUP		(SD_SCLK_PIN / 32UL & 1)
#define SD_SCLK_DIR_SET()		(PORT->Group[SD_SCLK_PORTGROUP].DIRSET.reg = SD_SCLK_BIT)
#define SD_SCLK_DIR_CLR()		(PORT->Group[SD_SCLK_PORTGROUP].DIRCLR.reg = SD_SCLK_BIT)
#define SD_SCLK_SET()			(PORT->Group[SD_SCLK_PORTGROUP].OUTSET.reg = SD_SCLK_BIT)
#define SD_SCLK_CLR()			(PORT->Group[SD_SCLK_PORTGROUP].OUTCLR.reg = SD_SCLK_BIT)
#define SD_SCLK_PMUXEN_SET()	(PORT->Group[SD_SCLK_PORTGROUP].PINCFG[SD_SCLK_PIN].bit.PMUXEN = 1)
#define SD_SCLK_PMUXEN_CLR()	(PORT->Group[SD_SCLK_PORTGROUP].PINCFG[SD_SCLK_PIN].bit.PMUXEN = 0)

/* MMC/SD command (SPI mode) */
#define CMD0    (0)         /* GO_IDLE_STATE */
#define CMD1    (1)         /* SEND_OP_COND */
#define ACMD41  (0x80+41)   /* SEND_OP_COND (SDC) */
#define CMD8    (8)         /* SEND_IF_COND */
#define CMD9    (9)         /* SEND_CSD */
#define CMD10   (10)        /* SEND_CID */
#define CMD12   (12)        /* STOP_TRANSMISSION */
#define ACMD13  (0x80+13)   /* SD_STATUS (SDC) */
#define CMD16   (16)        /* SET_BLOCKLEN */
#define CMD17   (17)        /* READ_SINGLE_BLOCK */
#define CMD18   (18)        /* READ_MULTIPLE_BLOCK */
#define CMD23   (23)        /* SET_BLOCK_COUNT */
#define ACMD23  (0x80+23)   /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24   (24)        /* WRITE_BLOCK */
#define CMD25   (25)        /* WRITE_MULTIPLE_BLOCK */
#define CMD41   (41)        /* SEND_OP_COND (ACMD) */
#define CMD55   (55)        /* APP_CMD */
#define CMD58   (58)        /* READ_OCR */

/* Card type flags (CardType) */
#define CT_MMC      0x01        /* MMC ver 3 */
#define CT_SD1      0x02        /* SD ver 1 */
#define CT_SD2      0x04        /* SD ver 2 */
#define CT_SDC      (CT_SD1|CT_SD2) /* SD */
#define CT_BLOCK    0x08        /* Block addressing */


#define WP              (0)                 /* Card is write protected (yes:true, no:false, default:false) */

#define INIT_PORT()     sd_enable()       /* Initialize MMC control port */
#define FAST_MODE()     sd_set_fast_baud()   /* Maximize SD Card transfer speed */

#define CS_H()          SD_CS_SET()  /* Set MMC CS "high" */
#define CS_L()          SD_CS_CLR()   /* Set MMC CS "low" */

struct t_spi sd_spi;

void sd_init_interface(void);
bool sd_disk_initialize(void);

uint8_t sd_transmit(uint8_t tx);
void sd_read_frame(uint8_t *pBuffer, uint16_t size);
void sd_send_frame(uint8_t *pBuffer, uint16_t size);
void sd_set_baud(uint32_t baud);
void sd_set_fast_baud(void);
void sd_enable(void);
void sd_disable(void);

void sd_dma_rx_512_bytes(BYTE *buff);
void sd_dma_tx_512_bytes(BYTE *buff);

bool sd_mount(void);
void sd_unmount(void);

bool sd_open_read_file(void);
bool sd_open_append_file(void);
bool sd_close_file(void);
bool sd_read_file(uint8_t *buff, uint16_t btr);
bool sd_write_file(const void* buff, uint16_t btw, bool sync);
bool sd_create_dir(void);
bool sd_change_dir(void);

bool sd_file_is_open_append(void);

// DSTATUS disk_initialize (BYTE pdrv);
// DSTATUS disk_status (BYTE pdrv);
// DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count);
// DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count);
// DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);




#endif /* SD_H_ */