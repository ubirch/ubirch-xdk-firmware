/*
 * ubirch-proto.c
 *
 *  Created on: Mar 21, 2018
 *      Author: nirao
 */

#include <stdio.h>

#include "wlan.h"

#include <Serval_HttpClient.h>
#include <Serval_Network.h>
#include "BCDS_ServalPal.h"
#include "BCDS_Basics.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "ubirch_protocol.h"
#include "ubirch_ed25519.h"

#include "ubirch-proto.h"
#include "BMA280.h"
#include "SntpTime.h"
#include "ubirch-xdk-firmware.h"


#define REQUEST_MAX_DOWNLOAD_SIZE       UINT32_C(512)

msgpack_sbuffer sbuf = {0}; // msgpack_sbuffer_new();

xTaskHandle httpTaskHandle =NULL;
xTaskHandle ubProtoTaskHandle = NULL;

xTimerHandle ubProtoTimerHandle  = NULL;
xSemaphoreHandle ReadSensorSemaphor = NULL; /**< Semaphore to lock/unlock the Ping/Pong-Buffer*/

unsigned char prevSig[64] = {0};
unsigned char UUID[16] = { 0 };

unsigned char ed25519_secret_key[crypto_sign_SECRETKEYBYTES] = {
		0x69, 0x09, 0xcb, 0x3d, 0xff, 0x94, 0x43, 0x26, 0xed, 0x98, 0x72, 0x60,
		0x1e, 0xb3, 0x3c, 0xb2, 0x2d, 0x9e, 0x20, 0xdb, 0xbb, 0xe8, 0x17, 0x34,
		0x1c, 0x81, 0x33, 0x53, 0xda, 0xc9, 0xef, 0xbb, 0x7c, 0x76, 0xc4, 0x7c,
		0x51, 0x61, 0xd0, 0xa0, 0x3e, 0x7a, 0xe9, 0x87, 0x01, 0x0f, 0x32, 0x4b,
		0x87, 0x5c, 0x23, 0xda, 0x81, 0x31, 0x32, 0xcf, 0x8f, 0xfd, 0xaa, 0x55,
		0x93, 0xe6, 0x3e, 0x6a
};

static void getubirchprotocol(void * param);
void http_init(void)
{
	retcode_t rc = HttpClient_initialize();
	if (RC_OK != rc)
	{
		printf("Failed to initialize http client \r\n ");
		return;
	}
	ReadSensorSemaphor = xSemaphoreCreateMutex();
	if (ReadSensorSemaphor == NULL)
	{
		assert(0);
	}

	if (xTaskCreate(getubirchprotocol, (const char * const)"UBPROTOTASK",
				TASK_STACK_SIZE_HTTP_REQ, NULL, TASK_PRIO_HTTP_REQ, &ubProtoTaskHandle) != pdPASS)
	{
		printf("failed to create ub proto task\r\n");
		assert(0);
	}
	if (xTaskCreate(httpPostTask, (const char * const)"HTTPTASK",
			TASK_STACK_SIZE_HTTP_REQ, NULL, TASK_PRIO_HTTP_REQ, &httpTaskHandle) != pdPASS)
	{
		printf("failed to create http task\r\n");
		assert(0);
	}

//	ubProtoTimerHandle = xTimerCreate((const char *)"UB-PROTO",
//													pdMS_TO_TICKS(3),
//													pdTRUE,
//													NULL,
//													getubirchprotocol);
//
//	if (ubProtoTimerHandle != NULL)
//	{
//		xTimerStart(ubProtoTimerHandle, (10/portTICK_RATE_MS));
//	}
}

