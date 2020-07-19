BUILD_TYPE=debug
BUILD_PATH=cmake-build-$(BUILD_TYPE)
CMAKE=cmake

build:
	$(CMAKE) -B $(BUILD_PATH) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	$(CMAKE) --build $(BUILD_PATH) --target cmake-project -- -j

run:
	$(BUILD_PATH)/cmake-project

release:
	CMAKE BUILD_TYPE=release

clean:
	@rm -rf $(BUILD_PATH)

