#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

#include <nvml.h>

int main(int argv, char** argc) {
    void *nvml_handle;
    int err;
    nvmlReturn_t nvml_err;
    char *error;

    char* (*_nvmlErrorString)(nvmlReturn_t);
    nvmlReturn_t (*_nvmlInit)(void);
    nvmlReturn_t (*_nvmlShutdown)(void);
    nvmlReturn_t (*_nvmlSystemGetDriverVersion)(char*, unsigned int);

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
