/////////////////////////////////////////////////////////////////////////
// TCP connection definition
//
// Represents an active connection socket (not a listening socket).
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <networking/socket.h>

namespace networking::tcp
{
	class connection
	{
		public:
			enum class status
			{
				invalid,
				open,
				shutdown,
				closed,
				error,
			};

		public:
			// Constructors / destructor
			connection(const networking::address& target);
			~connection();

			// Disallow copying
			connection(const connection&) = delete;
			connection& operator=(const connection&) = delete;

			// Move construction/assignment
			connection(connection&&);
			connection& operator=(connection&&);

			// Public interface
			void shutdown();
			void close();
			status state() const { return _status; }
			const networking::socket& socket() const { return _socket; }
			const socket_error_information& error() const { return _error; }

		private:
			networking::socket _socket;
			status _status;
			socket_error_information _error;
	};

	class incoming_connection_callback
	{
		public:
			virtual ~incoming_connection_callback() {}
			virtual void on_new_connection(connection&&) = 0;
	};

	class listener
	{
		public:
			enum class status
			{
				invalid,
				bound,		// Called bind, but not listen
				listening,	// Called bind and listen
				stopped,	// Socket closed
				error,
			};

		public:
			// Constructors / destructor
			listener(incoming_connection_callback&, port_number_t port, bool use_ipv6 = false);
			~listener();

			// Disallow copying
			listener(const listener&) = delete;
			listener& operator=(const listener&) = delete;

			// Move construction/assignment
			listener(listener&&);
			listener& operator=(listener&&);

			// Public interface
			bool start(uint16_t queue_length = 10);
			void stop();
			status state() const { return _status; }
			const networking::socket& socket() const { return _socket; }
			const socket_error_information& error() const { return _error; }
			const address& bound_address() const { return _address; }

		private:
			networking::socket _socket;
			status _status;
			socket_error_information _error;
			incoming_connection_callback& _callback;
			address _address;
	};

	inline std::ostream& operator<<(std::ostream& s, connection::status state)
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

	inline std::ostream& operator<<(std::ostream& s, listener::status state)
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
