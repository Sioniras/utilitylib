/////////////////////////////////////////////////////////////////////////
// UDP socket implementation
/////////////////////////////////////////////////////////////////////////
#include <networking/udp_socket.h>

namespace networking::udp
{
	// ----------------------------------------------------------------------
	// Constructors / destructor
	// ----------------------------------------------------------------------
	// Constructor
	socket::socket() : _socket(protocol::udp, ip_version::any)
	{
	}

	// Destructor
	socket::~socket()
	{
	}

	// ----------------------------------------------------------------------
	// Move construction / assignment
	// ----------------------------------------------------------------------
	// Move-construction
	socket::socket(socket&& s) : _socket(std::move(s._socket))
	{
	}

	// Move-assignment
	socket& socket::operator=(socket&& s)
	{
		_socket = std::move(s._socket);
		return *this;
	}

	// ----------------------------------------------------------------------
	// Public interface
	// ----------------------------------------------------------------------
}
