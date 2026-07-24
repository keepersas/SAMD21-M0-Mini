/*
 * sd.c
 *
 * Created: 20.07.2023 14:32:04
 *  Author: qzmn
 */ 

#include "sd.h"
#include "conf_sd.h"

static struct spi_module sd_spi_module;
static struct spi_config sd_spi_config;
struct t_spi sd_spi = { .module = &sd_spi_module, .hw = SD_SERCOM, .config = &sd_spi_config };

static FATFS m_fs;
static FRESULT m_rc;
static FIL m_fil;
static DIR m_dir;

TCHAR sd_filename[13] = {0};
TCHAR sd_dirname[9] = {0};
static bool m_is_open_write = false;
static bool sd_b_file_is_open_append = false;
static bool m_eof;
UINT m_br;

static bool sd_send_cmd(uint32_t cmd, uint32_t arg);
static bool sd_adtc_start(uint32_t cmd, uint32_t arg, uint16_t block_size, uint16_t nb_block, bool access_block);
static bool sd_mmc_spi_wait_busy(void);
static uint32_t sd_mmc_spi_response_32;
static bool sd_cmd8(uint8_t * v2);
uint32_t sd_get_response(void);

void sd_init_interface(void)
{
// 	struct port_config pc = {.direction = PORT_PIN_DIR_INPUT, .input_pull = PORT_PIN_PULL_UP, .powersave = false };
// 	port_pin_set_config(SD_SPI_MISO_PIN, &pc);

	PORT->Group[0].PINCFG[4].reg = 7;
	PORT->Group[0].PINCFG[5].reg = 7;
	PORT->Group[0].PINCFG[6].reg = 7;
	PORT->Group[0].PINCFG[7].reg = 7;
	PORT->Group[0].DIRCLR.reg = 0xF0;
	PORT->Group[0].OUTSET.reg = 0xF0;
	PORT->Group[0].PMUX[2].reg = 0x03;
	PORT->Group[0].PMUX[3].reg = 0x33;

	spi_get_config_defaults(sd_spi.config);
	sd_spi.config->mux_setting = SD_SPI_SERCOM_MUX_SETTING;
	sd_spi.config->pinmux_pad0 = SD_SPI_SERCOM_PINMUX_PAD0_MISO;
	// 	sd_spi.config.pinmux_pad1 = SD_MMC_SPI_PINMUX_PAD1;	// don't care 'cos MSSEN = 0
	sd_spi.config->pinmux_pad1 = SD_SPI_SERCOM_PINMUX_PAD1_CS;
	sd_spi.config->pinmux_pad2 = SD_SPI_SERCOM_PINMUX_PAD2_MOSI;
	sd_spi.config->pinmux_pad3 = SD_SPI_SERCOM_PINMUX_PAD3_SCK;
	sd_spi.config->transfer_mode = SD_SPI_TRANSFER_MODE;
	sd_spi.config->mode_specific.master.baudrate = SD_SPI_BAUDRATE;
	sd_spi.config->master_slave_select_enable = false;
	sd_spi.config->select_slave_low_detect_enable = false;
	my_spi_init(&sd_spi);
	// 	sd_set_baud(10000000UL);
// 	PORT->Group[SD_CS_PORTGROUP].PINCFG[5].bit.DRVSTR = 1;
// 	PORT->Group[SD_CS_PORTGROUP].PINCFG[5].bit.INEN = 0;
// 	PORT->Group[SD_CS_PORTGROUP].PINCFG[5].bit.PMUXEN = 0;
// 	PORT->Group[SD_CS_PORTGROUP].PINCFG[5].bit.PULLEN = 0;
	PORT->Group[SD_CS_PORTGROUP].DIRSET.reg = SD_CS_BIT;
	PORT->Group[SD_CS_PORTGROUP].PINCFG[5].reg = 0;
	SD_CS_SET();
	if (!sd_disk_initialize()) {
		nop();
	}
	SD_CS_SET();

// 	sd_enable();
// 	for (uint8_t i=0; i<10; i++) {
// 		sd_transmit(0xFF);
// 	}
// 	if (!sd_send_cmd(SDMMC_SPI_CMD0_GO_IDLE_STATE, 0)) {
// 		nop();
// 		return;
// 	}
// 	nop();
}

// sd_mmc_spi_card_init(void) from sd_mmc.c remake
bool sd_disk_initialize(void)
{
	sd_enable();
// 	CS_H();
	CS_L();
	for (uint8_t i=0; i<10; i++) {
		sd_transmit(0xFF);
	}
	// CMD0 - Reset all cards to idle state.
	if (!sd_send_cmd(SDMMC_SPI_CMD0_GO_IDLE_STATE, 0)) {
		return false;
	}
	uint8_t v2 = 0;
	if (!sd_cmd8(&v2)) {
		return false;
	}
	//qz No SDIO - always true
// 	// Try to get the SDIO card's operating condition
// 	if (!sdio_op_cond()) {
// 		return false;
// 	}

	nop();
	return true;
	
}

