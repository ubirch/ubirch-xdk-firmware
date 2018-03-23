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
* @ingroup APPS_LIST
*
* @defgroup XDK_APPLICATION_TEMPLATE XDK Application Template
* @{
*
* @brief XDK Application Template
*
* @details Empty XDK Application Template without any functionality. Should be used as a template to start new projects.
*
* @file
**/


/* module includes ********************************************************** */

#include "ubirch-xdk-firmware.h"
#include "BMA280.h"
#include "SntpTime.h"
#include "wlan.h"
#include "ubirch-proto.h"

/* system header files */
#include <stdio.h>
#include <BCDS_Basics.h>
/* additional interface header files */

/* own header files */
#include "BSP_BoardType.h"
#include "BCDS_CmdProcessor.h"
#include "BCDS_Assert.h"
#include "Serval_Sntp.h"
#include <Serval_Network.h>
#include "BCDS_ServalPal.h"
#include "BCDS_ServalPalWiFi.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "ubirch_protocol.h"


/* constant definitions ***************************************************** */

/* local variables ********************************************************** */
#define SECONDS(x) ((portTickType) (x * 1000) / portTICK_RATE_MS)
#define MILI_SECONDS(x) ((portTickType) (x) / portTICK_RATE_MS)

static CmdProcessor_T CommandProcessorHandle;

//xTaskHandle bma280TaskHandle = NULL;
xTaskHandle ledTaskHandle = NULL;

/* global variables ********************************************************* */

/* inline functions ********************************************************* */

/* local functions ********************************************************** */

// Initialize ServalPal to init TCP/IP, UDP, HTTP communication via WIFI
static Retcode_T ServalPalSetup(void)
{
    Retcode_T returnValue = RETCODE_OK;
    returnValue = CmdProcessor_Initialize(&CommandProcessorHandle, "Serval PAL", TASK_PRIORITY_SERVALPAL_CMD_PROC, TASK_STACK_SIZE_SERVALPAL_CMD_PROC, TASK_QUEUE_LEN_SERVALPAL_CMD_PROC);
    /* serval pal common init */
    if (RETCODE_OK == returnValue)
    {
        returnValue = ServalPal_Initialize(&CommandProcessorHandle);
    }
    if (RETCODE_OK == returnValue)
    {
        returnValue = ServalPalWiFi_Init();
    }
    if (RETCODE_OK == returnValue)
    {
        ServalPalWiFi_StateChangeInfo_T stateChangeInfo = { SERVALPALWIFI_OPEN, INT16_C(0) };
        returnValue = ServalPalWiFi_NotifyWiFiEvent(SERVALPALWIFI_STATE_CHANGE, &stateChangeInfo);
    }
    return returnValue;
}

static void toggleLED(void *pvParameters)
{
	Retcode_T retVal = RETCODE_OK;
	BCDS_UNUSED(pvParameters);


	for (;;)
	{
		retVal = BSP_LED_Switch((uint32_t) BSP_XDK_LED_Y, (uint32_t) BSP_LED_COMMAND_TOGGLE);
		if (RETCODE_OK != retVal)
		{
			printf("Failed to toggle led\r\n");
		}
		vTaskDelay(MILI_SECONDS(600));

		retVal = BSP_LED_Switch((uint32_t) BSP_XDK_LED_O, (uint32_t) BSP_LED_COMMAND_TOGGLE);
		if (RETCODE_OK != retVal)
		{
			printf("Failed to toggle orange led\r\n");
		}
		vTaskDelay(MILI_SECONDS(600));

		retVal = BSP_LED_Switch((uint32_t) BSP_XDK_LED_R, (uint32_t) BSP_LED_COMMAND_TOGGLE);
		if (RETCODE_OK != retVal)
		{
			printf("Failed to toggle red led\r\n");
		}
		vTaskDelay(MILI_SECONDS(600));
	}
}

/* Routine to Initialize the LED */
static Retcode_T LedInitialize(void)
{
    Retcode_T returnVal = RETCODE_OK;
    returnVal = BSP_LED_Connect();
    if (RETCODE_OK == returnVal)
    {
        returnVal = BSP_LED_Enable((uint32_t) BSP_XDK_LED_R);
    }
    if (RETCODE_OK == returnVal)
    {
        returnVal = BSP_LED_Enable((uint32_t) BSP_XDK_LED_O);
    }
    if (RETCODE_OK == returnVal)
    {
        returnVal = BSP_LED_Enable((uint32_t) BSP_XDK_LED_Y);
    }
    if (RETCODE_OK == returnVal)
    {
        printf("LED Initialization succeed without error %u \n", (unsigned int) returnVal);
    }
    else
    {
        printf(" Error occurred in LED Initialization routine %u \n", (unsigned int) returnVal);
    }
    return returnVal;
}

static void init (void)
{
	Retcode_T retVal = RETCODE_OK;

	retVal = LedInitialize();
	if (retVal != RETCODE_OK)
	{
		printf("failed to init led\r\n");
	}
	if (xTaskCreate(toggleLED, (const char *)"TOGGLE", configMINIMAL_STACK_SIZE,
	        		NULL, 4, &ledTaskHandle) != pdPASS)
	{
		printf("failed to create task\r\n");
		assert(0);
	}

	wlan_enable();

	retVal = BMA280_init();
	if (RETCODE_OK != retVal)
	{
		printf("Failed to initialize bma280AccTimerHandle\r\n");
		assert(false);
	}

	// Get time from ntp
	InitSntpTime();

    vTaskDelay(SECONDS(1));

    http_init();
}

/* global functions ********************************************************* */

/**
 * @brief This is a template function where the user can write his custom application.
 *
 */
void appInitSystem(void * CmdProcessorHandle, uint32_t param2)
{
    if (CmdProcessorHandle == NULL)
    {
        printf("Command processor handle is null \r\n");
        assert(false);
    }

    BCDS_UNUSED(param2);
    BCDS_UNUSED(CmdProcessorHandle);

    Retcode_T returnValue = ServalPalSetup();
    if (RETCODE_OK != returnValue)
    {
        Retcode_RaiseError(returnValue);
        printf("ServalPal Setup failed with %d \r\n", (int) returnValue);
        return;
    }
    // Enter the Appplication
    init();
}
/**@} */
/** ************************************************************************* */
