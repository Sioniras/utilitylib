/////////////////////////////////////////////////////////////////////////
// General networking definitions
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>

// Include platform-specific definitions
#include <networking/posix/socket_definitions.h>
#include <networking/win32/socket_definitions.h>

namespace networking
{
	using port_number_t = uint16_t;

	enum class protocol
	{
		tcp,
		udp,
		any,
	};

	enum class ip_version
	{
		ipv4,
		ipv6,
		any,
	};

	struct result_string
	{
		int code;	// Zero for success, non-zero for an error
		std::string value;
	};

	class socket;
	class address;
}
