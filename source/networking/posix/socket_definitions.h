/////////////////////////////////////////////////////////////////////////
// Platform-specific socket definitions
/////////////////////////////////////////////////////////////////////////
#pragma once
#ifdef USE_POSIX

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include <cerrno>
#include <poll.h>

namespace networking
{
	// POSIX sockets are just an integer (file descriptor)
	using socket_type = int;
	constexpr socket_type uninitialized_socket = -1;

	// Explicit startup/shutdown of the socket API is not needed for POSIX platforms
	constexpr bool socketapi_startup() { return true; }
	constexpr void socketapi_shutdown() {}

	// Basic socket functions
	inline bool is_valid_socket(socket_type s) { return (s >= 0); }
	inline void close_socket(socket_type s) { close(s); }

	struct socket_error_information
	{
		int error_code;
		std::string message;
	};

	// Returns error information
	inline socket_error_information get_error_information()
	{
		auto code = errno;
		return { code, strerror(code) };
	}
}

#endif
