.PHONY:run build
.SILENT:build

build:
	# $(MAKE) -C ./build && gdb -e run ./build/cmonkey
	@$(MAKE) -C ./build

run: build
	@./build/cmonkey

test: build
	@./build/ctests
