/////////////////////////////////////////////////////////////////////////
// Network address object
/////////////////////////////////////////////////////////////////////////
#pragma once

#include "networking.h"

namespace networking
{
	class address
	{
		public:
			// Constructors
			constexpr explicit address() : _valid(false), _address({}), _length(0), _family(AF_UNSPEC) {}
			constexpr explicit address(const sockaddr& a, socklen_t length, sa_family_t family) :
				_valid(true), _address(a), _length(length), _family(family) {}

			// Copy-construction / copy-assignment
			address(const address&);
			address& operator=(const address&);

			// Move-construction / move-assignment
			address(address&&);
			address& operator=(address&&);

			// Comparison operator

			// Public interface
			const sockaddr& get() const { return _address; }
			constexpr bool valid() const { return _valid; }
			constexpr socklen_t length() const { return _length; }
			constexpr sa_family_t family() const { return _family; }
			constexpr ip_version ip_version_value() const { return _family == AF_INET6 ? ip_version::ipv6 : ip_version::ipv4; }

			result_string host(bool numeric_only = true) const;
			result_string port() const;
			result_string to_string(bool numeric_host_only = true) const;
			port_number_t port_number() const;

			// Static definition of invalid address
			static constexpr address invalid() { return address {}; }

		private:
			bool _valid;
			sockaddr _address;
			socklen_t _length;
			sa_family_t _family;	// AF_INET / AF_INET6
	};

	address create_address(std::string hostname, uint16_t port, protocol, bool resolve_hostname = false, ip_version = ip_version::any);
	address create_host_address(uint16_t port, protocol, ip_version = ip_version::any);
}
