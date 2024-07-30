/////////////////////////////////////////////////////////////////////////
// UDP forward declarations / definitions
/////////////////////////////////////////////////////////////////////////
#pragma once

namespace networking::udp
{
	// Class prototypes
	class socket;

	// Status enum class for UDP sockets
	enum class socket_status
	{
		invalid,
		open,		// Open, not bound
		bound,		// Open, called bind
		error,		// Failed to bind
		closed,	// Socket closed
	};
}
