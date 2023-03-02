BUILD_CONTAINER=rpi-pico-build

.PHONY: build clean

build:
	docker run --rm -t -v $(PWD):/target/$(notdir $(PWD)) $(BUILD_CONTAINER)
	sed -i -e 's|/target/|$(dir $(realpath $(PWD)))|g' build/compile_commands.json
	-rm -f compile_commands.json
	ln -s build/compile_commands.json .

clean:
	rm -rf build compile_commands.json
