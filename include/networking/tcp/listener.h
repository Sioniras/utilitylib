/////////////////////////////////////////////////////////////////////////
// TCP listener definition
//
// A listening socket used for accepting incoming connections.
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <networking/socket.h>
#include <networking/address.h>

namespace networking::tcp
{
	class connection;

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

	std::ostream& operator<<(std::ostream&, listener::status state);
}
