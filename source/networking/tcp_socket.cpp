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

	// ----------------------------------------------------------------------
	// Non-member non-friend functions
	// ----------------------------------------------------------------------
}
