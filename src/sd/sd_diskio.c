/*
 * sd_diskio.c
 *
 * Created: 20.07.2023 14:27:36
 *  Author: qzmn
 */ 

#include "sd.h"
#include "delay.h"

static DSTATUS Stat = STA_NOINIT;  /* Disk status */
static BYTE CardType;          /* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */
static BYTE INS = 1;    // KLQ

static inline void xmit_mmc (
const BYTE* buff,               /* Data to be sent */
UINT bc                         /* Number of bytes to send */
)
{
	sd_send_frame((uint8_t *)buff, bc);
}

static inline void rcvr_mmc (
BYTE *buff, /* Pointer to read buffer */
UINT bc     /* Number of bytes to receive */
)
{
	sd_read_frame(buff, bc);
}

// int wait_ready (void);    /* 1:OK, 0:Timeout */
static 
int wait_ready (void)    /* 1:OK, 0:Timeout */
{
// 	for (uint16_t i=0xFFFF; i-- && (sd_transmit(0xFF) ^ 0x80); ) ;
	//    BYTE d;
	UINT tmr;
	for (tmr = 50000; tmr; tmr--) {    /* Wait for ready in timeout of 500ms */
		//        rcvr_mmc(&d, 1);
		//        if (d == 0xFF) return 1;
		if (sd_transmit(0xFF) == 0xFF) {
			return 1;
		}
// 		DLY_US(FCPU, 10);
		cpu_delay_us(10);
	}
	return 0;
}

static void deselect (void)
{
	CS_H();
	sd_transmit(0xFF);
}

// int sdselect (void);    /* 1:OK, 0:Timeout */
static 
int sdselect (void)    /* 1:OK, 0:Timeout */
{
	CS_L();
	if (!wait_ready()) {
		deselect();
		return 0;
	}
	return 1;
}

static int rcvr_datablock (    /* 1:OK, 0:Failed */
BYTE *buff,            /* Data buffer to store received data */
UINT btr            /* Byte count */
)
{
	BYTE d[2];
	UINT tmr;
	for (tmr = 10000; tmr; tmr--) {    /* Wait for data packet in timeout of 100ms */
		//        rcvr_mmc(d, 1);
		d[0] = sd_transmit(0xFF);
		if (d[0] != 0xFF) break;
// 		DLY_US(FCPU, 10);
		cpu_delay_us(10);
	}
	if (d[0] != 0xFE) return 0;        /* If not valid data token, return with error */

	if (btr == 512) {
 		sd_dma_rx_512_bytes(buff);
	} else {
		rcvr_mmc(buff, btr);            /* Receive the data block into buffer */
	}
	//    rcvr_mmc(d, 2);                    /* Discard CRC */
	sd_transmit(0xFF);
	sd_transmit(0xFF);

	return 1;                        /* Return with success */
}

static int xmit_datablock (    /* 1:OK, 0:Failed */
const BYTE *buff,    /* 512 byte data block to be transmitted */
BYTE token            /* Data/Stop token */
)
{
	BYTE d[2];
	if (!wait_ready()) return 0;
	d[0] = token;
	//    xmit_mmc(d, 1);                /* Xmit a token */
	sd_transmit(d[0]);
	if (token != 0xFD) {        /* Is it data token? */
		//        xmit_mmc(buff, 512);    /* Xmit the 512 byte data block to MMC */
		sd_dma_tx_512_bytes((BYTE*)buff);
		//        rcvr_mmc(d, 2);            /* Dummy CRC (FF,FF) */
// 		volatile uint8_t mon1 = sd_transmit(0xFF);
// 		volatile uint8_t mon2 = sd_transmit(0xFF);
		sd_transmit(0xFF);
		sd_transmit(0xFF);
		//        rcvr_mmc(d, 1);            /* Receive data response */
		//        if ((d[0] & 0x1F) != 0x05)    /* If not accepted, return with error */
		//            return 0;
		if ((sd_transmit(0xFF) & 0x1F) != 0x05)    /* If not accepted, return with error */
		return 0;
	}
	return 1;
}

