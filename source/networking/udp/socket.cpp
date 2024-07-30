/////////////////////////////////////////////////////////////////////////
// UDP socket implementation
/////////////////////////////////////////////////////////////////////////
#include <networking/udp/socket.h>

namespace networking::udp
{
	// ----------------------------------------------------------------------
	// Constructors / destructor
	// ----------------------------------------------------------------------
	// Constructor
	socket::socket() :
		_socket(protocol::udp, ip_version::any),
		_boundAddress(),
		_status(status::invalid),
		_error({0, "No error"})
	{
		if(_socket.valid())
			_status = status::open;
	}

	// Destructor
	socket::~socket()
	{
	}

	// ----------------------------------------------------------------------
	// Move construction / assignment
	// ----------------------------------------------------------------------
	// Move-construction
	socket::socket(socket&& s) :
		_socket(std::move(s._socket)),
		_boundAddress(std::move(s._boundAddress)),
		_status(std::move(s._status)),
		_error(std::move(s._error))
	{
	}

	// Move-assignment
	socket& socket::operator=(socket&& s)
	{
		_socket = std::move(s._socket);
		_boundAddress = std::move(s._boundAddress);
		_status = std::move(s._status);
		_error = std::move(s._error);

		return *this;
	}

	// ----------------------------------------------------------------------
	// Public interface
	// ----------------------------------------------------------------------
	// Bind to port
	bool socket::bind(const networking::address &target)
	{
		auto result = ::bind(_socket.get(), &(target.get()), target.length());
		if(result == 0)
		{
			_status = status::bound;
		}
		else
		{
			_error = get_error_information();
			_status = status::error;
			return false;
		}

		return true;
	}

	// Closes the socket
	void socket::close()
	{
		_socket.close();
		_status = status::closed;
	}

	// Receive data from connection
	ssize_t socket::receive_from(uint8_t* buffer, std::size_t buffer_size, networking::address& target) const
	{
		struct sockaddr_storage client;
		socklen_t length = sizeof(client);
		auto result = ::recvfrom(_socket.get(), buffer, buffer_size, 0, reinterpret_cast<struct sockaddr*>(&client), &length);
		target = networking::address { *reinterpret_cast<sockaddr*>(&client), length, client.ss_family };

		// Returns -1 on error, otherwise number of bytes received
		return result;
	}

	// Send data through connection
	ssize_t socket::send_to(const uint8_t* buffer, std::size_t number_of_elements_to_send, const networking::address& target) const
	{
		// Returns -1 on error, otherwise number of bytes sent
		return ::sendto(_socket.get(), buffer, number_of_elements_to_send, 0, &(target.get()), target.length());
	}
}
