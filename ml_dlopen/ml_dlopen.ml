module Nvml = struct
	exception Library_not_loaded of string
	exception Symbol_not_loaded of string

	let () =
		Callback.register_exception "Library_not_loaded" (Library_not_loaded "")
	let () =
		Callback.register_exception "Symbol_not_loaded" (Symbol_not_loaded "")

	type interface

	type device

	type utilization = {
		gpu: int;
		memory: int;
	}

	external library_open: unit -> interface = "stub_nvml_open"
	external library_close: interface -> unit = "stub_nvml_close"

	external init: interface -> unit = "stub_nvml_init"
	external shutdown: interface -> unit = "stub_nvml_shutdown"

	external device_get_count: interface -> int = "stub_nvml_device_get_count"
	external device_get_handle_by_index: interface -> int -> device =
		"stub_nvml_device_get_handle_by_index"
	external device_get_temperature: interface -> device -> int =
		"stub_nvml_device_get_temperature"
	external device_get_power_usage: interface -> device -> int =
		"stub_nvml_device_get_power_usage"
	external device_get_utilization_rates: interface -> device -> utilization =
		"stub_nvml_device_get_utilization_rates"
end

let () =
	let interface = Nvml.library_open () in
	Nvml.init interface;
	let count = Nvml.device_get_count interface in
	for index = 0 to (count - 1) do
		Printf.printf "----- Device %d -----\n" index;
		let device = Nvml.device_get_handle_by_index interface index in
		let temp = Nvml.device_get_temperature interface device in
		Printf.printf "temperature = %dC\n" temp;
		begin
			try
				let power_usage = Nvml.device_get_power_usage interface device in
				Printf.printf "power usage = %dmW\n" power_usage
			with _ ->
				Printf.printf "power usage lookup not supported\n"
		end;
		begin
			try
				let utilization = Nvml.device_get_utilization_rates interface device in
				Printf.printf
					"utilization = %d%% GPU, %d%% memory\n"
					utilization.Nvml.gpu
					utilization.Nvml.memory
			with _ ->
				Printf.printf "utilization lookup not supported\n"
		end
	done;
	Nvml.shutdown interface;
	Nvml.library_close interface