inline uint8_t sd_transmit(uint8_t tx)
{
	return my_spi_transmit(&sd_spi, tx);
}

void sd_read_frame(uint8_t *pBuffer, uint16_t size)
{
	while (size--) {
		*pBuffer++ = sd_transmit(0xFF);
	}
}

void sd_send_frame(uint8_t *pBuffer, uint16_t size)
{
	while (size--) {
		sd_transmit(*pBuffer++);
	}
}

void sd_dma_rx_512_bytes(BYTE *buff)
{
	sd_read_frame(buff, 512);
	# warning IMPLEMENT sd_dma_rx_512_bytes
}

void sd_dma_tx_512_bytes(BYTE *buff)
{
	sd_send_frame(buff, 512);
	# warning IMPLEMENT sd_dma_tx_512_bytes
}

inline void sd_set_baud(uint32_t baud)
{
	my_spi_set_baud(&sd_spi, baud);
}

inline void sd_set_fast_baud(void)
{
	sd_set_baud(SD_SPI_BAUDRATE_FAST);
}

inline void sd_enable(void)
{
	my_spi_enable(&sd_spi);
}

inline void sd_disable(void)
{
	my_spi_disable(&sd_spi);
}

bool sd_mount(void)
{
	return (m_rc = f_mount(&m_fs, "", 1)) == FR_OK;
// 	return (m_rc = f_mount(&m_fs, "", 0)) == FR_OK;
}

void sd_unmount(void)
{
	f_unmount("");
}

bool sd_open_read_file(void)
{
	return (m_rc = f_open(&m_fil, sd_filename, FA_READ)) == FR_OK;
}

bool sd_open_append_file(void)
{
	if ((m_rc = f_open(&m_fil, sd_filename, FA_OPEN_APPEND | FA_WRITE)) == FR_OK) {		
// 		if ((m_rc = f_expand(&m_fil, 512*50, 1)) == FR_OK) {
// 			m_is_open_write = true;
// 		} else {
// 			f_close(&m_fil);
// 		}
		//        UINT bw;
		//        if ((m_rc = f_write(&m_fil, &m_rc, 1, &bw)) == FR_OK) {
		//            if ((m_rc = f_lseek(&m_fil, f_size(&m_fil)-1)) == FR_OK) {
		//                m_rc = f_truncate(&m_fil);
		//            }
		//        }
	}
	return sd_b_file_is_open_append = m_rc == FR_OK;
}

bool sd_file_is_open_append(void)
{
	return sd_b_file_is_open_append;
}

bool sd_close_file(void)
{
	if (m_is_open_write) {
		f_truncate(&m_fil);
	}
	m_is_open_write = false;
	return (m_rc = f_close(&m_fil)) == FR_OK;
}

bool sd_read_file(uint8_t *buff, uint16_t btr)
{
	m_rc = f_read(&m_fil, buff, btr, &m_br);
	//     m_eof = f_eof(&m_fil);
	if (m_rc == FR_OK) {
		m_eof = m_br < btr;
	} else {
		m_eof = true;
		m_br = 0;
	}
	return m_rc == FR_OK;
}

bool sd_write_file(const void* buff, uint16_t btw, bool sync)
{
	UINT bw;
	m_rc = f_write(&m_fil, buff, btw, &bw);
	if ((m_rc == FR_OK) && sync) {
		m_rc = f_sync(&m_fil);
	}
	return (m_rc == FR_OK) && (btw == bw);
}

inline bool sd_eof(void)
{
	return m_eof;
}

inline uint16_t sd_bytes_read(void)
{
	return m_br;	
}

bool sd_create_dir(void)
{
	m_rc = f_mkdir(sd_dirname);
	return (m_rc == FR_OK) || (m_rc == FR_EXIST);
}

bool sd_change_dir(void)
{
	static TCHAR dir_arg[10] = "/12345678";
	strncpy(&dir_arg[1], sd_dirname, 9);
	return (m_rc = f_chdir(dir_arg)) == FR_OK;
}

static uint8_t sd_mmc_spi_crc7(uint8_t * buf, uint8_t size)
{
	uint8_t crc, value, i;

	crc = 0;
	while (size--) {
		value = *buf++;
		for (i = 0; i < 8; i++) {
			crc <<= 1;
			if ((value & 0x80) ^ (crc & 0x80)) {
				crc ^= 0x09;
			}
			value <<= 1;
		}
	}
	crc = (crc << 1) | 1;
	return crc;
}

