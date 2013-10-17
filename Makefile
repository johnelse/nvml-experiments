COMPONENTS=basic_dlopen get_device_info

.PHONY: all clean

all:
	$(foreach COMPONENT, $(COMPONENTS), $(MAKE) -C $(COMPONENT);)

clean:
	$(foreach COMPONENT, $(COMPONENTS), $(MAKE) -C $(COMPONENT) clean;)
