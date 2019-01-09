/*
 * EEPROM.c
 *
 *  Created on: 12 déc. 2018
 *      Author: gabin.fischbach
 */

#include "EEPROM.h"
#define EEPROM_ADRESS 0xA0

void EEPROM_I2C_Init(){

	// Pin configuration
	// PB6 -> SCL
	// PB7 -> SDA

	//Disable I2C to be safe
	I2C1->CR1 &= ~(I2C_CR1_PE);

	// Enable GPIOB clock
	RCC->IOPENR |= RCC_IOPENR_IOPBEN;

	// Enable I2C1 clock
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	// Configure PB6 & PB7 as AF mode
	GPIOB->MODER &= ~(GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	GPIOB->MODER |= (0x02 << GPIO_MODER_MODE6_Pos) | (0x02 << GPIO_MODER_MODE7_Pos);

	//Connect to I2C1 to (AF1)
	GPIOB->AFRL & ~(0xFF000000);
	GPIOB->AFRL |= 0x11000000;

	// Setup open-drain
	GPIOB->OTYPER |= GPIO_OTYPER_OT_7 | GPIO_OTYPER_OT_6;

	//Address of the EEPROM, 7 bits address
	I2C1->CR2 &= ~(I2C_CR2_ADD10_Msk);
	I2C1->CR2 |= (EEPROM_ADRESS << 1U);

	//Set the number of bytes transfered by default
	I2C1->CR2 |= (9 << I2C_CR2_NBYTES_Pos);

	// Setup pull-up on both SCL and SDA
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD6);
	GPIOB->PUPDR |= (0x01 << GPIO_PUPDR_PUPD7_Pos) | (0x01 << GPIO_PUPDR_PUPD6_Pos);

	// Reset the registers
	I2C1->CR1 = 0x00000000;
	I2C1->CR2 = 0x00000000;

	// Setup High/Low level to 5µs (100kHz)
	// -> 200 periods of main clock
	I2C1->TIMINGR |= 0x00707CBB; // valeur proposée par défaut par CubeMX

	// Enable I2C1
	I2C1->CR1 |= I2C_CR1_PE;
}

void EEPROM_Initialization (){

	//Disable I2C to be safe
	I2C1->CR1 &= ~(I2C_CR1_PE);

	// Disable noise filter
	I2C1->CR1 &= ~(I2C_CR1_ANFOFF);
	I2C1->CR1 |= (0x0000 << I2C_CR1_ANFOFF_Pos);

	// Configure the prescaler
	I2C1->TIMINGR &= ~(I2C_TIMINGR_PRESC_Msk);
	//I2C1->TIMINGR

	// Setup APB1 frequency, by setting the prescaler to 0,
	// we want the APB clock straight to the I2C clock
	I2C1->TIMINGR &= ~(I2C_TIMINGR_PRESC_Msk);



}

uint8_t EEPROM_Transfer(uint8_t position, uint8_t *buffer_of_data){

	// Buffer of 8 data, each of 1 byte in configuration
	// jour, mois, annee, heure, minutes, secondes, lat, lon

	// Create a time out to secure transfer data
	uint32_t timout;
	timout = 100000;

	// Set AUTOEND to 0 to be safe
	I2C1->CR2 &= ~(I2C_CR2_AUTOEND_Msk);

	// Set transfer direction from STM to memory
	I2C1->CR2 &= ~(I2C_CR2_RD_WRN_Msk);

	// 9 bytes to be transfered : 1 address + 8 data
	I2C1->CR2 &= ~(I2C_CR2_NBYTES_Msk);
	I2C1->CR2 |= (9 << I2C_CR2_NBYTES_Pos);

	// Start transaction
	I2C1->CR2 |= I2C_CR2_START;

	// Loop to verify the state of NACK flag
	// We want to check acknowledge from the EEPROM
	// So we wait until NACK is low or timout is down
	while((I2C1->ISR & I2C_ISR_NACKF) == I2C_ISR_NACKF && (timout > 0)){

		--timout;

	}
	if(timout == 0) return 1;

	// Loop to verify I2C transmit register is empty
	// and ready to contain value
	while((I2C1->ISR & I2C_ISR_TXIS) != I2C_ISR_TXIS && (timout > 0)){

		--timout;

	}
	if(timout == 0) return 2;

	// Tell where to store the data we send
	I2C1->TXDR = EEPROM_ADRESS;

	// Fill the buffer with data to be sent, byte by byte
	uint8_t n;
	for(n = 0; n < 9; n++){

		timout = 100000;
		// TXIS should be = 1 beacause TXDR waiting for a byte
		while(((I2C1->ISR & I2C_ISR_TXIS) != I2C_ISR_TXIS) && (timout > 0)){

			--timout;

		}
		if(timout == 0) return 2;

		I2C1->TXDR = buffer_of_data[n];

	}

	// Loop wait here until STOP flag is high or timout is down
	while((I2C1->ISR & I2C_ISR_STOPF) != I2C_ISR_STOPF && (timout > 0)){

		--timout;

	}
	if(timout == 0) return 3;

	// Disable I2C1
	I2C1->CR1 &= ~(I2C_CR1_PE);

	// A ajouter peut-être : les sous-registres de la mémoire
	// de la EEPROM pour stocker à la suite et ne pas réécrire au même endroit

}

/******************************************
 * I2C EEPROM (AT24C512) Byte Write
 *****************************************/
	//I2CStart();
	/* Slave address */
	//I2CSend(0xA0);
	/* 2-Byte Sub Address */
	//I2CSend(0x00);
	//I2CSend(0x00);
	/* Data */
	//I2CSend(0x55);
	//I2CStop();

