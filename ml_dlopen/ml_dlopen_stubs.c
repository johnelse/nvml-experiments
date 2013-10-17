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
} nvmlInterface;

CAMLprim value stub_nvml_open(value unit)
{
    CAMLparam1(unit);
    CAMLlocal1(ml_interface);

    void *handle;
    nvmlInterface *interface;
    interface = malloc(sizeof(nvmlInterface));

    handle = dlopen("libnvidia-ml.so.1", RTLD_LAZY);
    if (!handle) {
        goto Error;
    }
    interface->handle = handle;
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
