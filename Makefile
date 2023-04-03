run: 
	# $(MAKE) -C ./build && gdb -e run ./build/cmonkey
	$(MAKE) -C ./build && ./build/cmonkey
