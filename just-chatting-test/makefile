GRPC_PREFIX := $(HOME)/grpc-install
BUILD_DIR   := build

.PHONY: build clean configure

build: clean configure
	cmake --build $(BUILD_DIR) -j2

configure:
	LD_LIBRARY_PATH=$(GRPC_PREFIX)/lib:$$LD_LIBRARY_PATH \
	cmake -S . -B $(BUILD_DIR) \
	  -DCMAKE_PREFIX_PATH=$(GRPC_PREFIX) \
	  -DCMAKE_BUILD_TYPE=Release

clean:
	rm -rf $(BUILD_DIR)
