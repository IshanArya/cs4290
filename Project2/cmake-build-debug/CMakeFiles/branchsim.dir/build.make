# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

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


# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = "/Users/ishanarya/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/203.7148.70/CLion.app/Contents/bin/cmake/mac/bin/cmake"

# The command to remove a file.
RM = "/Users/ishanarya/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-0/203.7148.70/CLion.app/Contents/bin/cmake/mac/bin/cmake" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ishanarya/projects/cs4290/Project2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ishanarya/projects/cs4290/Project2/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/branchsim.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/branchsim.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/branchsim.dir/flags.make

CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.o: CMakeFiles/branchsim.dir/flags.make
CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.o: ../src/branchsim_driver.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ishanarya/projects/cs4290/Project2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.o -c /Users/ishanarya/projects/cs4290/Project2/src/branchsim_driver.cpp

CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ishanarya/projects/cs4290/Project2/src/branchsim_driver.cpp > CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.i

CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ishanarya/projects/cs4290/Project2/src/branchsim_driver.cpp -o CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.s

CMakeFiles/branchsim.dir/src/branchsim.cpp.o: CMakeFiles/branchsim.dir/flags.make
CMakeFiles/branchsim.dir/src/branchsim.cpp.o: ../src/branchsim.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/ishanarya/projects/cs4290/Project2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/branchsim.dir/src/branchsim.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/branchsim.dir/src/branchsim.cpp.o -c /Users/ishanarya/projects/cs4290/Project2/src/branchsim.cpp

CMakeFiles/branchsim.dir/src/branchsim.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/branchsim.dir/src/branchsim.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/ishanarya/projects/cs4290/Project2/src/branchsim.cpp > CMakeFiles/branchsim.dir/src/branchsim.cpp.i

CMakeFiles/branchsim.dir/src/branchsim.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/branchsim.dir/src/branchsim.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/ishanarya/projects/cs4290/Project2/src/branchsim.cpp -o CMakeFiles/branchsim.dir/src/branchsim.cpp.s

# Object files for target branchsim
branchsim_OBJECTS = \
"CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.o" \
"CMakeFiles/branchsim.dir/src/branchsim.cpp.o"

# External object files for target branchsim
branchsim_EXTERNAL_OBJECTS =

branchsim: CMakeFiles/branchsim.dir/src/branchsim_driver.cpp.o
branchsim: CMakeFiles/branchsim.dir/src/branchsim.cpp.o
branchsim: CMakeFiles/branchsim.dir/build.make
branchsim: CMakeFiles/branchsim.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/ishanarya/projects/cs4290/Project2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable branchsim"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/branchsim.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/branchsim.dir/build: branchsim

.PHONY : CMakeFiles/branchsim.dir/build

CMakeFiles/branchsim.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/branchsim.dir/cmake_clean.cmake
.PHONY : CMakeFiles/branchsim.dir/clean

CMakeFiles/branchsim.dir/depend:
	cd /Users/ishanarya/projects/cs4290/Project2/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ishanarya/projects/cs4290/Project2 /Users/ishanarya/projects/cs4290/Project2 /Users/ishanarya/projects/cs4290/Project2/cmake-build-debug /Users/ishanarya/projects/cs4290/Project2/cmake-build-debug /Users/ishanarya/projects/cs4290/Project2/cmake-build-debug/CMakeFiles/branchsim.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/branchsim.dir/depend

