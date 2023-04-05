.PHONY:run build
.SILENT:build

build:
	# $(MAKE) -C ./build && gdb -e run ./build/ctests
	@$(MAKE) -C ./build

run: build
	@./build/ctests

test: build
	@./build/ctests
