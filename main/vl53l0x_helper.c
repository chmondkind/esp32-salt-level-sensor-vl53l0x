/*

MIT License

Copyright (c) 2018 Kyle Hendricks

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/



#include "vl53l0x_helper.h"

#include "vl53l0x_api.h"

static const char *TAG = "vl53l0x_helper";

static void print_pal_error(VL53L0X_Error status, const char* method)
{
    if (status == VL53L0X_ERROR_NONE)
        return;
    char buf[VL53L0X_MAX_STRING_LENGTH];
    VL53L0X_GetPalErrorString(status, buf);
    ESP_LOGI(TAG, "%s API status: %i : %s\n", method, status, buf);
}

VL53L0X_Error init_vl53l0x(VL53L0X_Dev_t *pMyDevice)
{
    VL53L0X_Error status;
    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;

    status = VL53L0X_DataInit(pMyDevice);
    print_pal_error(status, "VL53L0X_DataInit");

    if(status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_StaticInit(pMyDevice); // Device Initialization
        print_pal_error(status, "VL53L0X_StaticInit");
    }

    if(status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_PerformRefCalibration(pMyDevice,
                                               &VhvSettings, &PhaseCal); // Device Initialization
        print_pal_error(status, "VL53L0X_PerformRefCalibration");
    }

    if(status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_PerformRefSpadManagement(pMyDevice,
                                                  &refSpadCount, &isApertureSpads); // Device Initialization
        ESP_LOGI(TAG,"refSpadCount = %d, isApertureSpads = %d\n", refSpadCount, isApertureSpads);
        print_pal_error(status, "VL53L0X_PerformRefSpadManagement");
    }

    if(status == VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_SetDeviceMode(pMyDevice, VL53L0X_DEVICEMODE_SINGLE_RANGING); // Setup in single ranging mode
        print_pal_error(status, "VL53L0X_SetDeviceMode");
    }

    // Enable/Disable Sigma and Signal check
    if (status == VL53L0X_ERROR_NONE) {
        status = VL53L0X_SetLimitCheckEnable(pMyDevice,
                                             VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
        print_pal_error(status, "VL53L0X_SetLimiteCheckEnable");
    }
    if (status == VL53L0X_ERROR_NONE) {
        status = VL53L0X_SetLimitCheckEnable(pMyDevice,
                                             VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
        print_pal_error(status, "VL53L0X_SetLimiteCheckEnable");
    }

    if (status == VL53L0X_ERROR_NONE) {
        status = VL53L0X_SetLimitCheckEnable(pMyDevice,
                                             VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD, 1);
        print_pal_error(status, "VL53L0X_SetLimiteCheckEnable");
    }

    //if (status == VL53L0X_ERROR_NONE) {
    //    status = VL53L0X_SetLimitCheckValue(pMyDevice,
    //                                        VL53L0X_CHECKENABLE_RANGE_IGNORE_THRESHOLD,
    //                                        (FixPoint1616_t)(1.5*0.023*65536));
    //    print_pal_error(status, "VL53L0X_SetLimitCheckValue");
    //}

    if (status == VL53L0X_ERROR_NONE) {
        status = VL53L0X_SetLimitCheckValue(pMyDevice,
                                            VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
                                            (FixPoint1616_t)(0.25*65536));
        print_pal_error(status, "VL53L0X_SetLimitCheckValue");
    }

    if (status == VL53L0X_ERROR_NONE) {
        status = VL53L0X_SetLimitCheckValue(pMyDevice,
                                            VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,
                                            (FixPoint1616_t)(18*65536));
        print_pal_error(status, "VL53L0X_SetLimitCheckValue");
    }

    if (status != VL53L0X_ERROR_NONE)
    {
        status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(pMyDevice,200000);
        print_pal_error(status, "VL53L0X_SetMeasurementTimingBudgetMicroSeconds");
    }

    return status;
}

VL53L0X_Error take_reading(VL53L0X_Dev_t *pMyDevice, VL53L0X_RangingMeasurementData_t *measurementData)
{
    VL53L0X_Error status = VL53L0X_PerformSingleRangingMeasurement(pMyDevice, measurementData);
    print_pal_error(status, "VL53L0X_PerformSingleRangingMeasurement");
    return status;
}
