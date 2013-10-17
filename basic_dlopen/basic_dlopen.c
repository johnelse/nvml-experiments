#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

int main(int argv, char** argc) {
    void *nvml_handle;
    int err;
    char *error;

    nvml_handle = dlopen("libnvidia-ml.so.1", RTLD_LAZY);
    if (!nvml_handle) {
        fprintf(stderr, "Failed to load library: %s\n", dlerror());
        exit(1);
    }

    printf("Library opened\n");

    err = dlclose(nvml_handle);
    if (err) {
        fprintf(stderr, "Failed to close library: %s\n", dlerror());
        exit(1);
    }

    printf("Library closed\n");

    return 0;
}
