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

    // Initialise the library.
    nvml_err = _nvmlInit();
    if (NVML_SUCCESS != nvml_err) {
        fprintf(stderr, "init failed: %s\n", _nvmlErrorString(nvml_err));
        exit(1);
    }
    printf("Initialised NVML\n");

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

    return 0;
}
