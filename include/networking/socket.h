/////////////////////////////////////////////////////////////////////////
// Generic socket definitions
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <cstdint>

// Include platform-specific definitions
#include <networking/posix/socket_definitions.h>
#include <networking/win32/socket_definitions.h>

namespace networking
{
	enum class protocol
	{
		tcp,
		udp,
		any,
	};

	enum class ip_version
	{
		ipv4,
		ipv6,
		any,
	};

	struct result_string
	{
		int code;	// Zero for success, non-zero for an error
		std::string value;
	};

	class socket
	{
		public:
			// Constructors / destructor
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

	class address
	{
		public:
			constexpr explicit address() : _valid(false), _address({}), _length(0), _family(AF_UNSPEC) {}
			constexpr explicit address(const sockaddr& a, socklen_t length, sa_family_t family) :
				_valid(true), _address(a), _length(length), _family(family) {}

			const sockaddr& get() const { return _address; }
			constexpr bool valid() const { return _valid; }
			constexpr socklen_t length() const { return _length; }
			constexpr sa_family_t family() const { return _family; }
			constexpr ip_version ip_version_value() const { return _family == AF_INET6 ? ip_version::ipv6 : ip_version::ipv4; }

			result_string host(bool numeric_only = true) const;
			result_string port() const;
			result_string to_string(bool numeric_host_only = true) const;
			uint16_t port_number() const;

			static constexpr address invalid() { return address {}; }

		private:
			const bool _valid;
			const sockaddr _address;
			const socklen_t _length;
			const sa_family_t _family;	// AF_INET / AF_INET6
	};

	address create_address(std::string hostname, uint16_t port, protocol, bool resolve_hostname = false, ip_version = ip_version::any);
	address create_host_address(uint16_t port, protocol, ip_version = ip_version::any);
}
