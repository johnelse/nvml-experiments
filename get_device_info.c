#include <stdio.h>
#include <stdlib.h>
#include <nvml.h>

int main(int argc, char** argv) {
    nvmlReturn_t result;
    unsigned int device_count, device_index, fan_speed, temp;
    nvmlDevice_t device;

    result = nvmlInit();
    if(NVML_SUCCESS != result) {
        printf("Failed to initialise: %s\n", nvmlErrorString(result));
        goto Error;
    };
    printf("nvmlInit successful\n");

    result = nvmlDeviceGetCount(&device_count);
    if(NVML_SUCCESS != result) {
        printf("Failed to get device count: %s\n", nvmlErrorString(result));
        goto Error;
    };
    printf("found %u devices\n", device_count);

    for(device_index = 0; device_index < device_count; device_index++) {
        result = nvmlDeviceGetHandleByIndex(device_index, &device);
        if(NVML_SUCCESS == result) {
            result = nvmlDeviceGetFanSpeed(device, &fan_speed);
            if (NVML_SUCCESS == result) {
                printf("device %u has fan speed %u\n", device_index, fan_speed);
            }
            else {
                printf("Failed to get device fan speed: %s\n", nvmlErrorString(result));
            };

            result = nvmlDeviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
            if(NVML_SUCCESS == result) {
                printf("device %u has temperature %uC\n", device_index, temp);
            }
            else {
                printf("Failed to get device temperature: %s\n", nvmlErrorString(result));
            };
        }
        else {
            printf("Failed to get handle for device %u: %s\n", device_index, nvmlErrorString(result));
        };
    };

    result = nvmlShutdown();
    if(NVML_SUCCESS != result)
        printf("Failed to shutdown: %s\n", nvmlErrorString(result));

    exit(0);

Error:
    result = nvmlShutdown();
    if(NVML_SUCCESS != result)
        printf("Failed to shutdown: %s\n", nvmlErrorString(result));

    exit(1);
}