static void getubirchprotocol(void * param)
{
	BCDS_UNUSED(param);

//	if ((xSemaphoreTake(ReadSensorSemaphor, (portTickType)1000)
//												== pdTRUE))
//	{
	for (;;)
	{
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		msgpack_sbuffer_clear(&sbuf);

		BMA280_get_values();

		ubirch_protocol proto = {};

		// Initialize ubirch protocol
		ubirch_protocol_init(&proto, proto_chained, 0x00, &sbuf,
			msgpack_sbuffer_write, ed25519_sign, UUID);

		//Copy the previcous signature if exists
		memcpy(proto.signature, prevSig, UBIRCH_PROTOCOL_SIGN_SIZE);

		// create a packer for ubirch protocol
		msgpack_packer pk;
		msgpack_packer_init(&pk, &proto, ubirch_protocol_write);

		// Prepare message header
		ubirch_protocol_start(&proto, &pk);

		// Create map("data": map(timstamp: map(sensortype: values)))
		msgpack_pack_map(&pk, 1);
		char *dataPayload = "data";
		msgpack_pack_raw(&pk, strlen(dataPayload));
		msgpack_pack_raw_body(&pk, dataPayload, strlen(dataPayload));

		time_t ts = GetUtcTime();
		// map(timestamp : map(sensortype:value), map(sensortype:value), map(sensortype:value))
		msgpack_pack_map(&pk, 1);
		msgpack_pack_uint32(&pk, ts);

		printf("MSGPACK: BMA280 Acceleration values:(%lu)\r\n X-Axis - (%f), "
								"Y-Axis - (%f), Z-Axis - (%f)\r\n",
								ts,
								(float)bma280AccelData.xAxisData,
								(float)bma280AccelData.yAxisData,
								(float)bma280AccelData.zAxisData);

		msgpack_pack_map(&pk, 3);

		char *xAxisValue = "ACCEL-X";
		msgpack_pack_raw(&pk, strlen(dataPayload));
		msgpack_pack_raw_body(&pk, dataPayload, strlen(xAxisValue));
		msgpack_pack_int32(&pk, bma280AccelData.xAxisData);

		char *yAxisValue = "ACCEL-Y";
		msgpack_pack_raw(&pk, strlen(dataPayload));
		msgpack_pack_raw_body(&pk, dataPayload, strlen(yAxisValue));
		msgpack_pack_int32(&pk, bma280AccelData.yAxisData);

		char *zAxisValue = "ACCEL-Z";
		msgpack_pack_raw(&pk, strlen(zAxisValue));
		msgpack_pack_raw_body(&pk, dataPayload, strlen(zAxisValue));
		msgpack_pack_int32(&pk, bma280AccelData.zAxisData);

		ubirch_protocol_finish(&proto, &pk);

		// copy the last signature into our local buffer
		memcpy(prevSig, proto.signature, UBIRCH_PROTOCOL_SIGN_SIZE);

		//	return RC_OK;
//		if (xSemaphoreGive(ReadSensorSemaphor) != pdTRUE)
//		{
//			assert(0);
//		}
	    xTaskNotifyGive(httpTaskHandle);
	}
}

static retcode_t onHTTPRequestSent(Callable_T *callfunc, retcode_t status)
{
	(void) (callfunc);
	if (status != RC_OK)
	{
		printf("Failed to send HTTP request!\r\n");
		return status;
	}
	printf("sucess send HTTP request!\r\n");
	return(RC_OK);
}

static retcode_t onHTTPResponseReceived(HttpSession_T *httpSession,
										Msg_T *msg_ptr, retcode_t status)
{
    BCDS_UNUSED(httpSession);

    if (RC_OK != status)
    {
        printf("httpPostResponseCallback: error while receiving response to POST request. error=%d\r\n", status);
        return RC_APP_ERROR;
    }
    if (NULL == httpSession)
    {
        printf("httpPostResponseCallback: received NULL as HTTP message. This should not happen.\r\n");
        return RC_APP_ERROR;
    }

    Http_StatusCode_T httpStatusCode = HttpMsg_getStatusCode(httpSession);
    if (Http_StatusCode_OK != httpStatusCode)
    {
        printf("httpPostResponseCallback: received HTTP status other than 200 OK. status=%d\r\n", httpStatusCode);
    }
    else
    {
        retcode_t retcode;
        bool isLastPartOfMessage;
        uint32_t pageContentSize;
        retcode = HttpMsg_getRange(httpSession, UINT32_C(0), &pageContentSize, &isLastPartOfMessage);
        if (RC_OK != retcode)
        {
            printf("httpPostResponseCallback: failed to get range from message. error=%d\r\n", retcode);
        }
        else
        {
            const char* responseContent;
            unsigned int responseContentLen;
            HttpMsg_getContent(httpSession, &responseContent, &responseContentLen);
            printf("httpPostResponseCallback: successfully received a response: %.*s\r\n", responseContentLen, responseContent);

            if (!isLastPartOfMessage)
            {
                /* We're not done yet downloading the page - let's make another request. */
                printf("httpPostResponseCallback: server response was too large. This example application does not support POST responses larger than %lu.\r\n", REQUEST_MAX_DOWNLOAD_SIZE);
            }
        }
    }

    return RC_OK;
}

