# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/oschalge.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/oschalge.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/oschalge.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/oschalge.dir/flags.make

CMakeFiles/oschalge.dir/server.c.o: CMakeFiles/oschalge.dir/flags.make
CMakeFiles/oschalge.dir/server.c.o: /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge/server.c
CMakeFiles/oschalge.dir/server.c.o: CMakeFiles/oschalge.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/oschalge.dir/server.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/oschalge.dir/server.c.o -MF CMakeFiles/oschalge.dir/server.c.o.d -o CMakeFiles/oschalge.dir/server.c.o -c /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge/server.c

CMakeFiles/oschalge.dir/server.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/oschalge.dir/server.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge/server.c > CMakeFiles/oschalge.dir/server.c.i

CMakeFiles/oschalge.dir/server.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/oschalge.dir/server.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge/server.c -o CMakeFiles/oschalge.dir/server.c.s

# Object files for target oschalge
oschalge_OBJECTS = \
"CMakeFiles/oschalge.dir/server.c.o"

# External object files for target oschalge
oschalge_EXTERNAL_OBJECTS =

oschalge: CMakeFiles/oschalge.dir/server.c.o
oschalge: CMakeFiles/oschalge.dir/build.make
oschalge: CMakeFiles/oschalge.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable oschalge"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/oschalge.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/oschalge.dir/build: oschalge
.PHONY : CMakeFiles/oschalge.dir/build

CMakeFiles/oschalge.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/oschalge.dir/cmake_clean.cmake
.PHONY : CMakeFiles/oschalge.dir/clean

CMakeFiles/oschalge.dir/depend:
	cd /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge/cmake-build-debug /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge/cmake-build-debug /mnt/d/thiba/Music/os-challenge-common/os-challenge-holycow/src/oschalge/cmake-build-debug/CMakeFiles/oschalge.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/oschalge.dir/depend
