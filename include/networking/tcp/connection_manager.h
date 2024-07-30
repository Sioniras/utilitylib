/////////////////////////////////////////////////////////////////////////
// TCP connection manager
//
// Handles a collection of connections and listeners.
// Note: Not thread-safe.
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>

#include <networking/networking.h>
#include <networking/tcp/tcp.h>

namespace networking::tcp
{
	class connection_manager : public incoming_connection_callback
	{
		public:
			// Constructor / destructor
			explicit connection_manager(data_received_callback&);
			~connection_manager();

			// Disallow copying
			connection_manager(const connection_manager&) = delete;
			connection_manager& operator=(const connection_manager&) = delete;

			// Move construction/assignment
			connection_manager(connection_manager&&);
			connection_manager& operator=(connection_manager&&);

			// incoming_connection_callback interface
			void on_new_connection(connection&&) override;

			// Public interface
			const listener& add_listener(port_number_t port, bool use_ipv6 = false);
			const listener& add_listener(listener&&);

			void add_connection(connection&& c) { on_new_connection(std::move(c)); };

			bool update(uint16_t timeout_ms = 500);		// !! BLOCKING, unless timeout is zero !!

		private:
			void setup_pollfd();

		private:
			std::vector<connection> _connections;
			std::vector<listener> _listeners;
			data_received_callback& _callback;
			std::vector<struct pollfd> _pollfd;
			bool _dirty;	// Dirty-flag for changes in _listeners and _connections
	};
}
