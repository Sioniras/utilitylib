/////////////////////////////////////////////////////////////////////////
// TCP connection implementation
/////////////////////////////////////////////////////////////////////////
#include <networking/tcp/connection.h>

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
	std::ostream& operator<<(std::ostream& s, connection::status state)
	{
		switch(state)
		{
			case connection::status::invalid: s << std::string("invalid");break;
			case connection::status::open: s << std::string("open");break;
			case connection::status::closed: s << std::string("closed");break;
			case connection::status::shutdown: s << std::string("shutdown");break;
			case connection::status::error: s << std::string("error");break;
			default: s << std::string("unknown");break;
		}

		return s;
	}
}