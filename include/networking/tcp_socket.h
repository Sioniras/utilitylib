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
			void close() { _socket.close(); _status = status::closed; }
			status state() { return _status; }
			const networking::socket& socket() { return _socket; }
			const socket_error_information& error() const { return _error; }

		private:
			networking::socket _socket;
			status _status;
			socket_error_information _error;
	};

	inline std::ostream& operator<<(std::ostream& s, connection::status state)
	{
		switch(state)
		{
			case connection::status::invalid: s << std::string("invalid");break;
			case connection::status::open: s << std::string("open");break;
			case connection::status::closed: s << std::string("closed");break;
			case connection::status::error: s << std::string("error");break;
			default: s << std::string("unknown");break;
		}

		return s;
	}
}
