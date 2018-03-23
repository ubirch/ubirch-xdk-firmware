/*
 * wlan.c
 *
 *  Created on: Mar 20, 2018
 *      Author: nirao
 */

#include <stdio.h>
#include <BCDS_Basics.h>

//#include "SntpTime.h"
#include "wlan.h"

#include "BCDS_WlanConnect.h"
#include "BCDS_NetworkConfig.h"
#include <Serval_Network.h>
#include "FreeRTOS.h"
#include "ubirch-xdk-firmware.h"

void wifiConnectCallback(WlanConnect_Status_T connectEvent)
{
	switch(connectEvent)
	{

	case WLAN_CONNECTED:

		printf("connected to ubirch\r\n");
//		xTaskCreate(pxTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask, puxStackBuffer, xRegions);
//		Retcode_T retVal = CmdProcessor_Enqueue(GetAppCmdProcessorHandle(), InitSntpTime, NULL, NULL);
//		if (retVal != RETCODE_OK)
//		{
//			printf("failed to enqueue sntp\r\n");
//		}
//		InitSntpTime();

		NetworkConfig_IpSettings_T currentIpSettings;
		Retcode_T retcode = NetworkConfig_GetIpSettings(&currentIpSettings);
		if (RETCODE_OK != retcode)
		{
			return;
		}
		else
		{
			uint32_t ipAddress = Basics_htonl(currentIpSettings.ipV4);

			char humanReadbleIpAddress[SERVAL_IP_ADDR_LEN] = { 0 };
			int conversionStatus = Ip_convertAddrToString(&ipAddress, humanReadbleIpAddress);
			if (conversionStatus < 0)
			{
				printf("Couldn't convert the IP address to string format \r\n");
			}
			else
			{
				printf("Connected to WPA network successfully \r\n");
				printf(" Ip address of the device %s \r\n", humanReadbleIpAddress);
			}
		}
		break;

	case WLAN_DISCONNECTED:
		printf("XDK Device disconnected form WIFI n/w \r\n");
		break;

	case WLAN_CONNECTION_ERROR:
		printf("XDK Device WIFI Connection error \r\n");
		break;

	case WLAN_CONNECTION_PWD_ERROR:
		printf("XDK Device WIFI connection error due to wrong password \r\n");
		break;

	case WLAN_DISCONNECT_ERROR:
		printf("XDK Device WIFI Disconnect error \r\n");
		break;

	default:
		printf("XDK Device unknown WIFI event \r\n");
		break;
	}
}

void wlan_enable(void)
{
//	BCDS_UNUSED(pvParameters);

	Retcode_T retval = RETCODE_OK;

	WlanConnect_SSID_T connectSSID = WIFI_SSID;
	WlanConnect_PassPhrase_T connectPass = WIFI_PASS;

	retval = WlanConnect_Init();
	if (retval != RETCODE_OK)
	{
		printf("Failed to init WLAN\r\n");
	}
	retval = NetworkConfig_SetIpDhcp(NULL);
	if (RETCODE_OK != retval)
	{
		printf("Error in setting IP to DHCP\n\r");
		return;
	}
	retval = WlanConnect_WPA(connectSSID, connectPass, NULL);
	if (retval != RETCODE_OK)
	{
		printf("Failed to connect WLAN\r\n");
	}
	NetworkConfig_IpSettings_T currentIpSettings;
	retval = NetworkConfig_GetIpSettings(&currentIpSettings);
	if (RETCODE_OK != retval)
	{
		return;
	}
	else
	{
		uint32_t ipAddress = Basics_htonl(currentIpSettings.ipV4);

		char humanReadbleIpAddress[SERVAL_IP_ADDR_LEN] = { 0 };
		int conversionStatus = Ip_convertAddrToString(&ipAddress, humanReadbleIpAddress);
		if (conversionStatus < 0)
		{
			printf("Couldn't convert the IP address to string format \r\n");
		}
		else
		{
			printf("Connected to WPA network successfully \r\n");
			printf(" Ip address of the device %s \r\n", humanReadbleIpAddress);
		}
	}
}

void wlan_scan(void *pvParameters)
{
	Retcode_T retScanStatus = RETCODE_OK;

	WlanConnect_ScanInterval_T scanInterval = 5;
	WlanConnect_ScanList_T scanList;
	portTickType delay500ms = 500;

	for(;;){
		retScanStatus = WlanConnect_ScanNetworks(scanInterval, &scanList);
		if (retScanStatus == RETCODE_OK)
		{
			for(int i = 0; i < scanList.NumOfScanEntries; i++)
			{
				if (scanList.ScanData[i].SsidLength != 0)
				{
					printf("Discovered SSID %d: %s, %d\r\n", i,
							scanList.ScanData[i].Ssid,
							scanList.ScanData[i].SecurityType);
				}
			}
		}
		else if (retScanStatus == RETCODE_NO_NW_AVAILABLE)
		{
			printf("No network available\r\n");
		}
		else
		{
			printf("Failed to scan for networks\r\n");
		}
		vTaskDelay(5000/portTICK_RATE_MS);
	}
}
