# The command to remove a file.
RM = cmake -E remove_directory

build:CMakeLists.txt
	cmake -S . -B build -G "Unix Makefiles" $(cmake_definition)
	cd build && make

clean:
	$(RM) build

.PHONY:build clean