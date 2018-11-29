/*
 * DMA.c
 *
 *  Created on: 17 oct. 2018
 *      Author: gabin.fischbach
 */

#include "stm32l0xx.h"

void Config_Init_DMA(){



}

void Receptition_Using_DMA(){


	// Setting DMA mode for reception
	LPUART1->CR3 &= ~(USART_CR3_DMAR_Msk);
	LPUART1->CR3 |= (0x01 << USART_CR3_DMAR_Pos);

	// Set the adress of the source of the transfer in the DMA
	DMA1->CPAR1 |= 0x24;
	USART_

	//


}

void DMA_Channel_Config(){

	//Enable peripheral clock on DMA
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

	//Enable DMA transfer on LPUART
	LPUART1->CR3 &= ~(USART_CR3_DMAR_Msk);
	LPUART1->CR3 |= (0x01 << USART_CR3_DMAR_Pos);

	//COnfigure the peripheral data register adress
	DMA1_Channel1->CPAR =

}
