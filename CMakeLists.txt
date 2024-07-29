# Set cmake version requirement
cmake_minimum_required(VERSION 3.14)

project(utilitylib)

# Compiler options
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "-pthread")

if (WIN32)
add_definitions(-DUSE_WINSOCK2)
else()
add_definitions(-DUSE_POSIX)
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/source")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

include_directories("$ENV{LIBRARIES_PATH}/gtest/include")
link_directories("$ENV{LIBRARIES_PATH}/gtest/lib")

option(BUILD_TARGET_EXE "Build the executable used during development." ON)

# -------------------------------------------------
# Sources for library target
# -------------------------------------------------

# General includes
set(SOURCES_TARGET_LIBRARY
	# Networking module - POSIX platform
	source/networking/posix/socket_definitions.h

	# Networking module - Win32 platform
	source/networking/win32/socket_definitions.h

	# Networking module
	include/networking/networking.h
	include/networking/socket.h
	source/networking/socket.cpp
	include/networking/address.h
	source/networking/address.cpp

	# Networking/TCP
	include/networking/tcp/tcp.h
	include/networking/tcp/connection.h
	source/networking/tcp/connection.cpp
	include/networking/tcp/listener.h
	source/networking/tcp/listener.cpp
	include/networking/tcp/connection_manager.h
	source/networking/tcp/connection_manager.cpp

	# Networking/UDP
	include/networking/udp_socket.h
	source/networking/udp_socket.cpp

	# Data structures
	include/containers/circular_buffer.h
	include/containers/safe_queue.h
)

# -------------------------------------------------
# Sources for executable target
# -------------------------------------------------
set(SOURCES_TARGET_EXE
	# Main entry point
	source/main.cpp
)

# -------------------------------------------------
# Tests
# -------------------------------------------------
set(SOURCES_TARGET_TESTS
	tests/test_main.cpp
	tests/address.cpp
)

# -------------------------------------------------
# Build targets
# -------------------------------------------------
add_library(utilities STATIC ${SOURCES_TARGET_LIBRARY})
add_executable(utilitydev ${SOURCES_TARGET_EXE})
target_link_libraries(utilitydev utilities)

# The tests
add_executable(tests ${SOURCES_TARGET_TESTS})
target_link_libraries(tests gtest utilities)

