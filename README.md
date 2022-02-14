## W25N01GV and W25M02GV FLASH driver for STM32

**This library works with the W25N01GV and W25M02GV FLASH memories using the STM32 HAL API for the SPI communication.**

**Some observations:**

- This library doesn't support SPI through interruption or DMA, only blocking mode.
- Column selection is not implemented and ECC is always enabled, thus the data for read/write must be always 2048-bytes long.
- SPI communication error handling is not implemented.

### How to use

\- Configure the HAL SPI peripheral with software CS (GPIO output);\
\- Configure the "User defines" section in `W25X0XGV.h` with SPI handle, CS pin and CS pin port;\
\- Include the `W25X0XGV.h` header;\
\- Call the `W25X0XGV_begin()` function;\
\- Call `W25X0XGV_block_erase()` to erase one block or `W25X0XGV_bulk_erase()` to erase the entire FLASH;\
\- Call `W25X0XGV_load_prog_data()` to load the user data to the internal FLASH buffer then `W25X0XGV_program_execute()` to execute the program;\
\- Call `W25X0XGV_page_data_read()` to load the FLASH data to the internal FLASH buffer then `W25X0XGV_read()` to copy the data to the user buffer.

**Example:**

```c
#include "W25X0XGV.h"
...

#define W25N01GV_STORAGE_PAGE_SIZ	W25X0XGV_MAX_COLUMN	// 2048 bytes per page
...

W25X0XGV_begin();

// Erase the block that holds the page 0
W25X0XGV_block_erase(0);

// Load the content of data_to_program to page 0 of the FLASH
uint8_t data_to_program[W25N01GV_STORAGE_PAGE_SIZ] = {0x01};

W25X0XGV_load_prog_data(data_to_program, W25N01GV_STORAGE_PAGE_SIZ);
W25X0XGV_program_execute(0);

// Load the content of page 0 to the data_to_read
uint8_t data_to_read[W25N01GV_STORAGE_PAGE_SIZ] = {0x00};

W25X0XGV_page_data_read(0);
W25X0XGV_read(data_to_read, W25N01GV_STORAGE_PAGE_SIZ);
```