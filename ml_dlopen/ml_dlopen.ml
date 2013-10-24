module Nvml = struct
	exception Library_not_loaded of string
	exception Symbol_not_loaded of string

	type interface

	type device

	type pci_info = {
		bus_id: string;
		domain: int;
		bus: int;
		device: int;
		pci_device_id: int;
		pci_subsystem_id: int;
	}

	type utilization = {
		gpu: int;
		memory: int;
	}

	external library_open: unit -> interface = "stub_nvml_open"
	let library_open () =
		Callback.register_exception "Library_not_loaded" (Library_not_loaded "");
		Callback.register_exception "Symbol_not_loaded" (Symbol_not_loaded "");
		library_open ()

	external library_close: interface -> unit = "stub_nvml_close"

	external init: interface -> unit = "stub_nvml_init"
	external shutdown: interface -> unit = "stub_nvml_shutdown"

	external device_get_count: interface -> int = "stub_nvml_device_get_count"
	external device_get_handle_by_index: interface -> int -> device =
		"stub_nvml_device_get_handle_by_index"
	external device_get_pci_info: interface -> device -> pci_info =
		"stub_nvml_device_get_pci_info"
	external device_get_power_usage: interface -> device -> int =
		"stub_nvml_device_get_power_usage"
	external device_get_temperature: interface -> device -> int =
		"stub_nvml_device_get_temperature"
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
		let pci_info = Nvml.device_get_pci_info interface device in
		let temp = Nvml.device_get_temperature interface device in
		Printf.printf "bus ID = %s\n" pci_info.Nvml.bus_id;
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
