/////////////////////////////////////////////////////////////////////////
// TCP connection implementation
/////////////////////////////////////////////////////////////////////////
#include <networking/tcp_socket.h>

namespace networking::tcp
{
	// ----------------------------------------------------------------------
	// Constructors / destructor
	// ----------------------------------------------------------------------
	// Constructor
	connection::connection(const networking::address& target) :
		_socket(protocol::tcp, target.ip_version_value()),
		_status(status::invalid),
		_error({0, "No error"})
	{
		if(_socket.valid())
		{
			auto result = ::connect(_socket.get(), &(target.get()), target.length());
			if(result == 0)
			{
				_status = status::open;
			}
			else
			{
				_error = get_error_information();
				_status = status::error;
			}
		}
	}

	// Destructor
	connection::~connection()
	{
	}

	// ----------------------------------------------------------------------
	// Move construction / assignment
	// ----------------------------------------------------------------------
	// Move-construction
	connection::connection(connection&& s) : _socket(std::move(s._socket))
	{
	}

	// Move-assignment
	connection& connection::operator=(connection&& s)
	{
		_socket = std::move(s._socket);
		return *this;
	}

	// ----------------------------------------------------------------------
	// Public interface
	// ----------------------------------------------------------------------
	// Shutdown further communication, but not closing the socket (there may still be data to process in send/receive buffers)
	// Note: Call shutdown and make sure that buffers become empty before closing.
	void connection::shutdown()
	{
		auto how = SHUT_RDWR;
		::shutdown(_socket.get(), how);
		_status = status::shutdown;
	}

	// Closes the socket, terminating the connection. Any data left in send/receive buffers is discarded.
	void connection::close()
	{
		_socket.close();
		_status = status::closed;
	}

	// ----------------------------------------------------------------------
	// Non-member non-friend functions
	// ----------------------------------------------------------------------

	// --- Listener ---
	// ----------------------------------------------------------------------
	// Constructors / destructor
	// ----------------------------------------------------------------------
	listener::listener(incoming_connection_callback& callback, port_number_t port, bool use_ipv6) :
		_socket(),
		_status(status::invalid),
		_error({0, "No error"}),
		_callback(callback),
		_address()
	{
		_address = create_host_address(port, protocol::tcp, use_ipv6 ? ip_version::ipv6 : ip_version::ipv4);
		networking::socket s { protocol::tcp, _address.ip_version_value() };
		_socket = std::move(s);

		auto result = ::bind(_socket.get(), &(_address.get()), _address.length());
		if(result == 0)
		{
			_status = status::bound;
		}
		else
		{
			_error = get_error_information();
			_status = status::error;
		}
	}

	// Destructor
	listener::~listener()
	{
		stop();
	}

	// ----------------------------------------------------------------------
	// Public interface
	// ----------------------------------------------------------------------
	// Start listening on the bound address
	// queue_length: Size of queue for connections to accept.
	bool listener::start(uint16_t queue_length)
	{
		auto result = ::listen(_socket.get(), static_cast<int>(queue_length));
		if(result == 0)
		{
			_status = status::listening;
			return true;
		}
		else
		{
			_error = get_error_information();
			_status = status::error;
		}
		return false;
	}

	// Stops listening for connections
	void listener::stop()
	{
		if(_status == status::bound || _status == status::listening)
		{
			_socket.close();
			_status = status::stopped;
		}
	}
}
