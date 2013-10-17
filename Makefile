COMPONENTS=basic_dlopen get_device_info ml_dlopen

.PHONY: all clean test

all:
	$(foreach COMPONENT, $(COMPONENTS), $(MAKE) -C $(COMPONENT);)

clean:
	$(foreach COMPONENT, $(COMPONENTS), $(MAKE) -C $(COMPONENT) clean;)

test:
	@$(foreach COMPONENT, $(COMPONENTS), \
		echo -e "---------------------------------------------------" && \
		echo -e "Running program: $(COMPONENT)" && \
		echo -e "---------------------------------------------------" && \
		./$(COMPONENT)/$(COMPONENT) &&) :
