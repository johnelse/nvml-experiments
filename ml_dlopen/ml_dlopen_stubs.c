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
    nvmlReturn_t (*deviceGetHandleByIndex)(unsigned int, nvmlDevice_t*);
    nvmlReturn_t (*deviceGetTemperature)
        (nvmlDevice_t, nvmlTemperatureSensors_t, unsigned int*);
} nvmlInterface;

CAMLprim value stub_nvml_open(value unit) {
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

    // Load nvmlDeviceGetHandleByIndex.
    interface->deviceGetHandleByIndex =
        dlsym(handle, "nvmlDeviceGetHandleByIndex");
    if(!interface->deviceGetHandleByIndex) {
        goto Error;
    }

    // Load nvmlDeviceGetTemperature.
    interface->deviceGetTemperature =
        dlsym(handle, "nvmlDeviceGetTemperature");
    if(!interface->deviceGetTemperature) {
        goto Error;
    }

    ml_interface = (value)interface;
    CAMLreturn(ml_interface);

Error:
    free(interface);
    caml_failwith(dlerror());
}

CAMLprim value stub_nvml_close(value ml_interface) {
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

CAMLprim value stub_nvml_init(value ml_interface) {
    CAMLparam1(ml_interface);
    nvmlReturn_t error;
    nvmlInterface* interface;

    interface = (nvmlInterface*)ml_interface;
    error = interface->init();
    check_error(interface, error);

    CAMLreturn(Val_unit);
}

CAMLprim value stub_nvml_shutdown(value ml_interface) {
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

CAMLprim value stub_nvml_device_get_handle_by_index(
        value ml_interface,
        value ml_index) {
    CAMLparam2(ml_interface, ml_index);
    nvmlReturn_t error;
    nvmlInterface* interface;
    unsigned int index;
    nvmlDevice_t* device;

    device = malloc(sizeof(nvmlDevice_t));
    interface = (nvmlInterface*)ml_interface;
    index = Int_val(ml_index);
    error = interface->deviceGetHandleByIndex(index, device);
    check_error(interface, error);

    CAMLreturn((value)device);
}

CAMLprim value stub_nvml_device_get_temperature(
        value ml_interface,
        value ml_device) {
    CAMLparam2(ml_interface, ml_device);
    nvmlReturn_t error;
    nvmlInterface* interface;
    unsigned int temp;
    nvmlDevice_t device;

    interface = (nvmlInterface*)ml_interface;
    device = *(nvmlDevice_t*)ml_device;
    error =
        interface->deviceGetTemperature(device, NVML_TEMPERATURE_GPU, &temp);
    check_error(interface, error);

    CAMLreturn(Val_int(temp));
}
