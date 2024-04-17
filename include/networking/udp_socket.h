/////////////////////////////////////////////////////////////////////////
// UDP socket definition
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <networking/socket.h>

namespace networking::udp
{
	class socket
	{
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
			void close() { _socket.close(); }
			bool valid() { return _socket.valid(); }

		private:
			networking::socket _socket;
	};
}
