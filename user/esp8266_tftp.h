/*
 * esp8266_tftp.h
 *
 *  Created on: 16 de sept. de 2015
 *      Author: MANUEL
 */

#ifndef USER_ESP8266_TFTP_H_
#define USER_ESP8266_TFTP_H_


#define TFTP_MAX_PACKAGE_SIZE						512+2+2

#define TFTP_STATE_INIT								0
#define	TFTP_STATE_INCOMING_CONNECTION				1
#define	TFTP_STATE_NEW_CONNECTION					2
#define TFTP_STATE_READ_REQUEST						3
#define TFTP_STATE_WRITE_REQUEST					4
#define TFTP_STATE_DATA								5
#define TFTP_STATE_ACK								6
#define TFTP_STATE_ERROR							7
#define TFTP_STATE_UNKNOWN							8


#define TFTP_OPCODE_RRQ				0x100
#define TFTP_OPCODE_WRQ				0x200
#define TFTP_OPCODE_DATA			0x300
#define TFTP_OPCODE_ACK				0x400
#define TFTP_OPCODE_ERROR			0x500


#define TFTP_ERROR_WRONG_BLOCKNUMBER	1


#define TFTP_PACKAGE_SIZE			512

#define TFTP_CONNECTION_NOT_VALID	0
#define TFTP_CONNECTION_VALID		1

#define _TFTP_DEBUG					1

struct _tftp_package_data {
                 uint16_t opcode;
				 uint16_t block_number;
				 uint8_t data[TFTP_PACKAGE_SIZE];
              };

typedef struct _tftp_package_data s_tftp_package_data;


struct _tftp_ackpackage {
                 uint16_t opcode;
				 uint16_t block_number;
              };

typedef struct _tftp_ackpackage s_tftp_ackpackage;

unsigned short tftp_callback(unsigned char *data, unsigned short length);

#endif /* USER_ESP8266_TFTP_H_ */
