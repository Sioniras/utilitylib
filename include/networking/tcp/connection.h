/////////////////////////////////////////////////////////////////////////
// TCP connection definition
//
// Represents an active connection socket (not a listening socket).
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <networking/socket.h>
#include <networking/address.h>

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

	std::ostream& operator<<(std::ostream&, connection::status state);
}
