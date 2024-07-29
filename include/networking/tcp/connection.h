/////////////////////////////////////////////////////////////////////////
// TCP connection definition
//
// Represents an active connection socket (not a listening socket).
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <networking/socket.h>
#include <networking/address.h>
#include <networking/tcp/tcp.h>

namespace networking::tcp
{
	class connection
	{
		public:
			using status = connection_status;

		public:
			// Constructors / destructor
			connection(const networking::address& target);
			connection(networking::socket&&, const address&, status = status::open, const socket_error_information& = {0,"No error"});
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
			const address& connected_to() const { return _address; }
			const networking::socket& socket() const { return _socket; }
			const socket_error_information& error() const { return _error; }

			// Note: Type ssize_t can be negative (indicating an error)
			ssize_t receive(uint8_t* buffer, std::size_t buffer_size);
			ssize_t send(const uint8_t* buffer, std::size_t number_of_elements_to_send);

		private:
			networking::address _address;
			networking::socket _socket;
			status _status;
			socket_error_information _error;
	};

	std::ostream& operator<<(std::ostream&, connection::status state);
}
