/* Includes */
#include "W25X0XGV.h"
#include "spi.h"

/* Private Variables */
static uint8_t W25X0XGV_model = 0x00;
static uint8_t W25M_die = 0x00;

/* Public Functions */
/**
  * @brief Initialises the memory
  */
uint8_t W25X0XGV_begin(void) {
	W25X0XGV_reset();

	uint8_t jedec_send[5] = { W25X0XGV_JEDEC_ID, 0x00, 0x00, 0x00, 0x00 };
	uint8_t jedec_ret[5] = { 0x00 };

	W25X0XGV_send_receive_data(jedec_send, jedec_ret, 6);

	if (jedec_ret[2] == WINBOND_MAN_ID) {
		uint16_t ret_ID = (uint16_t) (jedec_ret[3] << 8 | jedec_ret[4]);

		if (ret_ID == W25N01GV_DEV_ID) {
			W25X0XGV_set_status_reg(W25X0XGV_PROT_REG, 0x00);

			W25X0XGV_model = W25N01GV;

			return W25X0XGV_OK;
		} else if (ret_ID == W25M02GV_DEV_ID) {
			W25X0XGV_die_select(0);
			W25X0XGV_set_status_reg(W25X0XGV_PROT_REG, 0x00);
			W25X0XGV_die_select(1);
			W25X0XGV_set_status_reg(W25X0XGV_PROT_REG, 0x00);
			W25X0XGV_die_select(1);

			W25X0XGV_model = W25M02GV;

			return W25X0XGV_OK;
		}
	}

	return W25X0XGV_ERROR;
}

/**
  * @brief Erase one block of memory
  * @param page_add page address
  */
uint8_t W25X0XGV_block_erase(uint32_t page_add) {
	if (page_add > W25X0XGV_get_max_page()) return W25X0XGV_ERROR;

	if (W25X0XGV_model == W25M02GV) W25X0XGV_die_select_by_add(page_add);

	uint8_t page_high = (uint8_t) ((page_add & 0xFF00) >> 8);
	uint8_t page_low = (uint8_t) (page_add & 0xC0);

	uint8_t blk_er_send[4] = { W25X0XGV_BLOCK_ERASE, 0x00, page_high, page_low };

	W25X0XGV_block_wip();
	W25X0XGV_write_enable();
	W25X0XGV_send_data(blk_er_send, 4);
	W25X0XGV_block_wip();

	return W25X0XGV_OK;
}

/**
  * @brief Erase the entire memory
  */
uint8_t W25X0XGV_bulk_erase(void) {
	for (uint32_t i = 0; i < W25X0XGV_get_max_page(); i++) {
		if (W25X0XGV_block_erase(i) != 0) return W25X0XGV_ERROR;
	}

	return W25X0XGV_OK;
}

/**
  * @brief Load the data to the memory buffer
  * @param buf pointer to the data to be load
  * @param data_len data length (2048 bytes max)
  */
uint8_t W25X0XGV_load_prog_data(uint8_t *buf, uint32_t data_len) {
	if (data_len > W25X0XGV_MAX_COLUMN) return W25X0XGV_ERROR;

	uint8_t cmd_buf_send[3] = { W25X0XGV_PROG_DATA_LOAD, 0x00, 0x00 };

	W25X0XGV_block_wip();
	W25X0XGV_write_enable();
	HAL_GPIO_WritePin(W25N_CS_GPIO_PORT, W25N_CS_GPIO_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&W25N_SPI, cmd_buf_send, 3, 10);
	HAL_SPI_Transmit(&W25N_SPI, buf, data_len, 100);
	HAL_GPIO_WritePin(W25N_CS_GPIO_PORT, W25N_CS_GPIO_PIN, GPIO_PIN_SET);

	return W25X0XGV_OK;
}

/**
  * @brief Execute the memory program
  * @param page_add page address
  */
uint8_t W25X0XGV_program_execute(uint32_t page_add) {
	if (page_add > W25X0XGV_get_max_page()) return W25X0XGV_ERROR;

	if (W25X0XGV_model == W25M02GV) W25X0XGV_die_select_by_add(page_add);

	uint8_t page_high = (uint8_t) ((page_add & 0xFF00) >> 8);
	uint8_t page_low = (uint8_t) (page_add);

	uint8_t prog_exec_send[4] = { W25X0XGV_PROG_EXECUTE, 0x00, page_high, page_low };

	W25X0XGV_block_wip();
	W25X0XGV_write_enable();
	W25X0XGV_send_data(prog_exec_send, 4);
	W25X0XGV_block_wip();

	return W25X0XGV_OK;
}

/**
  * @brief Read a page into the internal memory buffer
  * @param page_add page address
  */
uint8_t W25X0XGV_page_data_read(uint32_t page_add) {
	if (page_add > W25X0XGV_get_max_page()) return W25X0XGV_ERROR;

	if (W25X0XGV_model == W25M02GV) W25X0XGV_die_select_by_add(page_add);

	uint8_t page_high = (uint8_t) ((page_add & 0xFF00) >> 8);
	uint8_t page_low = (uint8_t) (page_add);

	uint8_t data_read_send[4] = { W25X0XGV_PAGE_DATA_READ, 0x00, page_high, page_low };

	W25X0XGV_block_wip();
	W25X0XGV_send_data(data_read_send, 4);
	W25X0XGV_block_wip();

	return W25X0XGV_OK;
}

