module Nvml = struct
	type interface

	external library_open: unit -> interface = "stub_nvml_open"
	external library_close: interface -> unit = "stub_nvml_close"

	external init: interface -> unit = "stub_nvml_init"
	external shutdown: interface -> unit = "stub_nvml_shutdown"
end

let () =
	let interface = Nvml.library_open () in
	Nvml.init interface;
	Nvml.shutdown interface;
	Nvml.library_close interface
