/////////////////////////////////////////////////////////////////////////
// TCP forward declarations / definitions
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>

namespace networking::tcp
{
	// Class prototypes
	class connection;
	class listener;

	// Status enum class for TCP listeners, aliased in listener class
	enum class listener_status
	{
		invalid,
		bound,		// Called bind, but not listen
		listening,	// Called bind and listen
		stopped,	// Socket closed
		error,
	};

	// Status enum class for TCP connections, aliased in connection class
	enum class connection_status
	{
		invalid,
		open,
		shutdown,
		closed,
		error,
	};

	// Callback interface for listeners
	class incoming_connection_callback
	{
		public:
			virtual ~incoming_connection_callback() {}
			virtual void on_new_connection(connection&&) = 0;
	};

	// Callback interface for receiving data
	class data_received_callback
	{
		public:
			virtual ~data_received_callback() {}
			virtual void on_receive(connection& ready) = 0;
	};
}
