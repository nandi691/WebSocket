# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /home/admin-009/Desktop/WebSocket

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/admin-009/Desktop/WebSocket/build

# Include any dependencies generated for this target.
include CMakeFiles/WebSocket.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/WebSocket.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/WebSocket.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/WebSocket.dir/flags.make

CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.o: CMakeFiles/WebSocket.dir/flags.make
CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.o: ../src/Common/ProcessUtils.c
CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.o: CMakeFiles/WebSocket.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/admin-009/Desktop/WebSocket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.o -MF CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.o.d -o CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.o -c /home/admin-009/Desktop/WebSocket/src/Common/ProcessUtils.c

CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/admin-009/Desktop/WebSocket/src/Common/ProcessUtils.c > CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.i

CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/admin-009/Desktop/WebSocket/src/Common/ProcessUtils.c -o CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.s

CMakeFiles/WebSocket.dir/src/Main.c.o: CMakeFiles/WebSocket.dir/flags.make
CMakeFiles/WebSocket.dir/src/Main.c.o: ../src/Main.c
CMakeFiles/WebSocket.dir/src/Main.c.o: CMakeFiles/WebSocket.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/admin-009/Desktop/WebSocket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/WebSocket.dir/src/Main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/WebSocket.dir/src/Main.c.o -MF CMakeFiles/WebSocket.dir/src/Main.c.o.d -o CMakeFiles/WebSocket.dir/src/Main.c.o -c /home/admin-009/Desktop/WebSocket/src/Main.c

CMakeFiles/WebSocket.dir/src/Main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/WebSocket.dir/src/Main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/admin-009/Desktop/WebSocket/src/Main.c > CMakeFiles/WebSocket.dir/src/Main.c.i

CMakeFiles/WebSocket.dir/src/Main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/WebSocket.dir/src/Main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/admin-009/Desktop/WebSocket/src/Main.c -o CMakeFiles/WebSocket.dir/src/Main.c.s

# Object files for target WebSocket
WebSocket_OBJECTS = \
"CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.o" \
"CMakeFiles/WebSocket.dir/src/Main.c.o"

# External object files for target WebSocket
WebSocket_EXTERNAL_OBJECTS =

../exe/WebSocket: CMakeFiles/WebSocket.dir/src/Common/ProcessUtils.c.o
../exe/WebSocket: CMakeFiles/WebSocket.dir/src/Main.c.o
../exe/WebSocket: CMakeFiles/WebSocket.dir/build.make
../exe/WebSocket: CMakeFiles/WebSocket.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/admin-009/Desktop/WebSocket/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable ../exe/WebSocket"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/WebSocket.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/WebSocket.dir/build: ../exe/WebSocket
.PHONY : CMakeFiles/WebSocket.dir/build

CMakeFiles/WebSocket.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/WebSocket.dir/cmake_clean.cmake
.PHONY : CMakeFiles/WebSocket.dir/clean

CMakeFiles/WebSocket.dir/depend:
	cd /home/admin-009/Desktop/WebSocket/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/admin-009/Desktop/WebSocket /home/admin-009/Desktop/WebSocket /home/admin-009/Desktop/WebSocket/build /home/admin-009/Desktop/WebSocket/build /home/admin-009/Desktop/WebSocket/build/CMakeFiles/WebSocket.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/WebSocket.dir/depend
