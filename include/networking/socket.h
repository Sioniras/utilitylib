/////////////////////////////////////////////////////////////////////////
// Generic socket definitions
/////////////////////////////////////////////////////////////////////////
#pragma once

#include "networking.h"

namespace networking
{
	class socket
	{
		public:
			// Constructors / destructor
			socket();
			socket(protocol, ip_version);
			~socket();

			// Disallow copying
			socket(const socket&) = delete;
			socket& operator=(const socket&) = delete;

			// Move construction/assignment
			socket(socket&&);
			socket& operator=(socket&&);

			// Public interface
			void close();
			bool valid() const { return is_valid_socket(_socket); }
			socket_type get() const { return _socket; }

		private:
			socket_type _socket;
	};
}
