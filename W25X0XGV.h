#ifndef INC_W25X0XGV_H_
#define INC_W25X0XGV_H_

/* Includes */
#include "stdint.h"

/* User defines */
#define W25N_SPI					hspi1				// SPI Handle
#define W25N_CS_GPIO_PORT			SPI1_CS_GPIO_Port	// SPI CS Port
#define W25N_CS_GPIO_PIN			SPI1_CS_Pin			// SPI CS Pin

/* Defines */
#define W25M_DIE_SELECT				0xC2

#define W25X0XGV_WRITE_STATUS_REG	0x01
#define W25X0XGV_PROG_DATA_LOAD		0x02
#define W25X0XGV_READ				0x03
#define W25X0XGV_READ_STATUS_REG	0x05
#define W25X0XGV_WRITE_ENABLE		0x06
#define W25X0XGV_PROG_EXECUTE		0x10
#define W25X0XGV_PAGE_DATA_READ		0x13
#define W25X0XGV_JEDEC_ID			0x9F
#define W25X0XGV_PROT_REG			0xA0
#define W25X0XGV_STAT_REG			0xC0
#define W25X0XGV_BLOCK_ERASE		0xD8
#define W25X0XGV_RESET				0xFF

#define WINBOND_MAN_ID				0xEF
#define W25N01GV_DEV_ID				0xAA21
#define W25M02GV_DEV_ID				0xAB21

#define W25N01GV_MAX_PAGE			65535
#define W25X0XGV_MAX_COLUMN			2048
#define W25M02GV_MAX_PAGE			131071

/* Enums */
typedef enum {
	W25X0XGV_OK			= 0,
	W25X0XGV_ERROR		= 1,
} W25N_STATUS;

enum chip_models {
	W25N01GV,
	W25M02GV
};

/* Public Functions */
uint8_t W25X0XGV_begin(void);

uint8_t W25X0XGV_block_erase(uint32_t page_add);
uint8_t W25X0XGV_bulk_erase(void);

uint8_t W25X0XGV_load_prog_data(uint8_t *buf, uint32_t data_len);
uint8_t W25X0XGV_program_execute(uint32_t page_add);

uint8_t W25X0XGV_page_data_read(uint32_t page_add);
uint8_t W25X0XGV_read(uint8_t *buf, uint32_t data_len);

/* Private Functions */
void W25X0XGV_send_receive_data(uint8_t *send_buf, uint8_t *ret_buf, uint32_t len);
void W25X0XGV_send_data(uint8_t *send_buf, uint32_t len);
void W25X0XGV_reset(void);
void W25X0XGV_set_status_reg(uint8_t reg, uint8_t set);
uint32_t W25X0XGV_get_max_page(void);
uint8_t W25X0XGV_block_wip(void);
uint8_t W25X0XGV_check_wip(void);
uint8_t W25X0XGV_get_status_reg(uint8_t reg);
void W25X0XGV_write_enable(void);
void W25X0XGV_die_select(uint8_t die);
uint8_t W25X0XGV_die_select_by_add(uint8_t page_add);

#endif /* INC_W25X0XGV_H_ */
