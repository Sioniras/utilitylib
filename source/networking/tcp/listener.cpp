/////////////////////////////////////////////////////////////////////////
// TCP listener implementation
/////////////////////////////////////////////////////////////////////////
#include <networking/tcp/listener.h>
#include <networking/tcp/connection.h>

namespace networking::tcp
{
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

	// Accepts a connection
	// Note: This is a BLOCKING function!
	bool listener::accept()
	{
		struct sockaddr_storage client;
		socklen_t length = sizeof(client);
		socket_type client_socket = ::accept(_socket.get(), reinterpret_cast<struct sockaddr*>(&client), &length);
		address client_address { *reinterpret_cast<sockaddr*>(&client), length, client.ss_family };

		if(is_valid_socket(client_socket))
		{
			tcp::connection client_connection { networking::socket(client_socket), client_address };
			_callback.on_new_connection(std::move(client_connection));
			return true;
		}

		return false;
	}

	// Accepts a connection
	// Note: This is a BLOCKING function!
	bool listener::poll_accept(uint16_t timeout_ms)
	{
		struct pollfd handle[1];
		handle[0].fd = _socket.get();
		handle[0].events = POLLIN;

		// Note: Use WSAPoll with Winsock?
		int number_of_events = poll(handle, 1, static_cast<int>(timeout_ms));
		if(number_of_events > 0 && (handle[0].revents & POLLIN))
		{
			struct sockaddr_storage client;
			socklen_t length = sizeof(client);
			socket_type client_socket = ::accept(_socket.get(), reinterpret_cast<struct sockaddr*>(&client), &length);
			address client_address { *reinterpret_cast<sockaddr*>(&client), length, client.ss_family };

			if(is_valid_socket(client_socket))
			{
				tcp::connection client_connection { networking::socket(client_socket), client_address };
				_callback.on_new_connection(std::move(client_connection));
			}
			else
			{
				auto error = get_error_information();
				tcp::connection client_connection { networking::socket(client_socket), client_address, tcp::connection::status::error, error };
				_callback.on_new_connection(std::move(client_connection));
			}

			return true;
		}

		return false;
	}

	// ----------------------------------------------------------------------
	// Non-member non-friend functions
	// ----------------------------------------------------------------------
	std::ostream& operator<<(std::ostream& s, listener::status state)
	{
		switch(state)
		{
			case listener::status::invalid: s << std::string("invalid");break;
			case listener::status::bound: s << std::string("bound");break;
			case listener::status::listening: s << std::string("listening");break;
			case listener::status::stopped: s << std::string("stopped");break;
			case listener::status::error: s << std::string("error");break;
			default: s << std::string("unknown");break;
		}

		return s;
	}
}
