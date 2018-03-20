/*
 * accelerometer.h
 *
 *  Created on: Mar 20, 2018
 *      Author: nirao
 */

#ifndef SOURCE_BMA280_H_
#define SOURCE_BMA280_H_

#include "BCDS_Retcode.h"
#include "BCDS_Accelerometer.h"

extern Accelerometer_XyzData_T bma280AccelData;

Retcode_T BMA280_init(void);

Retcode_T BMA280_get_values(void);

Retcode_T BMA280_deinit(void);

#endif /* SOURCE_BMA280_H_ */
