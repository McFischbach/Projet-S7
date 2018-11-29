/*
 * GPS.c
 *
 *  Created on: 14 nov. 2018
 *      Author: gabin.fischbach
 */

#include "GPS.h"

void ParseurGPS(void* parameters){

	uint8_t bufIndex;
	uint8_t nmea_byte;

	uint8_t nmea_sentence[128];
	uint8_t sentence_length;

	uint8_t nmea_field[24][16];

	uint16_t i,j,k;

	uint8_t nmea_crc[2];
	uint8_t local_crc;
	uint8_t received_crc;

	uint8_t parseur_state = 0;
	uint16_t error_count = 0, previous_error_count = 0;

	uint8_t heures, minutes, secondes;

	uint8_t latitude_int, longitude_int;
	uint32_t latitude_dec, longitude_dec;

	uint8_t jour, mois, annee;

	//ENable DMA1
	NVIC_SetPriority(DMA1_Channel2_3_IRQn, 17);
	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

	// Take the semaphore once to make sure it's empty


	// Start GPS stream
	// codice di Youness

	while(1){

		// Parse incomming NMEA buffer
		// --------------------------

		bufIndex = 0;

		// Check if buffer index is not yet at half transfer size
		while(bufIndex < GPS_RX_HALFBUFFER_SIZE){

			//load the sentence in nmea_byte, then we will slice it
			nmea_byte = gps_rx_buffer[bufIndex + GPS_RX_HALFBUFFER_SIZE*dma_buffer_id];

			switch(parser_state){

			// if the state of the parser is 0, wait for the start of the sentence

			case 0:{

				if(nmea_byte = '$'){

					 // Start byte detected, initialization of the parser

					for (i=0; i<128; i++){

						nmea_sentence[i] = 0;

					}

					i = 0; // Go back to start value of variable
					local_CRC = 0; // Set the local CRC to 0
					parser_state = 1; // Go to next case

					break;
				}

			}

			// if the state of the parser is 1, that means the start of
			// the sentence is detected and we can start collecting data
			// from the gps buffer while the detection of *, the end of
			// the data, followed by checksum and Carriage Return Line Feed
			case 1: {

				if(nmea_byte != '*'){

					nmea_sentence[i] = nmea_byte;
					local_CRC ^= nmea_byte; // XOR
					++i;

					if(i>256){ // Message probably too long

						parser_state = 0;
						++error_count;

					}

				}
				else // This is the end of the sentence
				{

					sentence_length = i; // The length is where we stopped
					j = 0;
					parser_state = 2; // Go to the second case

				}
				break;

			}

			// We reached the end of the sentence, now we can check the CRC
			case 2: {

				if(nmea_byte == '\r' || nmea_byte == '\n'){

					//Conversion ASCII -> Hexa of the CRC
					if(nmea_CRC[0] >= 65)
						received_CRC = (nmea_CRC[0]-55)*16;
					else
						received_CRC = (nmea_CRC[0]-48)*16;
					if(nmea_CRC[1] >= 65)
						received_CRC = received_CRC + (nmea_CRC[1]-55);
					else
						received_CRC = received_CRC + (nmea_CRC[1]-48);

					//Compare the localCRC we calculated and the received one

					if(local_CRC == received_CRC){

						// Initilaize the field array

						for(j=0; j<24; j++){

							for(k=0; k<16; k++){

								nmea_field[j][k] = 0;

							}

						}

						parser_state = 3; // Go to next state

					}
					else
					{

						// There is an error between the CRC's
						parser_state = 0; // GO back to first case
						++error_count;

					}

				}
				break;

			}

			// Now that we checked the CRC's, let's parse the sentence !
			case 3: {

				j=0;
				k=0;

				// Fill the NMEA fields

				for(i=0; i<sentence_length; i++){ // Browse the table
					// we write 1 information by line, at each ',' make a line break

					if(nmea_sentence[i] != ','){ // k collums and j lines

						nmea_field[j][k] = nmea_sentence[i]; // writing characters from left to right in the table
						k++; // shift right

					}
					else
					{

						k = 0; // Go back to the begining (left)
						j++; // start a new line

					}


				}

				// Start $GPRMC Process
				if ((nmea_sentence[2] == 'R') && (nmea_sentence[3] == 'M') && (nmea_sentence[4] == 'C')){
				// beacause [0] = G and [1] = P
				// PUT THE PRINT HERE

					//  Decode the time which the measurement was made
					if((nmea_field[1][0] >= 0x30) && (nmea_field[1][0] <= 0x39)){ // if the hour starts with a number meaning it's the valid format

						heures = (nmea_field[1][0] - 48) * 10 + (nmea_field[1][1] - 48); // 0 -> 48, chiffre des dizaines puis chiffre des unités
						minutes = (nmea_field[1][2] - 48) * 10 + (nmea_field[1][3] - 48);
						secondes = (nmea_field[1][4] - 48) * 10 + (nmea_field[1][5] - 48);

					}
					else {

						heures = 0;
						minutes = 0;
						secondes = 0;

					}

					// Decode latitude and convert it to decimal
					if(nmea_field[3][0] != 0){

						//latitude_int = (nmea_field[3][0] - 48) * 10 + (nmea_field[3][1] - 48);

						latitude_dec = (nmea_field[3][0] - 48) * 1000 + (nmea_field[3][1] - 48) * 100 + (nmea_field[3][2] - 48) * 10 + (nmea_field[3][3] - 48) + (nmea_field[3][5] - 48) *  + (nmea_field[3][1] - 48);

					}
					else {

						latitude_dec = 0;
						latitude_int = 0;

					}
					if (nmea_field[5][0] != 0){

						latitude_int = 0;

						latitude_dec = 0;

					}
					else {

						latitude_dec = 0;
						latitude_int = 0;

					}

					// Decode Date
					if((nmea_field[9][0] >= 0x30) && (nmea_field[9][0] <= 0x39) && (nmea_field[9][1] >= 0x30) && (nmea_field[9][1] <= 0x39)){

						jour = (nmea_field[9][0] - 48) * 10 + (nmea_field[9][1] - 48);

						if((nmea_field[9][2] >= 0x30) && (nmea_field[9][2] <= 0x39) && (nmea_field[9][3] >= 0x30) && (nmea_field[9][3] <= 0x39)){

							mois = (nmea_field[9][2] - 48) * 10 + (nmea_field[9][3] - 48);

							if((nmea_field[9][4] >= 0x30) && (nmea_field[9][4] <= 0x39) && (nmea_field[9][5] >= 0x30) && (nmea_field[9][5] <= 0x39)){

								annee = 2000 + (nmea_field[9][4] - 48) * 10 + (nmea_field[9][5] - 48);

							}

						}

					}
					else {

						jour =0;
						mois = 0;
						annee = 0;

					}

					// Update global variables

					g_heures = 				(uint8_t)  heures;
					g_minutes = 			(uint8_t)  minutes;
					g_secondes = 			(uint8_t)  secondes;
					g_latitude_int =  	(uint8_t)  latitude_int;
					g_latitude_dec =  	(uint32_t) latitude_decc;
					g_longitude_int = 	(uint8_t)  longitude_int;
					g_longitude_dec = 	(uint32_t) longitude_dec;


			}

				// End of parsing
				parseur_state = 0;
				break;

			} // case

			default :
			{

				parseur_state = 0;

			}

			} // switch

			bufIndex++;

	} // while buffer < halfbuffer

} // while 1



} // function
