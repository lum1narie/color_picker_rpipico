BUILD_CONTAINER=rpi-pico-build

BUILD_DIRECTORY=build
# TARGET=$(BUILD_DIRECTORY)/color_picker.uf2

# SOURCES := $(shell git ls-files -cmo --deduplicate --exclude-standard | grep -vx "$$(git ls-files -d)" | grep -E '\.c(pp)?$$')
# HEADERS := $(shell git ls-files -cmo --deduplicate --exclude-standard | grep -vx "$$(git ls-files -d)" | grep -E '\.h(pp)?$$')

.PHONY: build fmt clean

build:
	docker run --rm -t -v $(PWD):/target/$(notdir $(PWD)) $(BUILD_CONTAINER)
	make compile_commands.json

.PHONY: compile_commands.json
compile_commands.json:
	sed -i -e 's|/target/|$(dir $(realpath $(PWD)))|g' build/compile_commands.json
	-rm -f compile_commands.json
	ln -s build/compile_commands.json .

fmt:
	$(eval FMT_TARGET := $(shell find -regex \
		'\./\([^./][^/]*/\)*[^./][^/]*\.\(c\|h\)\(pp\)?' \
		-not -regex '\./\(build\|LCD1in8\)/.*'))
	clang-format -i $(FMT_TARGET)

clean:
	rm -rf build compile_commands.json