/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static BYTE send_cmd (        /* Returns command response (bit7==1:Send failed)*/
BYTE cmd,        /* Command byte */
DWORD arg        /* Argument */
)
{
	BYTE n, d, buf[6];

	if (cmd & 0x80) {    /* ACMD<n> is the command sequense of CMD55-CMD<n> */
		cmd &= 0x7F;
		n = send_cmd(CMD55, 0);
		if (n > 1) return n;
	}

	/* Select the card and wait for ready */
	deselect();
	if (!sdselect()) {
		return 0xFF;
	}
	/* Send a command packet */
	buf[0] = 0x40 | cmd;            /* Start + Command index */
	buf[1] = (BYTE)(arg >> 24);        /* Argument[31..24] */
	buf[2] = (BYTE)(arg >> 16);        /* Argument[23..16] */
	buf[3] = (BYTE)(arg >> 8);        /* Argument[15..8] */
	buf[4] = (BYTE)arg;                /* Argument[7..0] */
	n = 0x01;                        /* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;        /* (valid CRC for CMD0(0)) */
	if (cmd == CMD8) n = 0x87;        /* (valid CRC for CMD8(0x1AA)) */
	buf[5] = n;
	xmit_mmc(buf, 6);

	/* Receive command response */
	//    if (cmd == CMD12) rcvr_mmc(&d, 1);    /* Skip a stuff byte when stop reading */
	if (cmd == CMD12) sd_transmit(0xFF);    /* Skip a stuff byte when stop reading */
	n = 10;                                /* Wait for a valid response in timeout of 10 attempts */
	do
	//        rcvr_mmc(&d, 1);
	d = sd_transmit(0xFF);
	while ((d & 0x80) && --n);

	return d;            /* Return with the response value */
}

