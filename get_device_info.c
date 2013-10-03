#include <stdio.h>
#include <stdlib.h>
#include <nvml.h>

unsigned int DEVICE_NAME_LENGTH = 64;

int main(int argc, char** argv) {
    nvmlReturn_t result;
    nvmlUtilization_t utilization;
    char device_name[DEVICE_NAME_LENGTH];
    unsigned int device_count, device_index, fan_speed, temp, power_usage;
    nvmlDevice_t device;
    nvmlEnableState_t current, pending;

    // Initialise the library.
    result = nvmlInit();
    if(NVML_SUCCESS != result) {
        printf("Failed to initialise: %s\n", nvmlErrorString(result));
        goto Error;
    };
    printf("nvmlInit successful\n");

    // Get the number of installed devices.
    result = nvmlDeviceGetCount(&device_count);
    if(NVML_SUCCESS != result) {
        printf("Failed to get device count: %s\n", nvmlErrorString(result));
        goto Error;
    };
    printf("found %u devices\n", device_count);

    // Iterate through the devices.
    for(device_index = 0; device_index < device_count; device_index++) {
        printf("---------- Device %u ----------\n", device_index);

        // Get the device's handle.
        result = nvmlDeviceGetHandleByIndex(device_index, &device);
        if(NVML_SUCCESS == result) {
            // Get the device's name.
            result = nvmlDeviceGetName(device, device_name, DEVICE_NAME_LENGTH);
            if(NVML_SUCCESS == result) {
                printf("Device name: %s\n", device_name);
            }
            else {
                printf("Failed to get device name: %s\n", nvmlErrorString(result));
            };

            // Get the device's utilization.
            result = nvmlDeviceGetUtilizationRates(device, &utilization);
            if(NVML_SUCCESS == result) {
                printf("Device utilization: gpu = %u%%, memory = %u%%\n", utilization.gpu, utilization.memory);
            }
            else {
                printf("Failed to get device utilization: %s\n", nvmlErrorString(result));
            };

            // Get the current and pending ECC modes.
            result = nvmlDeviceGetEccMode(device, &current, &pending);
            if(NVML_SUCCESS == result) {
                if(NVML_FEATURE_ENABLED == current) {
                    printf("Device has ECC enabled\n");
                }
                else {
                    printf("Device has ECC disabled\n");
                }
            }
            else
            {
                printf("Failed to get ECC mode: %s\n", nvmlErrorString(result));
            }

            // Get the device's fan speed.
            result = nvmlDeviceGetFanSpeed(device, &fan_speed);
            if(NVML_SUCCESS == result) {
                printf("Device has fan speed %u%%\n", fan_speed);
            }
            else {
                printf("Failed to get device fan speed: %s\n", nvmlErrorString(result));
            };

            // Get the device's temperature.
            result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
            if(NVML_SUCCESS == result) {
                printf("Device has temperature %uC\n", temp);
            }
            else {
                printf("Failed to get device temperature: %s\n", nvmlErrorString(result));
            };

            // Get the device's power usage.
            result = nvmlDeviceGetPowerUsage(device, &power_usage);
            if(NVML_SUCCESS == result) {
                printf("Device has power usage %umW\n", power_usage);
            }
            else {
                printf("Failed to get device power usage: %s\n", nvmlErrorString(result));
            };
        }
        else {
            printf("Failed to get handle for device: %s\n", nvmlErrorString(result));
        };
    };

    // Shutdown the library.
    result = nvmlShutdown();
    if(NVML_SUCCESS != result)
        printf("Failed to shutdown: %s\n", nvmlErrorString(result));

    exit(0);

Error:
    // Handle any unrecoverable errors.
    result = nvmlShutdown();
    if(NVML_SUCCESS != result)
        printf("Failed to shutdown: %s\n", nvmlErrorString(result));

    exit(1);
}
