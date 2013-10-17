module Nvml = struct
	type interface

	external library_open: unit -> interface = "stub_nvml_open"
	external library_close: interface -> unit = "stub_nvml_close"
end

let () =
	let interface = Nvml.library_open () in
	Nvml.library_close interface
