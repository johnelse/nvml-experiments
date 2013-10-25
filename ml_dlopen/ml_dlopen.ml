module Nvml = struct
	exception Library_not_loaded of string
	exception Symbol_not_loaded of string

	type interface

	type device

	type memory_info = {
		total: int64;
		free: int64;
		used: int64;
	}

	type pci_info = {
		bus_id: string;
		domain: int32;
		bus: int32;
		device: int32;
		pci_device_id: int32;
		pci_subsystem_id: int32;
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
	external device_get_memory_info: interface -> device -> memory_info =
		"stub_nvml_device_get_memory_info"
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
		let memory_info = Nvml.device_get_memory_info interface device in
		let pci_info = Nvml.device_get_pci_info interface device in
		let temp = Nvml.device_get_temperature interface device in
		Printf.printf "total memory = %Ld\n" memory_info.Nvml.total;
		Printf.printf "free memory = %Ld\n" memory_info.Nvml.free;
		Printf.printf "used memory = %Ld\n" memory_info.Nvml.used;
		Printf.printf "bus ID = %s\n" pci_info.Nvml.bus_id;
		Printf.printf "domain = 0x%04lx\n" pci_info.Nvml.domain;
		Printf.printf "bus = 0x%02lx\n" pci_info.Nvml.bus;
		Printf.printf "device = %ld\n" pci_info.Nvml.device;
		Printf.printf "PCI device ID = 0x%08lx\n" pci_info.Nvml.pci_device_id;
		Printf.printf "PCI subsystem ID = 0x%08lx\n" pci_info.Nvml.pci_subsystem_id;
		Printf.printf "temperature = %d°C\n" temp;
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
