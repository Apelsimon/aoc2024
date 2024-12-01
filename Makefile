config=Release
day=1

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
	./build/day$(day)/$(config)/aoc input.txt