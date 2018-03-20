/*
 * accelerometer.c
 *
 *  Created on: Mar 20, 2018
 *      Author: nirao
 */

/* Interface for the sensors on XDK*/
#include "BMA280.h"
#include "XdkSensorHandle.h"

Accelerometer_XyzData_T bma280AccelData = {INT32_C(0), INT32_C(0), INT32_C(0)};

Retcode_T BMA280_init(void)
{
	Retcode_T retVal = RETCODE_OK;

	retVal = Accelerometer_init(xdkAccelerometers_BMA280_Handle);
	if (retVal != RETCODE_OK)
	{
		printf("Failed to initialize Accelerometer BMA280 (%d)\r\n", retVal);
		return retVal;
	}

	retVal = Accelerometer_setBandwidth(xdkAccelerometers_BMA280_Handle, ACCELEROMETER_BMA280_BANDWIDTH_125HZ);
	if (retVal != RETCODE_OK)
	{
		printf("Failed to set bandwidth of Accelerometer BMA280 (%d)\r\n", retVal);
		return retVal;
	}

	retVal = Accelerometer_setRange(xdkAccelerometers_BMA280_Handle, ACCELEROMETER_BMA280_RANGE_2G);
	if (retVal != RETCODE_OK)
	{
		printf("Failed to set range for Accelerometer BMA280 (%d)\r\n", retVal);
		return retVal;
	}

	return retVal;
}

Retcode_T BMA280_get_values(void)
{
	Retcode_T retVal = RETCODE_FAILURE;

	memset(&bma280AccelData, 0, sizeof(Accelerometer_XyzData_T));

	retVal = Accelerometer_readXyzGValue(xdkAccelerometers_BMA280_Handle, &bma280AccelData);
	if (retVal != RETCODE_OK)
	{
		printf("Failed to read accelerometer value (%d)\r\n", retVal);
	}
	else
	{
//		printf("BMA280 Acceleration values:\r\n X-Axis - (%f), "
//				"Y-Axis - (%f), Z-Axis - (%f)\r\n", (float)bma280AccelData.xAxisData,
//													(float)bma280AccelData.yAxisData,
//													(float)bma280AccelData.zAxisData);
	}
	return retVal;
}

Retcode_T BMA280_deinit(void)
{
	Retcode_T retVal = RETCODE_FAILURE;
	retVal = Accelerometer_deInit(xdkAccelerometers_BMA280_Handle);
	if (retVal != RETCODE_OK)
	{
		pritnf("Failed to deinit Accelerometer BMA280 (%d)\r\n", retVal);
	}
	else
	{
		printf('Accelerometer BMA280 Deinit success..\r\n');
	}
	return retVal;
}