retcode_t writeNextPartToBuffer(OutMsgSerializationHandover_T* handover)
{
	retcode_t rc = RC_OK;

//	if ((xSemaphoreTake(ReadSensorSemaphor, (portTickType)100) == pdTRUE))
//	{
		uint16_t payloadLength = (uint16_t) sbuf.size;
		uint16_t alreadySerialized = handover->offset;
		uint16_t remainingLength = payloadLength - alreadySerialized;
		uint16_t bytesToCopy;

		if ( remainingLength <= handover->bufLen )
		{
			bytesToCopy = remainingLength;
			rc = RC_OK;
		}
		else
		{
			bytesToCopy = handover->bufLen;
			rc = RC_MSG_FACTORY_INCOMPLETE;
		}

		memcpy(handover->buf_ptr, sbuf.data + alreadySerialized, bytesToCopy);

		handover->offset = alreadySerialized + bytesToCopy;
		handover->len = bytesToCopy;

//		if (xSemaphoreGive(ReadSensorSemaphor) != pdTRUE)
//		{
//			assert(0);
//		}
//	}
	return rc;
}

void httpPostTask(void * p)
{
	BCDS_UNUSED(p);
	retcode_t retCode = RC_APP_ERROR;
	Retcode_T retVal = RETCODE_FAILURE;

	for (;;){

		xTaskNotifyGive(ubProtoTaskHandle);

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

		Ip_Address_T destAddr = 0;
		Ip_Port_T port = Ip_convertIntToPort(DESTINATION_SERVER_PORT);
		Msg_T* msg_ptr;

		retVal = NetworkConfig_GetIpAddress((uint8_t*) DESTINATION_SERVER_HOST, &destAddr);
		if (retVal != RETCODE_OK)
		{
			printf("httpPostTask: unable to resolve hostname " DESTINATION_SERVER_HOST ". error=%d.\r\n", (int) retVal);
		}

		else if (retVal == RETCODE_OK)
		{
			retCode = HttpClient_initRequest(&destAddr, port, &msg_ptr);

			if (retCode != RC_OK)
			{
				printf("HttpPostRequest: unable to create http request, ERROR(%d)\r\n", retCode);
				retVal = RETCODE_FAILURE;
			}
		}

		if (retVal == RETCODE_OK)
		{
			HttpMsg_setReqMethod(msg_ptr, Http_Method_Post);

			HttpMsg_setContentType(msg_ptr, Http_ContentType_App_Json);

			retCode = HttpMsg_setReqUrl(msg_ptr, "/post");

			if (RC_OK != retCode)
			{
				printf("httpPostTask: unable to set request URL. error=%d.\r\n", retCode);
				retVal = RETCODE_FAILURE;
			}
		}
		if (retVal == RETCODE_OK)
		{
			retCode = HttpMsg_setHost(msg_ptr, DESTINATION_SERVER_HOST);
			if (RC_OK != retCode)
			{
				printf("httpPostTask: unable to set HOST header. error=%d.\r\n", retCode);
				retVal = RETCODE_FAILURE;
			}
		}

		if (retVal == RETCODE_OK)
		{
			retCode = Msg_prependPartFactory(msg_ptr, &writeNextPartToBuffer);
			if (RC_OK != retCode)
			{
				printf("httpPostTask: unable to serialize request body. error=%d.\r\n", retCode);
				retVal = RETCODE_FAILURE;
			}
		}

		if (retVal == RETCODE_OK)
		{
			// send the request
			static Callable_T sentCallable;
			Callable_assign(&sentCallable, &onHTTPRequestSent);

			retCode  = HttpClient_pushRequest(msg_ptr, &sentCallable, &onHTTPResponseReceived);
			if (RC_OK != retCode)
			{
				printf("httpPostTask: unable to push the HTTP request. error=%d.\r\n", retCode);
			}
		}
		 vTaskDelay((portTickType) 10000 / portTICK_RATE_MS);
	}
}



