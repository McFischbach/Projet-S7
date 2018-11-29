/*
 * bsp.c
 *
 *  Created on: 10 oct. 2018
 *      Author: gabin.fischbach
 */

/*
 * BSP_Console_Init()
 * LPUART1 @ 115200 Full Duplex
 * 1 start - 8-bit - 1 stop
 * TX -> PA2 (AF6)
 * RX -> PA3 (AF6)
 */

#include "stm32l0xx.h"

void BSP_Console_Init()
{
	// Enable GPIOA Clock
	RCC->IOPENR |= RCC_IOPENR_IOPAEN;

	// Configure PA2 & PA3 as Alternate Function
	GPIOA->MODER &= ~(GPIO_MODER_MODE2_Msk | GPIO_MODER_MODE3_Msk);
	GPIOA->MODER |= (0x02 <<GPIO_MODER_MODE2_Pos) | (0x02 <<GPIO_MODER_MODE3_Pos);

	// Set PA2 & PA3 to AF6 (LPUART1)
	GPIOA->AFR[0] &= ~(0x0000FF00);
	GPIOA->AFR[0] |=  (0x00001100);

	// Enable LPUART1 clock
	RCC->APB1ENR |= RCC_APB1ENR_LPUART1EN;

	// Clear LPUART1 configuration (reset state)
	// 8bits, 1 start, 1 stop, CTS/RTS disabled
	LPUART1->CR1 = 0x00000000;
	LPUART1->CR2 = 0x00000000;
	LPUART1->CR3 = 0x00000000;

	// Select PCLK (APB1) as clock source
	// PCLK -> 65kHz
	RCC->APB1ENR |= RCC_APB1ENR_LPUART1EN;

	// Baud rate = 9600 bauds
	// Fclk = 65531 kHz, LPUARTDIV = 65531/9600 = 6.82614
	// BRR = 7 -> Baud rate = 65531/7 = 9361.5714 = 2.5% error
	LPUART1->BRR = 0x369;

	// Enable both transmitter & receiver
	LPUART1->CR1 |= USART_CR1_RE | USART_CR1_TE;

	// Enable LPUART1
	LPUART1->CR1 |= USART_CR1_UE;

}


void BSP_NVIC_Init()
{

	// Set maximum priority for LPUART1
	NVIC_SetPriority(LPUART1_IRQn, 0);

	// Enable LPUART1 interrupts
	NVIC_EnableIRQ(LPUART1_IRQn);

}
