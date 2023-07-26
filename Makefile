# The command to remove a file.
RM = cmake -E remove_directory

build:CMakeLists.txt
	cmake -S . -B build -G "Unix Makefiles" $(cmake_definition)
	cd build && make

test:build
	./build/build_out/target/test

memcheck:build
	valgrind --leak-check=full --show-leak-kinds=all ./build/build_out/target/test

clean:
	$(RM) build

.PHONY:build clean test memcheck