uint32_t sd_get_response(void)
{
	return sd_mmc_spi_response_32;
}

static bool sd_cmd8(uint8_t * v2)
{
	uint32_t resp;

	*v2 = 0;
	// Test for SD version 2
	if (!sd_send_cmd(SD_CMD8_SEND_IF_COND, SD_CMD8_PATTERN | SD_CMD8_HIGH_VOLTAGE)) {
		return true; // It is not a V2
	}
	// Check R7 response
	resp = sd_get_response();
	if (resp == 0xFFFFFFFF) {
		// No compliance R7 value
		return true; // It is not a V2
	}
	if ((resp & (SD_CMD8_MASK_PATTERN | SD_CMD8_MASK_VOLTAGE)) != (SD_CMD8_PATTERN | SD_CMD8_HIGH_VOLTAGE)) {
// 		sd_mmc_debug("%s: CMD8 resp32 0x%08x UNUSABLE CARD\n\r", __func__, resp);
		return false;
	}
// 	sd_mmc_debug("SD card V2\n\r");
	*v2 = 1;
	return true;
}

static bool sd_send_cmd(uint32_t cmd, uint32_t arg)
{
	return sd_adtc_start(cmd, arg, 0, 0, false);
}

static bool sd_adtc_start(uint32_t cmd, uint32_t arg, uint16_t block_size, uint16_t nb_block, bool access_block)
{
	uint8_t dummy = 0xFF;
	uint8_t cmd_token[6];
	uint8_t ncr_timeout;
	uint8_t r1; //! R1 response
	uint16_t dummy2 = 0xFF;
uint8_t sd_mmc_spi_err;
#define SD_MMC_SPI_NO_ERR                 0 //! No error
#define SD_MMC_SPI_ERR                    1 //! General or an unknown error
#define SD_MMC_SPI_ERR_RESP_TIMEOUT       2 //! Timeout during command
#define SD_MMC_SPI_ERR_RESP_BUSY_TIMEOUT  3 //! Timeout on busy signal of R1B response
#define SD_MMC_SPI_ERR_READ_TIMEOUT       4 //! Timeout during read operation
#define SD_MMC_SPI_ERR_WRITE_TIMEOUT      5 //! Timeout during write operation
#define SD_MMC_SPI_ERR_RESP_CRC           6 //! Command CRC error
#define SD_MMC_SPI_ERR_READ_CRC           7 //! CRC error during read operation
#define SD_MMC_SPI_ERR_WRITE_CRC          8 //! CRC error during write operation
#define SD_MMC_SPI_ERR_ILLEGAL_COMMAND    9 //! Command not supported
#define SD_MMC_SPI_ERR_WRITE             10 //! Error during write operation
#define SD_MMC_SPI_ERR_OUT_OF_RANGE      11 //! Data access out of range

	UNUSED(access_block);
	Assert(cmd & SDMMC_RESP_PRESENT); // Always a response in SPI mode
	sd_mmc_spi_err = SD_MMC_SPI_NO_ERR;

	// Encode SPI command
	cmd_token[0] = SPI_CMD_ENCODE(SDMMC_CMD_GET_INDEX(cmd));
	cmd_token[1] = arg >> 24;
	cmd_token[2] = arg >> 16;
	cmd_token[3] = arg >> 8;
	cmd_token[4] = arg;
	cmd_token[5] = sd_mmc_spi_crc7(cmd_token, 5);

	// 8 cycles to respect Ncs timing
	// Note: This byte does not include start bit "0",
	// thus it is ignored by card.
// 	spi_write_buffer_wait(&sd_mmc_master, &dummy, 1);
	sd_transmit(dummy);
	// Send command
// 	spi_write_buffer_wait(&sd_mmc_master, cmd_token, sizeof(cmd_token));
	for (uint8_t i=0; i<6; i++) sd_transmit(cmd_token[i]);

	// Wait for response
	// Two retry will be done to manage the Ncr timing between command and reponse
	// Ncr: Min. 1x8 clock  cycle, Max. 8x8 clock cycles
	// WORKAROUND for no compliance card (Atmel Internal ref. SD13):
	r1 = 0xFF;
	// Ignore first byte because Ncr min. = 8 clock cylces
// 	spi_read_buffer_wait(&sd_mmc_master, &r1, 1, dummy2);
	r1 = sd_transmit(dummy2);
	ncr_timeout = 7;
	while (1) {
// 		spi_read_buffer_wait(&sd_mmc_master, &r1, 1, dummy2); // 8 cycles
		r1 = sd_transmit(dummy2);
		if ((r1 & R1_SPI_ERROR) == 0) {
			// Valid R1 response
			break;
		}
		if (--ncr_timeout == 0) {
			// Here Valid R1 response received
// 			sd_mmc_spi_debug("%s: cmd %02d, arg 0x%08lX, R1 timeout\n\r", __func__, (int)SDMMC_CMD_GET_INDEX(cmd), arg);
			sd_mmc_spi_err = SD_MMC_SPI_ERR_RESP_TIMEOUT;
			return false;
		}
	}

	// Save R1 (Specific to SPI interface) in 32 bit response
	// The R1_SPI_IDLE bit can be checked by high level
	sd_mmc_spi_response_32 = r1;

	// Manage error in R1
	if (r1 & R1_SPI_COM_CRC) {
// 		sd_mmc_spi_debug("%s: cmd %02d, arg 0x%08lx, r1 0x%02x, R1_SPI_COM_CRC\n\r", __func__, (int)SDMMC_CMD_GET_INDEX(cmd), arg, r1);
		sd_mmc_spi_err = SD_MMC_SPI_ERR_RESP_CRC;
		return false;
	}
	if (r1 & R1_SPI_ILLEGAL_COMMAND) {
// 		sd_mmc_spi_debug("%s: cmd %02d, arg 0x%08lx, r1 0x%x, R1 ILLEGAL_COMMAND\n\r", __func__, (int)SDMMC_CMD_GET_INDEX(cmd), arg, r1);
		sd_mmc_spi_err = SD_MMC_SPI_ERR_ILLEGAL_COMMAND;
		return false;
	}
	if (r1 & ~R1_SPI_IDLE) {
		// Other error
// 		sd_mmc_spi_debug("%s: cmd %02d, arg 0x%08lx, r1 0x%x, R1 error\n\r", __func__, (int)SDMMC_CMD_GET_INDEX(cmd), arg, r1);
		sd_mmc_spi_err = SD_MMC_SPI_ERR;
		return false;
	}

	// Manage other responses
	if (cmd & SDMMC_RESP_BUSY) {
		if (!sd_mmc_spi_wait_busy()) {
			sd_mmc_spi_err = SD_MMC_SPI_ERR_RESP_BUSY_TIMEOUT;
// 			sd_mmc_spi_debug("%s: cmd %02d, arg 0x%08lx, Busy signal always high\n\r", __func__, (int)SDMMC_CMD_GET_INDEX(cmd), arg);
			return false;
		}
	}
	if (cmd & SDMMC_RESP_8) {
// 		sd_mmc_spi_response_32 = 0;
// 		spi_read_buffer_wait(&sd_mmc_master, (uint8_t *)&sd_mmc_spi_response_32, 1, dummy2);
		sd_mmc_spi_response_32 = sd_transmit(dummy2);
		sd_mmc_spi_response_32 = le32_to_cpu(sd_mmc_spi_response_32);
	}
	if (cmd & SDMMC_RESP_32) {
// 		spi_read_buffer_wait(&sd_mmc_master, (uint8_t *)&sd_mmc_spi_response_32, 4, dummy2);
		union {uint8_t bytes[4]; uint32_t uint32;} r32;
		for (uint8_t i=0; i<4; i++) {
			r32.bytes[i] = sd_transmit(dummy2);
		}
		sd_mmc_spi_response_32 = r32.uint32;
		
		sd_mmc_spi_response_32 = be32_to_cpu(sd_mmc_spi_response_32);
	}

// 	sd_mmc_spi_block_size = block_size;
// 	sd_mmc_spi_nb_block = nb_block;
// 	sd_mmc_spi_transfert_pos = 0;
	return true; // Command complete
}

static bool sd_mmc_spi_wait_busy(void)
{
	uint8_t line = 0xFF;
	uint16_t dummy = 0xFF;

	/* Delay before check busy
	 * Nbr timing minimum = 8 cylces
	 */
// 	spi_read_buffer_wait(&sd_mmc_master, &line, 1, dummy);
	line = sd_transmit(dummy);

	/* Wait end of busy signal
	 * Nec timing: 0 to unlimited
	 * However a timeout is used.
	 * 200 000 * 8 cycles
	 */
	uint32_t nec_timeout = 200000;
// 	spi_read_buffer_wait(&sd_mmc_master, &line, 1, dummy);
	line = sd_transmit(dummy);
	do {
// 		spi_read_buffer_wait(&sd_mmc_master, &line, 1, dummy);
		line = sd_transmit(dummy);
		if (!(nec_timeout--)) {
			return false;
		}
	} while (line != 0xFF);
	return true;
}
