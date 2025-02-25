/*
 *  Created on: Jan 18, 2022
 *      Author: Joe Krachey
 */

#ifndef SPI_H__
#define SPI_H__

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

#undef CONN_SPI_J300
#define CONN_SPI_J301
#undef CONN_SPI_J302

#if defined(CONN_SPI_J300)
#define PIN_MCU_SPI_MOSI	P9_0
#define PIN_MCU_SPI_MISO	P9_1
#define PIN_MCU_SPI_CLK		P9_2
#elif defined(CONN_SPI_J301)
#define PIN_MCU_SPI_MOSI	P10_0
#define PIN_MCU_SPI_MISO	P10_1
#define PIN_MCU_SPI_CLK		P10_2
#elif defined(CONN_SPI_J302)
#define PIN_MCU_SPI_MOSI	P5_0
#define PIN_MCU_SPI_MISO	P5_1
#define PIN_MCU_SPI_CLK		P5_2
#else
#error "MUST DEFINE A SPI CONNECTOR"
#endif
/* Macros */

#define SPI_FREQ			1000000

/* Public Global Variables */
extern cyhal_spi_t mSPI;



/* Public API */
cy_rslt_t spi_init(void);

#endif 
