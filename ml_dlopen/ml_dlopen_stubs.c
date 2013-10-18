#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <dlfcn.h>

#include <nvml.h>

#include <caml/alloc.h>
#include <caml/callback.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/signals.h>

typedef struct nvmlInterface {
    void* handle;
    char* (*errorString)(nvmlReturn_t);
    nvmlReturn_t (*init)(void);
    nvmlReturn_t (*shutdown)(void);
    nvmlReturn_t (*deviceGetCount)(unsigned int*);
} nvmlInterface;

CAMLprim value stub_nvml_open(value unit)
{
    CAMLparam1(unit);
    CAMLlocal1(ml_interface);

    void *handle;
    nvmlInterface *interface;

    interface = malloc(sizeof(nvmlInterface));

    // Open the library.
    handle = dlopen("libnvidia-ml.so.1", RTLD_LAZY);
    if (!handle) {
        goto Error;
    }
    interface->handle = handle;

    // Load nvmlErrorString.
    interface->errorString = dlsym(handle, "nvmlErrorString");
    if (!interface->errorString) {
        goto Error;
    }

    // Load nvmlInit.
    interface->init = dlsym(handle, "nvmlInit");
    if (!interface->init) {
        goto Error;
    }

    // Load nvmlShutdown.
    interface->shutdown = dlsym(handle, "nvmlShutdown");
    if (!interface->shutdown) {
        goto Error;
    }

    // Load nvmlDeviceGetCount.
    interface->deviceGetCount = dlsym(handle, "nvmlDeviceGetCount");
    if (!interface->deviceGetCount) {
        goto Error;
    }

    ml_interface = (value)interface;
    CAMLreturn(ml_interface);

Error:
    free(interface);
    caml_failwith(dlerror());
}

CAMLprim value stub_nvml_close(value ml_interface)
{
    CAMLparam1(ml_interface);
    nvmlInterface* interface;

    interface = (nvmlInterface*)ml_interface;
    dlclose((void*)(interface->handle));
    free(interface);

    CAMLreturn(Val_unit);
}

void check_error(nvmlInterface* interface, nvmlReturn_t error) {
    if (NVML_SUCCESS != error) {
        caml_failwith(interface->errorString(error));
    }
}

CAMLprim value stub_nvml_init(value ml_interface)
{
    CAMLparam1(ml_interface);
    nvmlReturn_t error;
    nvmlInterface* interface;

    interface = (nvmlInterface*)ml_interface;
    error = interface->init();
    check_error(interface, error);

    CAMLreturn(Val_unit);
}

CAMLprim value stub_nvml_shutdown(value ml_interface)
{
    CAMLparam1(ml_interface);
    nvmlReturn_t error;
    nvmlInterface* interface;

    interface = (nvmlInterface*)ml_interface;
    error = interface->shutdown();
    check_error(interface, error);

    CAMLreturn(Val_unit);
}

CAMLprim value stub_nvml_device_get_count(value ml_interface) {
    CAMLparam1(ml_interface);
    nvmlReturn_t error;
    nvmlInterface* interface;
    unsigned int count;

    interface = (nvmlInterface*)ml_interface;
    error = interface->deviceGetCount(&count);
    check_error(interface, error);

    CAMLreturn(Val_int(count));
}