DSTATUS disk_initialize (BYTE pdrv)
{
	BYTE n, ty, 
// 	cmd,
	buf[4];
	UINT tmr;
	DSTATUS s;

// 	sd_enable();
	INIT_PORT();                /* Initialize control port */

// 	DLY_US(FCPU, 100);
// 	cpu_delay_us(100);

	s = disk_status(pdrv);        /* Check if card is in the socket */
	if (s & STA_NODISK) return s;

	CS_H();
	//    for (n = 10; n; n--) rcvr_mmc(buf, 1);    /* 80 dummy clocks */
	for (n = 10; n; n--) sd_transmit(0xFF);    /* 80 dummy clocks */

	ty = 0;
	if (send_cmd(CMD0, 0) == 1) {            /* Enter Idle state */
		if (send_cmd(CMD8, 0x1AA) == 1) {    /* SDv2? */
			rcvr_mmc(buf, 4);                            /* Get trailing return value of R7 resp */
			if (buf[2] == 0x01 && buf[3] == 0xAA) {        /* The card can work at vdd range of 2.7-3.6V */
				for (tmr = 1000; tmr; tmr--) {            /* Wait for leaving idle state (ACMD41 with HCS bit) */
					if (send_cmd(ACMD41, 1UL << 30) == 0) break;
// 					DLY_US(FCPU, 1000);
					cpu_delay_us(1000);
				}
				if (tmr && send_cmd(CMD58, 0) == 0) {    /* Check CCS bit in the OCR */
					rcvr_mmc(buf, 4);
					ty = (buf[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;    /* SDv2 */
				}
			}
		} else {                            /* SDv1 or MMCv3 */
			if (send_cmd(ACMD41, 0) <= 1)     {
				ty = CT_SD1;
// 				cmd = ACMD41;    /* SDv1 */
			} else {
				ty = CT_MMC;
// 				cmd = CMD1;    /* MMCv3 */
			}
			for (tmr = 1000; tmr; tmr--) {            /* Wait for leaving idle state */
				if (send_cmd(ACMD41, 0) == 0) break;
// 				DLY_US(FCPU, 1000);
				cpu_delay_us(100);
			}
			if (!tmr || send_cmd(CMD16, 512) != 0)    /* Set R/W block length to 512 */
			ty = 0;
		}
	}
	CardType = ty;
	deselect();

	if (ty) {      /* Initialization succeded */
		FAST_MODE();
		s &= ~STA_NOINIT;
	} else {       /* Initialization failed */
		s |= STA_NOINIT;
	}
	Stat = s;
	return s;
}

DSTATUS disk_status (BYTE pdrv)
{
	DSTATUS s = Stat;

	if (pdrv || !INS) {
		s = STA_NODISK | STA_NOINIT;
	} else {
		s &= ~STA_NODISK;
		if (WP)
			s |= STA_PROTECT;
		else
			s &= ~STA_PROTECT;
	}
	Stat = s;

	return s;
}

DRESULT disk_read (BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	DSTATUS s;

	s = disk_status(pdrv);
	if (s & STA_NOINIT) return RES_NOTRDY;
	if (!count) return RES_PARERR;
	if (!(CardType & CT_BLOCK)) sector *= 512;    /* Convert LBA to byte address if needed */

	if (count == 1) {    /* Single block read */
		if ((send_cmd(CMD17, sector) == 0)    /* READ_SINGLE_BLOCK */
		&& rcvr_datablock(buff, 512))
		count = 0;
	}
	else {                /* Multiple block read */
		if (send_cmd(CMD18, sector) == 0) {    /* READ_MULTIPLE_BLOCK */
			do {
				if (!rcvr_datablock(buff, 512)) break;
				buff += 512;
			} while (--count);
			send_cmd(CMD12, 0);                /* STOP_TRANSMISSION */
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}

DRESULT disk_write (BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
	DSTATUS s;

	s = disk_status(pdrv);
	if (s & STA_NOINIT) return RES_NOTRDY;
	if (s & STA_PROTECT) return RES_WRPRT;
	if (!count) return RES_PARERR;
	if (!(CardType & CT_BLOCK)) sector *= 512;    /* Convert LBA to byte address if needed */

	if (count == 1) {    /* Single block write */
		if ((send_cmd(CMD24, sector) == 0)    /* WRITE_BLOCK */
		&& xmit_datablock(buff, 0xFE))
		count = 0;
	}
	else {                /* Multiple block write */
		if (CardType & CT_SDC) send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0) {    /* WRITE_MULTIPLE_BLOCK */
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD))    /* STOP_TRAN token */
			count = 1;
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
	DRESULT res;
	BYTE n, csd[16];
	WORD cs;
	if (disk_status(pdrv) & STA_NOINIT)                    /* Check if card is in the socket */
	return RES_NOTRDY;

	res = RES_ERROR;
	switch (cmd) {
		case CTRL_SYNC :        /* Make sure that no pending write process */
			if (sdselect()) {
				deselect();
				res = RES_OK;
			}
			break;

		case GET_SECTOR_COUNT :    /* Get number of sectors on the disk (DWORD) */
			if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
				if ((csd[0] >> 6) == 1) {    /* SDC ver 2.00 */
					cs= csd[9] + ((WORD)csd[8] << 8) + 1;
					*(DWORD*)buff = (DWORD)cs << 10;
					} else {                    /* SDC ver 1.XX or MMC */
					n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
					cs = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
					*(DWORD*)buff = (DWORD)cs << (n - 9);
				}
				res = RES_OK;
			}
			break;

		case GET_BLOCK_SIZE :    /* Get erase block size in unit of sector (DWORD) */
			*(DWORD*)buff = 128;
			res = RES_OK;
			break;

		default:
			res = RES_PARERR;
	}

	deselect();

	return res;
}

DWORD get_fattime(void)
{
	union {
		DWORD dw;
		struct {
			DWORD sec_div2:5, min: 6, hour:5, day:5, month:4, year:7;
		};
	} dt;
	/* Pack date and time into a DWORD variable */
	// 	struct t_rtc_datetime rtctime;
	t_rtc_datetime rtctime;
	rtc_get_time(&rtctime);
	dt.year = rtctime.year + (2000 - 1980);
	dt.month = rtctime.month;
	dt.day = rtctime.day;
	dt.hour = rtctime.hour;
	dt.min = rtctime.minute;
	dt.sec_div2 = rtctime.second >> 1;
	return dt.dw;	
}
