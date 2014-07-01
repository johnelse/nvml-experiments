#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

#include <nvml.h>

/**
 * - Load the NVML library via dlopen.
 * - Look up some symbols:
 *   - nvmlErrorString
 *   - nvmlInit
 *   - nvmlShutdown
 *   - nvmlSystemGetDriverVersion
 * - Using these symbols:
 *   - Initialise the library.
 *   - Get the driver version.
 *   - Shut down the library.
 * - Finally, unload the library with dlclose.
 */

int main(int argv, char** argc) {
    void *nvml_handle;
    int err;
    nvmlReturn_t nvml_err;
    char *error;
    unsigned int i;
    unsigned int device_count;
    nvmlDevice_t device;
    nvmlPciInfo_t pci_info;

    char* (*_nvmlErrorString)(nvmlReturn_t);
    nvmlReturn_t (*_nvmlInit)(void);
    nvmlReturn_t (*_nvmlShutdown)(void);
    nvmlReturn_t (*_nvmlSystemGetDriverVersion)(char*, unsigned int);
    nvmlReturn_t (*_nvmlDeviceGetCount)(unsigned int*);
    nvmlReturn_t (*_nvmlDeviceGetHandleByIndex)(unsigned int, nvmlDevice_t*);
    nvmlReturn_t (*_nvmlDeviceGetPciInfo)(nvmlDevice_t, nvmlPciInfo_t*);

    char driver_version[NVML_SYSTEM_NVML_VERSION_BUFFER_SIZE];

    // Open a handle to the shared library.
    nvml_handle = dlopen("libnvidia-ml.so.1", RTLD_LAZY);
    if (!nvml_handle) {
        fprintf(stderr, "Failed to load library: %s\n", dlerror());
        exit(1);
    }
    printf("Library opened\n");

    // Load some symbols.
    _nvmlErrorString = dlsym(nvml_handle, "nvmlErrorString");
    if((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("Found nvmlErrorString\n");

    _nvmlInit = dlsym(nvml_handle, "nvmlInit");
    if((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("Found nvmlInit\n");

    _nvmlShutdown = dlsym(nvml_handle, "nvmlShutdown");
    if((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("Found nvmlShutdown\n");

    _nvmlSystemGetDriverVersion =
        dlsym(nvml_handle, "nvmlSystemGetDriverVersion");
    if((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("Found nvmlSystemGetDriverVersion\n");

    _nvmlDeviceGetCount =
        dlsym(nvml_handle, "nvmlDeviceGetCount");
    if((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("Found nvmlDeviceGetCount\n");

    _nvmlDeviceGetHandleByIndex =
        dlsym(nvml_handle, "nvmlDeviceGetHandleByIndex");
    if((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("Found nvmlDeviceGetHandleByIndex\n");

    _nvmlDeviceGetPciInfo =
        dlsym(nvml_handle, "nvmlDeviceGetPciInfo");
    if((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        exit(1);
    }
    printf("Found nvmlDeviceGetPciInfo\n");

    // Initialise the library.
    nvml_err = _nvmlInit();
    if (NVML_SUCCESS != nvml_err) {
        fprintf(stderr, "init failed: %s\n", _nvmlErrorString(nvml_err));
        exit(1);
    }
    printf("Initialised NVML\n");

    // Query the driver version.
    nvml_err = _nvmlSystemGetDriverVersion(driver_version,
        NVML_SYSTEM_NVML_VERSION_BUFFER_SIZE);
    if (NVML_SUCCESS != nvml_err) {
        fprintf(stderr, "couldn't get driver version: %s\n",
                _nvmlErrorString(nvml_err));
        exit(1);
    }
    printf("Got driver version: %s\n", driver_version);

    // Query the number of installed devices.
    nvml_err = _nvmlDeviceGetCount(&device_count);
    if (NVML_SUCCESS != nvml_err) {
        fprintf(stderr, "couldn't get device count: %s\n",
                _nvmlErrorString(nvml_err));
        exit(1);
    }

    // Attempt to query each device's PCI info.
    for (i = 0; i < device_count; i++) {
        nvml_err = _nvmlDeviceGetHandleByIndex(i, &device);
        if (NVML_SUCCESS != nvml_err) {
            fprintf(stderr, "couldn't get handle for device %d: %s\n",
                    i, _nvmlErrorString(nvml_err));
        }
        else {
            nvml_err = _nvmlDeviceGetPciInfo(device, &pci_info);
            if (NVML_SUCCESS != nvml_err) {
                fprintf(stderr, "couldn't get PCI info for device %d: %s\n",
                        i, _nvmlErrorString(nvml_err));
            }
            else {
                printf("%s\n", "-------------------");
                printf("PCI info for device %d:\n", i);
                printf("busId = %s\n", pci_info.busId);
                printf("pciDeviceId = %x\n", pci_info.pciDeviceId);
                printf("pciSubSystemId = %x\n", pci_info.pciSubSystemId);
            }
        }
    }

    // Shutdown the library.
    nvml_err = _nvmlShutdown();
    if (NVML_SUCCESS != nvml_err) {
        fprintf(stderr, "shutdown failed: %s\n", _nvmlErrorString(nvml_err));
        exit(1);
    }
    printf("Shutdown NVML\n");

    // Close the handle.
    err = dlclose(nvml_handle);
    if (err) {
        fprintf(stderr, "Failed to close library: %s\n", dlerror());
        exit(1);
    }
    printf("Library closed\n");

    printf("Everything seems to be working OK :-)\n");

    return 0;
}
