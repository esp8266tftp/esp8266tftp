/*
 * tpm2net.c
 *
 *  Created on: Nov 18, 2014
 *      Author: frans-willem
 */
#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "espconn.h"


static void ICACHE_FLASH_ATTR tftp_recv(void *arg, char *pusrdata, unsigned short length) {

	unsigned char *data =(unsigned char *)pusrdata;
	int count;

	unsigned short reply_length = 0;


	#ifdef PLATFORM_DEBUG
	ets_uart_printf("tftp_recv called. Length %d\r\n", length);
	#endif

	struct espconn *conn=(struct espconn *)arg;
	char buff[516];
	char reply[516];
	int i;
	char *rend=&reply[length];

	if (length>516) return;

	/*
	ets_uart_printf("tpm2net_recv data -> ");
	for (count= 0; count < length; count++) {
		ets_uart_printf("%02x ", data[count]);
	}
	ets_uart_printf("\r\n");
	*/
	memcpy(buff, data+4, length);

	do
	{
		reply_length = tftp_callback(data, length);
	}
	while (reply_length == 0);


	espconn_sent(conn, (uint8*)data, reply_length);

}

void tftp_init() {
	static struct espconn tftpconn;
	static esp_udp tftpudp;

	tftpconn.type = ESPCONN_UDP;
	tftpconn.state = ESPCONN_NONE;
	tftpconn.proto.udp = &tftpudp;
	tftpudp.local_port=69;
	tftpconn.reverse = NULL;
	espconn_regist_recvcb(&tftpconn, tftp_recv);
	espconn_create(&tftpconn);
	#ifdef PLATFORM_DEBUG
	ets_uart_printf("TFTP init done.\r\n");
	#endif
}
