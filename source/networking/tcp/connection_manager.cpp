/////////////////////////////////////////////////////////////////////////
// TCP connection manager implementation
//
// Handles a collection of connections and listeners.
/////////////////////////////////////////////////////////////////////////
#include <networking/tcp/connection_manager.h>
#include <networking/tcp/listener.h>
#include <networking/tcp/connection.h>

namespace networking::tcp
{
	// ----------------------------------------------------------------------
	// Constructors / destructor
	// ----------------------------------------------------------------------
	connection_manager::connection_manager(data_received_callback& callback) :
		_connections(),
		_listeners(),
		_callback(callback),
		_pollfd(),
		_dirty(true)
	{
	}

	// Destructor
	connection_manager::~connection_manager()
	{
	}

	// ----------------------------------------------------------------------
	// Public interface
	// ----------------------------------------------------------------------
	// Creates a new listener that lets the same connection manager instance handle new connections
	const listener& connection_manager::add_listener(port_number_t port, bool use_ipv6)
	{
		listener new_listener { *this, port, use_ipv6 };
		_listeners.push_back(std::move(new_listener));
		_dirty = true;
		return _listeners.back();
	}

	// Add an already constructed listener, which does not have to use this connection manager instance for handling new connections
	const listener& connection_manager::add_listener(listener&& l)
	{
		_listeners.push_back(std::move(l));
		_dirty = true;
		return _listeners.back();
	}

	// Note: For more on poll, see https://beej.us/guide/bgnet/html/split/slightly-advanced-techniques.html#poll
	bool connection_manager::update(uint16_t timeout_ms)
	{
		// Check whether the collections have changed
		if(_dirty)
			setup_pollfd();

		int number_of_events = ::poll(&(_pollfd[0]), _pollfd.size(), static_cast<int>(timeout_ms));
		if (number_of_events > 0)
		{
			// Check for new connections
			unsigned int i = 0;
			for ( ; i < _listeners.size(); i++)
			{
				if (_pollfd[i].revents & POLLIN)
					_listeners[i].accept();
			}

			// Check for available data in an active connection
			for ( ; i < _pollfd.size(); i++)
			{
				if (_pollfd[i].revents & POLLIN)
					_callback.on_receive(_connections[i - _listeners.size()]);
			}
		}
		else if (number_of_events < 0)
		{
			return false;
		}

		return true;
	}

	// ----------------------------------------------------------------------
	// Private helpers
	// ----------------------------------------------------------------------
	void connection_manager::setup_pollfd()
	{
		_dirty = false;
		_pollfd.clear();

		for(auto i = _listeners.cbegin(); i != _listeners.cend(); i++)
			_pollfd.push_back({ i->socket().get(), POLLIN, 0 });

		for(auto i = _connections.cbegin(); i != _connections.cend(); i++)
			_pollfd.push_back({ i->socket().get(), POLLIN, 0 });
	}
}
