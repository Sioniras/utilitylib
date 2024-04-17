/////////////////////////////////////////////////////////////////////////
// Platform-specific socket definitions
/////////////////////////////////////////////////////////////////////////
#pragma once
#ifdef USE_WINSOCK2

#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

namespace networking
{
	// Win32 uses a specific socket type
	using socket_type = SOCKET;
	constexpr socket_type uninitialized_socket = INVALID_SOCKET;

	// Starts the socket API and returns true if successful
	bool socketapi_startup()
	{
		// This part may be expanded by actually using the WSADATA and
		// check possible error codes of WSAStartup.
		WSADATA data;
		return (WSAStartup(MAKEWORD(2,2), &data) == 0);
	}

	// The win32 socket API must be shut down explicitly
	void socketapi_shutdown()
	{
		WSACleanup();
	}

	// Basic socket functions
	bool is_valid_socket(socket_type s) { return (s == INVALID_SOCKET); }
	void close_socket(socket_type s) { closesocket(s); }

	struct socket_error_information
	{
		int error_code;
		std::string message;
	};

	// Returns error information
	socket_error_information get_error_information()
	{
		// Note: Error message can be retrieved using FormatMessage
		auto code = WSAGetLastError();
		return { code, "Error message not retrieved" };
	}
}

#endif
