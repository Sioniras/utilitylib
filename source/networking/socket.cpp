/////////////////////////////////////////////////////////////////////////
// Generic socket implementation
/////////////////////////////////////////////////////////////////////////
#include <networking/socket.h>

#include <stdexcept>

namespace networking
{
	// ----------------------------------------------------------------------
	// Constructors / destructor
	// ----------------------------------------------------------------------
	// Constructor
	socket::socket() : _socket(uninitialized_socket)
	{
	}

	// Constructor
	socket::socket(protocol p, ip_version ipv) : _socket(uninitialized_socket)
	{
		auto inet_protocol = (ipv == ip_version::ipv6 ? AF_INET6 : AF_INET);
		auto protocol_type = (p == protocol::tcp ? SOCK_STREAM : SOCK_DGRAM);
		auto specific_protocol = 0;
		auto s = ::socket(inet_protocol, protocol_type, specific_protocol);

		if(is_valid_socket(s))
			_socket = s;
	}

	// Constructor
	socket::socket(socket_type s) : _socket(s)
	{
	}

	// Destructor
	socket::~socket()
	{
		if(valid())
			close_socket(_socket);
	}

	// ----------------------------------------------------------------------
	// Move construction / assignment
	// ----------------------------------------------------------------------
	// Move-construction
	socket::socket(socket&& s) : _socket(uninitialized_socket)
	{
		// Make a proper swap such that two socket objects never hold the same socket
		auto temp = s._socket;
		s._socket = uninitialized_socket;
		_socket = temp;
	}

	// Move-assignment
	socket& socket::operator=(socket&& s)
	{
		// Make a proper swap such that two socket objects never hold the same socket
		auto temp = s._socket;
		s._socket = uninitialized_socket;
		_socket = temp;

		return *this;
	}

	// ----------------------------------------------------------------------
	// Public interface
	// ----------------------------------------------------------------------
	void socket::close()
	{
		close_socket(_socket);
		_socket = uninitialized_socket;
	}
}
