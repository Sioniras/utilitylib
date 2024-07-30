/////////////////////////////////////////////////////////////////////////
// UDP socket definition
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <networking/socket.h>
#include <networking/address.h>
#include <networking/udp/udp.h>

namespace networking::udp
{
	class socket
	{
		public:
			using status = socket_status;

		public:
			// Constructors / destructor
			socket();
			~socket();

			// Disallow copying
			socket(const socket&) = delete;
			socket& operator=(const socket&) = delete;

			// Move construction/assignment
			socket(socket&&);
			socket& operator=(socket&&);

			// Public interface
			bool bind(const networking::address& target);
			void close();
			bool valid() { return _socket.valid(); }
			const address& bound_to() const { return _boundAddress; }
			status state() const { return _status; }
			const socket_error_information& error() const { return _error; }

			// Note: Type ssize_t can be negative (indicating an error)
			ssize_t receive_from(uint8_t* buffer, std::size_t buffer_size, networking::address& target) const;
			ssize_t send_to(const uint8_t* buffer, std::size_t number_of_elements_to_send, const networking::address& target) const;

		private:
			networking::socket _socket;
			address _boundAddress;
			status _status;
			socket_error_information _error;
	};
}
