config=Release
day=1
full_day=day$(day)

.PHONY: default clean init build run

default:
	clean init build

clean:
	rm -rf build

init:
	mkdir -p build

build:
	cd build; \
	cmake ..; \
	cmake --build . --config $(config)

run:
	./build/$(full_day)/$(config)/$(full_day) input.txt