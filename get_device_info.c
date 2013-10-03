#include <stdio.h>
#include <stdlib.h>
#include <nvml.h>

int main(int argc, char** argv) {
    nvmlReturn_t result;
    nvmlUtilization_t utilization;
    unsigned int device_count, device_index, temp;
    nvmlDevice_t device;

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
        // Get the device's handle.
        result = nvmlDeviceGetHandleByIndex(device_index, &device);
        if(NVML_SUCCESS == result) {
            // Get the device's utilization.
            result = nvmlDeviceGetUtilizationRates(device, &utilization);
            if(NVML_SUCCESS == result) {
                printf("Device utilization: gpu = %u, memory = %u\n", utilization.gpu, utilization.memory);
            }
            else {
                printf("Failed to get device utilization: %s\n", nvmlErrorString(result));
            };

            // Get the device's temperature.
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
