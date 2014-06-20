#include <stdio.h>
#include <stdlib.h>
#include <nvml.h>

/**
 * Iterate through all the devices on the system which are recognised by NVML.
 * For each device, print out:
 * - The device's index.
 * - The device's name.
 * - The device's PCI info.
 * - The device's utilisation, if supported.
 * - The device's ECC mode, if supported.
 * - The device's fan speed, if supported.
 * - The device's temperature.
 * - The device's persistence mode.
 * - The device's power usage, if supported.
 */

const unsigned int MAX_DEVICE_NAME_LENGTH = 64;

int main(int argc, char** argv) {
    nvmlReturn_t result;
    nvmlUtilization_t utilization;
    char device_name[MAX_DEVICE_NAME_LENGTH];
    unsigned int device_count, device_index, fan_speed, temp, power_usage;
    unsigned int device_id, vendor_id, subsystem_device_id, subsystem_vendor_id;
    nvmlDevice_t device;
    nvmlPciInfo_t pci_info;
    nvmlEnableState_t current, pending;
    nvmlEnableState_t persistence_mode;

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
            result = nvmlDeviceGetName(device, device_name, MAX_DEVICE_NAME_LENGTH);
            if(NVML_SUCCESS == result) {
                printf("Device name: %s\n", device_name);
            }
            else {
                printf("Failed to get device name: %s\n", nvmlErrorString(result));
            };

            result = nvmlDeviceGetPciInfo(device, &pci_info);
            if(NVML_SUCCESS == result) {
                device_id = pci_info.pciDeviceId >> 16;
                vendor_id = pci_info.pciDeviceId & 0xffff;
                subsystem_device_id = pci_info.pciSubSystemId >> 16;
                subsystem_vendor_id = pci_info.pciSubSystemId & 0xffff;
                printf("device id = %x\n", device_id);
                printf("vendor id = %x\n", vendor_id);
                printf("subsystem id = %x\n", pci_info.pciSubSystemId);
                printf("subsystem device id = %x\n", subsystem_device_id);
                printf("subsystem vendor id = %x\n", subsystem_vendor_id);
            }
            else {
                printf("Failed to get PCI info: %s\n", nvmlErrorString(result));
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

            // Get the device's persistence mode.
            result = nvmlDeviceGetPersistenceMode(device, &persistence_mode);
            if(NVML_SUCCESS == result) {
                if(NVML_FEATURE_ENABLED == persistence_mode) {
                    printf("Device has persistence mode enabled\n");
                }
                else {
                    printf("Device has persistence mode disabled\n");
                }
            }
            else {
                printf("Failed to get persistence mode: %s\n", nvmlErrorString(result));
            }
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