/**
  * @brief Read the page
  * @param buf pointer to the receiver buffer
  * @param data_len data length (2048 bytes max)
  */
uint8_t W25X0XGV_read(uint8_t *buf, uint32_t data_len) {
	if (data_len > W25X0XGV_MAX_COLUMN) return W25X0XGV_ERROR;

	uint8_t cmd_buf_send[4] = { W25X0XGV_READ, 0x00, 0x00, 0x00 };

	W25X0XGV_block_wip();
	HAL_GPIO_WritePin(W25N_CS_GPIO_PORT, W25N_CS_GPIO_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&W25N_SPI, cmd_buf_send, 4, 10);
	HAL_SPI_Receive(&W25N_SPI, buf, data_len, 100);
	HAL_GPIO_WritePin(W25N_CS_GPIO_PORT, W25N_CS_GPIO_PIN, GPIO_PIN_SET);

	return W25X0XGV_OK;
}

/* Private Functions */
/**
  * @brief Send and receive data through SPI
  * @param send_buf pointer to the data to be sent
  * @param ret_buf pointer to the receiver buffer
  * @param len data length
  */
void W25X0XGV_send_receive_data(uint8_t *send_buf, uint8_t *ret_buf, uint32_t len) {
	HAL_GPIO_WritePin(W25N_CS_GPIO_PORT, W25N_CS_GPIO_PIN, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&W25N_SPI, send_buf, ret_buf, len, 10);
	HAL_GPIO_WritePin(W25N_CS_GPIO_PORT, W25N_CS_GPIO_PIN, GPIO_PIN_SET);
}

/**
  * @brief Send data through SPI
  * @param send_buf pointer to the data to be sent
  * @param len data length
  */
void W25X0XGV_send_data(uint8_t *send_buf, uint32_t len) {
	HAL_GPIO_WritePin(W25N_CS_GPIO_PORT, W25N_CS_GPIO_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&W25N_SPI, send_buf, len, 10);
	HAL_GPIO_WritePin(W25N_CS_GPIO_PORT, W25N_CS_GPIO_PIN, GPIO_PIN_SET);
}

/**
  * @brief Reset the memory
  */
void W25X0XGV_reset(void) {
	uint8_t sbuf[1] = { W25X0XGV_RESET };

	W25X0XGV_block_wip();
	W25X0XGV_send_data(sbuf, 1);
}

/**
  * @brief Set the memory status register
  * @param reg register address
  * @param set register data
  */
void W25X0XGV_set_status_reg(uint8_t reg, uint8_t set) {
	uint8_t stat_send[3] = { W25X0XGV_WRITE_STATUS_REG, reg, set };

	W25X0XGV_send_data(stat_send, 3);
}

/**
  * @brief Get the maximum number of pages
  */
uint32_t W25X0XGV_get_max_page(void) {
	if (W25X0XGV_model == W25N01GV) return W25N01GV_MAX_PAGE;
	else if (W25X0XGV_model == W25M02GV) return W25M02GV_MAX_PAGE;

	return 0;
}

/**
  * @brief Wait for the memory to be ready
  */
uint8_t W25X0XGV_block_wip(void) {
	while (W25X0XGV_check_wip());

	return W25X0XGV_OK;
}

/**
  * @brief Check if the memory is ready
  */
uint8_t W25X0XGV_check_wip(void) {
	uint8_t status = W25X0XGV_get_status_reg(W25X0XGV_STAT_REG);

	if (status & 0x01) return W25X0XGV_ERROR;

	return W25X0XGV_OK;
}

/**
  * @brief Get the memory status register
  * @param reg register address
  */
uint8_t W25X0XGV_get_status_reg(uint8_t reg) {
	uint8_t stat_send[3] = { W25X0XGV_READ_STATUS_REG, reg, 0x00 };
	uint8_t stat_ret[3] = { 0x00 };

	W25X0XGV_send_receive_data(stat_send, stat_ret, 4);

	return stat_ret[2];
}

/**
  * @brief Enable the memory to be written
  */
void W25X0XGV_write_enable(void) {
	uint8_t write_en_send[1] = { W25X0XGV_WRITE_ENABLE };

	W25X0XGV_send_data(write_en_send, 1);
}

/**
  * @brief Select the memory die (W25M only)
  * @param die die to be used
  */
void W25X0XGV_die_select(uint8_t die) {
	uint8_t die_select_buf[2] = { W25M_DIE_SELECT, die };

	W25X0XGV_send_data(die_select_buf, 2);

	W25M_die = die;
}

/**
  * @brief Select the memory die by page address (W25M only)
  * @param page_add page address
  */
uint8_t W25X0XGV_die_select_by_add(uint8_t page_add) {
	if (page_add > W25X0XGV_get_max_page()) return W25X0XGV_ERROR;

	W25X0XGV_die_select(page_add / W25N01GV_MAX_PAGE);

	return W25X0XGV_OK;
}
