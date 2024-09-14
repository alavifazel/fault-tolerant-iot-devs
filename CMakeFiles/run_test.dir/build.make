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
CMAKE_SOURCE_DIR = /home/afx/code/HELL/esp32/pub_sub/simulation_code

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/afx/code/HELL/esp32/pub_sub/simulation_code

# Utility rule file for run_test.

# Include any custom commands dependencies for this target.
include CMakeFiles/run_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/run_test.dir/progress.make

CMakeFiles/run_test:
	./bin/test_generator input_data/generator_test_input.txt
	./bin/test_server input_data/server_test_input.txt
	./bin/test_transducer input_data/transducer_test_input_generator.txt input_data transducer_test_input_processed.txt > /dev/null
	./bin/test_balancer input_data/balancer_test_input.txt
	echo Done.\ Visit\ the\ log\ files.

run_test: CMakeFiles/run_test
run_test: CMakeFiles/run_test.dir/build.make
.PHONY : run_test

# Rule to build all files generated by this target.
CMakeFiles/run_test.dir/build: run_test
.PHONY : CMakeFiles/run_test.dir/build

CMakeFiles/run_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/run_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/run_test.dir/clean

CMakeFiles/run_test.dir/depend:
	cd /home/afx/code/HELL/esp32/pub_sub/simulation_code && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/afx/code/HELL/esp32/pub_sub/simulation_code /home/afx/code/HELL/esp32/pub_sub/simulation_code /home/afx/code/HELL/esp32/pub_sub/simulation_code /home/afx/code/HELL/esp32/pub_sub/simulation_code /home/afx/code/HELL/esp32/pub_sub/simulation_code/CMakeFiles/run_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/run_test.dir/depend

