module Nvml = struct
	type interface

	type device

	external library_open: unit -> interface = "stub_nvml_open"
	external library_close: interface -> unit = "stub_nvml_close"

	external init: interface -> unit = "stub_nvml_init"
	external shutdown: interface -> unit = "stub_nvml_shutdown"

	external device_get_count: interface -> int = "stub_nvml_device_get_count"
	external device_get_handle_by_index: interface -> int -> device =
		"stub_nvml_device_get_handle_by_index"
end

let () =
	let interface = Nvml.library_open () in
	Nvml.init interface;
	Nvml.shutdown interface;
	Nvml.library_close interface
