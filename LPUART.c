/*
 * ReceiveCharacter.c
 *
 *  Created on: 10 oct. 2018
 *      Author: gabin.fischbach
 */

#include "stm32l0xx.h"


void Receive_Char_Config_LPUART(){

	//The default setting is 9600bps, 8 bits, no parity bit, 1 stop bit. (GPS doc)

	//Disable LPUART in order to set the registers
	LPUART1->CR1 &= ~(USART_CR1_UE_Msk);

	//Define word length
	LPUART1->CR1 &= ~(USART_CR1_M1_Msk);
	LPUART1->CR1 |= (0x00 << USART_CR1_M1_Pos);

	LPUART1->CR1 &= ~(USART_CR1_M0_Msk);
	LPUART1->CR1 |= (0x00 << USART_CR1_M0_Pos);


	//No parity bit
	LPUART1->CR1 &= ~(USART_CR1_PCE_Msk);
	LPUART1->CR1 |= (0x00 << USART_CR1_PCE_Pos);

	// Baud rate = 9600 bauds
	// Fclk = 65531 kHz, LPUARTDIV = 65531/9600 = 6.82614
	// BRR = 7 -> Baud rate = 65531/7 = 9361.5714 = 2.5% error
	LPUART1->BRR |= 0x369;

	// Set the number of stop bit at 1
	LPUART1->CR2 &= ~(USART_CR2_STOP_Msk);
	LPUART1->CR2 |= (0x00 << USART_CR2_STOP_Pos);

	//Enable the LPUART
	LPUART1->CR1 |= USART_CR1_UE;

	//Set the RE bit to enable the receiver searching for a start bit
	LPUART1->CR1 |= USART_CR1_RE;

	//Use PA3 to receive
}

void Transmit_Char_Config_LPUART(){

	//Disable LPUART in order to set the registers
	LPUART1->CR1 &= ~(USART_CR1_UE_Msk);

	//Define word length
	LPUART1->CR1 &= ~(USART_CR1_M1_Msk);
	LPUART1->CR1 |= (0x01 << USART_CR1_M1_Pos);

	LPUART1->CR1 &= ~(USART_CR1_M0_Msk);
	LPUART1->CR1 |= (0x01 << USART_CR1_M0_Pos);

	// Baud rate = 9600 bauds
	// Fclk = 65531 kHz, LPUARTDIV = 65531/9600 = 6.82614
	// BRR = 7 -> Baud rate = 65531/7 = 9361.5714 = 2.5% error
	LPUART1->BRR |= 0x369;

	// Set the number of stop bit at 2
	LPUART1->CR2 &= ~(USART_CR2_STOP_Msk);
	LPUART1->CR2 |= (0x10 << USART_CR2_STOP_Pos);

	//Use data from the DMA

	//Enable the LPUART
	LPUART1->CR1 |= USART_CR1_UE;

	//Set the TE bit to enable the transmitter to
	//Send an idle frame for 1st transmit
	LPUART1->CR1 |= USART_CR1_TE;

	// Use PA2 to transmit


}


















