BUILD_CONTAINER=rpi-pico-build

.PHONY: build clean

build:
	docker run --rm -v $(PWD):/target/$(notdir $(PWD)) $(BUILD_CONTAINER)

clean:
	rm -rf build
