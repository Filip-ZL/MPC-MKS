/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */


#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
#include <stdio.h>
#include <string.h>

#define TELNET_THREAD_PRIO  ( tskIDLE_PRIORITY + 4 )
#define CMD_BUFFER_LEN	256
static void telnet_process_command(char *cmd, struct netconn *conn);
static void telnet_byte_available(uint8_t c, struct netconn *conn);

/*-----------------------------------------------------------------------------------*/

static void telnet_thread(void *arg)
{
  struct netconn *conn, *newconn;
  err_t err, accept_err;
  struct netbuf *buf;
  uint8_t *data;
  u16_t len;
      
  LWIP_UNUSED_ARG(arg);

  /* Create a new connection identifier. */
  conn = netconn_new(NETCONN_TCP);
  
  if (conn!=NULL)
  {  
    /* Bind connection to well known port number 7. */
    err = netconn_bind(conn, NULL, 23);
    
    if (err == ERR_OK)
    {
      /* Tell connection to go into listening mode. */
      netconn_listen(conn);
    
      while (1) 
      {
        /* Grab new connection. */
         accept_err = netconn_accept(conn, &newconn);
    
        /* Process the new connection. */
        if (accept_err == ERR_OK) 
        {

          while (netconn_recv(newconn, &buf) == ERR_OK) 
          {
            do 
            {
              netbuf_data(buf, (void**)&data, &len);
              while (len--) telnet_byte_available(*data++, newconn);
          
            } 
            while (netbuf_next(buf) >= 0);
          
            netbuf_delete(buf);
          }
        
          /* Close connection and discard connection identifier. */
          netconn_close(newconn);
          netconn_delete(newconn);
        }
      }
    }
    else
    {
      netconn_delete(newconn);
    }
  }
}
static void telnet_byte_available(uint8_t c, struct netconn *conn)
{
	static uint16_t cnt;
	static char data[CMD_BUFFER_LEN];
	if (cnt < CMD_BUFFER_LEN && c >= 32 && c <= 127) data[cnt++] = c;
	if (c == '\n' || c == '\r') {
		data[cnt] = '\0';
		telnet_process_command(data, conn);
		cnt = 0;
	}
}
static void telnet_process_command(char *cmd, struct netconn *conn){
	char s[50];
	char *token;
	char *saveptr1;
	token = strtok_r(cmd, " ", &saveptr1);
	// Parsing commands sent remotely
	if(strcasecmp(token, "HELLO") == 0){
		sprintf(s, "Communication OK \n");
		netconn_write(conn, s, strlen(s), NETCONN_COPY);
	}
	else if(strcasecmp(token, "LED1") == 0){
		token = strtok_r(NULL, " ", &saveptr1);
		if(strcasecmp(token, "ON") == 0){
			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 1);
		}
		else if(strcasecmp(token, "OFF") == 0){
			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, 0);
		}
		sprintf(s, "OK \n");
		netconn_write(conn, s, strlen(s), NETCONN_COPY);
	}
	else if(strcasecmp(token, "LED2") == 0){
		token = strtok_r(NULL, " ", &saveptr1);
		if(strcasecmp(token, "ON") == 0){
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
		}
		else if(strcasecmp(token, "OFF") == 0){
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);
		}
		sprintf(s, "OK \n");
		netconn_write(conn, s, strlen(s), NETCONN_COPY);
	}
	else if(strcasecmp(token, "LED3") == 0){
		token = strtok_r(NULL, " ", &saveptr1);
		if(strcasecmp(token, "ON") == 0){
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 1);
		}
		else if(strcasecmp(token, "OFF") == 0){
			HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, 0);
		}
		sprintf(s, "OK \n");
		netconn_write(conn, s, strlen(s), NETCONN_COPY);
	}
	else if(strcasecmp(token, "STATUS") == 0){
		// Figuring status of led's
		char *led1 = HAL_GPIO_ReadPin(LD1_GPIO_Port, LD1_Pin)? "ON" : "OFF";
		char *led2 = HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin)? "ON" : "OFF";
		char *led3 = HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin)? "ON" : "OFF";
		sprintf(s,"Led 1 is %s, Led 2 is %s, led 3 is %s \n", led1, led2, led3);
		netconn_write(conn, s, strlen(s), NETCONN_COPY);
	}
}
/*-----------------------------------------------------------------------------------*/

void telnet_init(void)
{
  sys_thread_new("telnet_thread", telnet_thread, NULL, DEFAULT_THREAD_STACKSIZE, TELNET_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/

#endif /* LWIP_NETCONN */
