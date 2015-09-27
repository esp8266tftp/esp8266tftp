/*------------------------------------------------------------------------------
**               F I L E       I N F O R M A T I O N
**------------------------------------------------------------------------------
** Project Name:  P3 ATTiny261 Solar Charge Controller B1 LOW
** $Id: esp8266_tftp.c 383 2015-09-25 13:48:37Z ivan $
** $LastChangedBy: ivan $
** $Date: 2015-09-25 08:48:37 -0500 (vie, 25 sep 2015) $
** $Rev: 383 $
** $HeadURL: https://redmine/svn/Software/ESP8266_TFTP/user/esp8266_tftp.c $
** Target system: ESP8266
** Compiler:   Eclipse
**
**------------------------------------------------------------------------------
**               C O P Y R I G H T     N O T I C E
**------------------------------------------------------------------------------
**                                                                            **
** This program is the confidential and proprietary product of Ivan J. Eis    **
** Any unauthorised use, reproduction or transfer of this                     **
** program is strictly prohibited.                                            **
**                                                                            **
** (c) Copyright 2014-2015 Nexus Sun Data E.I.R.L. (Subject to limited        **
** distribution and restricted disclosure only.) All rights reserved.         **
**                                                                            **
**------------------------------------------------------------------------------
**               A U T H O R   I D E N T I T Y
**------------------------------------------------------------------------------
** Initials    Name                     Company
** --------    ---------------------    ----------------------------------------
** ieis	       Ivan J. Eis	        	Nexus Sun Data E.I.R.L.
** manu	       Manuel Rodriguez	        Nexus Sun Data E.I.R.L.
**
**------------------------------------------------------------------------------
**               R E V I S I O N   H I S T O R Y
**------------------------------------------------------------------------------
** Version	Author	Task #	Task Synopsis (Change Description)
** -------+-------+-------+-----------------------------------------------------
** 1.0		manu				Initial Version
**------------------------------------------------------------------------------
**------------------------------------------------------------------------------*/

#include "c_types.h"
#include "esp8266_tftp.h"


uint8_t tftp_valid_connection = TFTP_CONNECTION_NOT_VALID;
uint8_t tftp_state = TFTP_STATE_INIT;



s_tftp_package_data * tftp_package_data;
s_tftp_ackpackage * tftp_ackpackage;

uint16_t tftp_blocknumber = 0;
uint16_t tftp_package_size = 0;
uint16_t tftp_ack_size = 0;


void tftp_senderror(unsigned char errorcode)
{
	tftp_package_data->opcode = TFTP_OPCODE_ERROR;
	tftp_package_data->block_number = (tftp_blocknumber>>8) | (tftp_blocknumber<<8);
	tftp_ack_size = 4;
}

void tftp_copy_data(void)
{
	/* copy data from memory and write it into eeprom */
	
}

/* send tftp ACK over UDP */
void tftp_send_package(void)
{
	#ifdef TFTP_DEBUG
	ets_uart_printf("tftp_send_package \r\n");
	#endif
}

void tftp_sendack(void)
{
	#ifdef TFTP_DEBUG
	ets_uart_printf("tftp_sendack\r\n");
	#endif

	/* send acknowledge here and tftp_blocknumber */
	tftp_package_data->opcode = TFTP_OPCODE_ACK;
	tftp_package_data->block_number = (tftp_blocknumber>>8) | (tftp_blocknumber<<8);

	tftp_ack_size = 4;
}

unsigned short tftp_callback(unsigned char *data, unsigned short length)
{
	tftp_package_data = (s_tftp_package_data *)data;

	switch (tftp_state) {

			case TFTP_STATE_INIT:
				tftp_state = TFTP_STATE_INCOMING_CONNECTION;
				tftp_package_size = length;
				tftp_ack_size = 0;

				#ifdef TFTP_DEBUG
				ets_uart_printf("tftp_state: %d tftp_package_size:%d\r\n", tftp_state, tftp_package_size);
				#endif

				break;

			case TFTP_STATE_INCOMING_CONNECTION:

				#ifdef TFTP_DEBUG
				ets_uart_printf("tftp_state: %x opcode: %d \r\n", tftp_state, tftp_package_data->opcode);
				#endif

				if (tftp_package_data->opcode == TFTP_OPCODE_WRQ)
				{
					/* here we received a WRQ  - goto WRQ TFTP_STATE_WRITE_REQUEST and send ACK */
					tftp_state = TFTP_STATE_WRITE_REQUEST;
				}
				else if (tftp_package_data->opcode == TFTP_OPCODE_RRQ)
				{
					/* here we received a WRQ  - goto RRQ state and send ACK */
					tftp_state = TFTP_STATE_READ_REQUEST;
				}
				else
				{
					tftp_state = TFTP_STATE_INIT;
				}	

				break;

			case TFTP_STATE_READ_REQUEST:
					tftp_ackpackage->opcode = TFTP_OPCODE_ACK;
					tftp_ackpackage->block_number = tftp_blocknumber;
					tftp_sendack();
				break;

			case TFTP_STATE_WRITE_REQUEST:

				#ifdef TFTP_DEBUG
				ets_uart_printf("tftp_state: %x opcode: %d tftp_blocknumber:%d tftp_package_size:%d\r\n",
								tftp_state, tftp_package_data->opcode, tftp_blocknumber, tftp_package_size);
				#endif

				if (tftp_package_data->opcode == TFTP_OPCODE_DATA)
				{
					/* make sure we are in WRQ mode and sent the first ACK upfront before receiving data*/
					if (tftp_blocknumber)
					{
						tftp_copy_data();				/* copy data to flash */
						tftp_sendack();
						tftp_blocknumber++;
					}
					else
					{
						/* something went terribly wrong here */
						tftp_senderror(TFTP_ERROR_WRONG_BLOCKNUMBER);
					}
					
					if (tftp_package_size < TFTP_MAX_PACKAGE_SIZE)
					{
						/* last package arrived < 512 bytes */
					
					}
				}
				else if (tftp_package_data->opcode == TFTP_OPCODE_WRQ)
				{
					tftp_blocknumber = 0;
					tftp_sendack();

					tftp_blocknumber++;

					
				}
				break;			
			
			case TFTP_STATE_ERROR:
				break;

			case TFTP_STATE_UNKNOWN:
				break;
			
			default:
				break;

	}


	return tftp_ack_size;
}


