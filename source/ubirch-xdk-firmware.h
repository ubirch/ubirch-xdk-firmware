/*
* Licensee agrees that the example code provided to Licensee has been developed and released by Bosch solely as an example to be used as a potential reference for Licensee�s application development. 
* Fitness and suitability of the example code for any use within Licensee�s applications need to be verified by Licensee on its own authority by taking appropriate state of the art actions and measures (e.g. by means of quality assurance measures).
* Licensee shall be responsible for conducting the development of its applications as well as integration of parts of the example code into such applications, taking into account the state of the art of technology and any statutory regulations and provisions applicable for such applications. Compliance with the functional system requirements and testing there of (including validation of information/data security aspects and functional safety) and release shall be solely incumbent upon Licensee. 
* For the avoidance of doubt, Licensee shall be responsible and fully liable for the applications and any distribution of such applications into the market.
* 
* 
* Redistribution and use in source and binary forms, with or without 
* modification, are permitted provided that the following conditions are 
* met:
* 
*     (1) Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer. 
* 
*     (2) Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in
*     the documentation and/or other materials provided with the
*     distribution.  
*     
*     (3)The name of the author may not be used to
*     endorse or promote products derived from this software without
*     specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR 
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
*  DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
*  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
*  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
*  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
*  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
*  POSSIBILITY OF SUCH DAMAGE.
*/
/*----------------------------------------------------------------------------*/
/**
* @file
* @brief  This Module is Configuration header for XDK Application Template configurations
*
**/

/* header definition ******************************************************** */
#ifndef XDK110_XDKAPPLICATIONTEMPLATE_H_
#define XDK110_XDKAPPLICATIONTEMPLATE_H_

#include "BCDS_Basics.h"

/* local interface declaration ********************************************** */
 /* Priorities */

#define TASK_PRIO_MAIN_CMD_PROCESSOR                (UINT32_C(1))
#define TASK_STACK_SIZE_MAIN_CMD_PROCESSOR          (UINT16_C(700))
#define TASK_Q_LEN_MAIN_CMD_PROCESSOR                (UINT32_C(10))

/* local type and macro definitions */

#define TIMER_AUTORELOAD_ON                         (UINT32_C(1))
#define TIMER_AUTORELOAD_OFF                        (UINT32_C(0))
#define BMA280_READ_FREQ_MS                         (UINT32_C(2000))


#define TASK_PRIORITY_SERVALPAL_CMD_PROC            UINT32_C(3)
#define TASK_STACK_SIZE_SERVALPAL_CMD_PROC          UINT32_C(600)
#define TASK_QUEUE_LEN_SERVALPAL_CMD_PROC           UINT32_C(10)

#define TASK_PRIO_HTTP_REQ							(UINT32_C(2))
#define TASK_STACK_SIZE_HTTP_REQ                    (UINT16_C(700))

//#define DESTINATION_SERVER_HOST     "192.168.150.114"
//#define DESTINATION_SERVER_PORT UINT16_C(8999)

#define WIFI_SSID                "ubirch"
#define WIFI_PASS                "H3ll0Ub1rch!"
#define DESTINATION_SERVER_HOST  "postman-echo.com"
#define DESTINATION_SERVER_PORT  UINT16_C(80)

#define DESTINATON_POST_PATH     "/post"


void appInitSystem(void * CmdProcessorHandle, uint32_t param2);

#endif /* XDK110_XDKAPPLICATIONTEMPLATE_H_ */

/** ************************************************************************* */
