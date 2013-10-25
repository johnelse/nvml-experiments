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
    nvmlReturn_t (*deviceGetPciInfo)(nvmlDevice_t, nvmlPciInfo_t*);
    nvmlReturn_t (*deviceGetPowerUsage)(nvmlDevice_t, unsigned int*);
    nvmlReturn_t (*deviceGetTemperature)
        (nvmlDevice_t, nvmlTemperatureSensors_t, unsigned int*);
    nvmlReturn_t (*deviceGetUtilizationRates)(nvmlDevice_t, nvmlUtilization_t*);
} nvmlInterface;

CAMLprim value stub_nvml_open(value unit) {
    CAMLparam1(unit);
    CAMLlocal1(ml_interface);

    nvmlInterface *interface;
    value *exn;

    interface = malloc(sizeof(nvmlInterface));

    // Open the library.
    interface->handle = dlopen("libnvidia-ml.so.1", RTLD_LAZY);
    if (!interface->handle) {
        free(interface);
        exn = caml_named_value("Library_not_loaded");
        if (exn) {
            caml_raise_with_string(*exn, dlerror());
        }
        else {
            caml_failwith(dlerror());
        }
    }

    // Load nvmlErrorString.
    interface->errorString = dlsym(interface->handle, "nvmlErrorString");
    if (!interface->errorString) {
        goto SymbolError;
    }

    // Load nvmlInit.
    interface->init = dlsym(interface->handle, "nvmlInit");
    if (!interface->init) {
        goto SymbolError;
    }

    // Load nvmlShutdown.
    interface->shutdown = dlsym(interface->handle, "nvmlShutdown");
    if (!interface->shutdown) {
        goto SymbolError;
    }

    // Load nvmlDeviceGetCount.
    interface->deviceGetCount = dlsym(interface->handle, "nvmlDeviceGetCount");
    if (!interface->deviceGetCount) {
        goto SymbolError;
    }

    // Load nvmlDeviceGetHandleByIndex.
    interface->deviceGetHandleByIndex =
        dlsym(interface->handle, "nvmlDeviceGetHandleByIndex");
    if(!interface->deviceGetHandleByIndex) {
        goto SymbolError;
    }

    // Load nvmlDeviceGetPciInfo.
    interface->deviceGetPciInfo =
        dlsym(interface->handle, "nvmlDeviceGetPciInfo");
    if(!interface->deviceGetPciInfo) {
        goto SymbolError;
    }

    // Load nvmlDeviceGetPowerUsage.
    interface->deviceGetPowerUsage =
        dlsym(interface->handle, "nvmlDeviceGetPowerUsage");
    if(!interface->deviceGetPowerUsage) {
        goto SymbolError;
    }

    // Load nvmlDeviceGetTemperature.
    interface->deviceGetTemperature =
        dlsym(interface->handle, "nvmlDeviceGetTemperature");
    if(!interface->deviceGetTemperature) {
        goto SymbolError;
    }

    // Load nvmlDeviceGetUtilizationRates.
    interface->deviceGetUtilizationRates =
        dlsym(interface->handle, "nvmlDeviceGetUtilizationRates");
    if(!interface->deviceGetUtilizationRates) {
        goto SymbolError;
    }

    ml_interface = (value)interface;
    CAMLreturn(ml_interface);

SymbolError:
    free(interface);
    exn = caml_named_value("Symbol_not_loaded");
    if (exn) {
        caml_raise_with_string(*exn, dlerror());
    }
    else {
        caml_failwith(dlerror());
    }
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

CAMLprim value stub_nvml_device_get_power_usage(
        value ml_interface,
        value ml_device) {
    CAMLparam2(ml_interface, ml_device);
    nvmlReturn_t error;
    nvmlInterface* interface;
    nvmlDevice_t device;
    unsigned int power_usage;

    interface = (nvmlInterface*)ml_interface;
    device = *(nvmlDevice_t*)ml_device;
    error = interface->deviceGetPowerUsage(device, &power_usage);
    check_error(interface, error);

    CAMLreturn(Val_int(power_usage));
}

CAMLprim value stub_nvml_device_get_pci_info(
        value ml_interface,
        value ml_device) {
    CAMLparam2(ml_interface, ml_device);
    CAMLlocal1(ml_pci_info);
    nvmlReturn_t error;
    nvmlInterface* interface;
    nvmlPciInfo_t pci_info;
    nvmlDevice_t device;

    interface = (nvmlInterface*)ml_interface;
    device = *(nvmlDevice_t*)ml_device;
    error =
        interface->deviceGetPciInfo(device, &pci_info);
    check_error(interface, error);

    ml_pci_info = caml_alloc(6, 0);
    Store_field(ml_pci_info, 0, caml_copy_string(pci_info.busId));
    Store_field(ml_pci_info, 1, Val_int(pci_info.domain));
    Store_field(ml_pci_info, 2, Val_int(pci_info.bus));
    Store_field(ml_pci_info, 3, Val_int(pci_info.device));
    Store_field(ml_pci_info, 4, Val_int(pci_info.pciDeviceId));
    Store_field(ml_pci_info, 5, Val_int(pci_info.pciSubSystemId));

    CAMLreturn(ml_pci_info);
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

CAMLprim value stub_nvml_device_get_utilization_rates(
        value ml_interface,
        value ml_device) {
    CAMLparam2(ml_interface, ml_device);
    CAMLlocal1(ml_utilization);
    nvmlReturn_t error;
    nvmlInterface* interface;
    nvmlDevice_t device;
    nvmlUtilization_t utilization;

    interface = (nvmlInterface*)ml_interface;
    device = *(nvmlDevice_t*)ml_device;
    error = interface->deviceGetUtilizationRates(device, &utilization);
    check_error(interface, error);

    ml_utilization = caml_alloc(2, 0);
    Store_field(ml_utilization, 0, utilization.gpu);
    Store_field(ml_utilization, 1, utilization.memory);

    CAMLreturn(ml_utilization);
}
