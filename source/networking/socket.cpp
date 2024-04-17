/////////////////////////////////////////////////////////////////////////
// Generic socket implementation
/////////////////////////////////////////////////////////////////////////
#include <networking/socket.h>

#include <stdexcept>

namespace networking
{
	// ----------------------------------------------------------------------
	// Constructors / destructor
	// ----------------------------------------------------------------------
	// Constructor
	socket::socket(protocol p, ip_version ipv) : _socket(uninitialized_socket)
	{
		auto inet_protocol = (ipv == ip_version::ipv6 ? AF_INET6 : AF_INET);
		auto protocol_type = (p == protocol::tcp ? SOCK_STREAM : SOCK_DGRAM);
		auto specific_protocol = 0;
		auto s = ::socket(inet_protocol, protocol_type, specific_protocol);

		if(is_valid_socket(s))
			_socket = s;
	}

	// Destructor
	socket::~socket()
	{
		if(valid())
			close_socket(_socket);
	}

	// ----------------------------------------------------------------------
	// Move construction / assignment
	// ----------------------------------------------------------------------
	// Move-construction
	socket::socket(socket&& s) : _socket(uninitialized_socket)
	{
		// Make a proper swap such that two socket objects never hold the same socket
		auto temp = s._socket;
		s._socket = uninitialized_socket;
		_socket = temp;
	}

	// Move-assignment
	socket& socket::operator=(socket&& s)
	{
		// Make a proper swap such that two socket objects never hold the same socket
		auto temp = s._socket;
		s._socket = uninitialized_socket;
		_socket = temp;

		return *this;
	}

	// ----------------------------------------------------------------------
	// Public interface
	// ----------------------------------------------------------------------
	void socket::close()
	{
		close_socket(_socket);
		_socket = uninitialized_socket;
	}

	// ----------------------------------------------------------------------
	// Address public interface
	// ----------------------------------------------------------------------
	result_string address::host(bool numeric_only) const
	{
		if(!_valid)
			return { EAI_FAMILY, "invalid" };

		constexpr uint32_t hostname_size = 256;
		char hostname[hostname_size];

		auto flags = numeric_only ? NI_NUMERICHOST : 0;

		int result = getnameinfo(&(_address), _length, hostname, hostname_size, 0, 0, flags);
		return { result, result == 0 ? std::string(hostname) : gai_strerror(result) };
	}

	result_string address::port() const
	{
		if(!_valid)
			return { EAI_FAMILY, "invalid" };

		constexpr uint32_t service_size = 16;
		char service[service_size];

		auto flags = NI_NUMERICSERV;

		int result = getnameinfo(&(_address), _length, 0, 0, service, service_size, flags);
		return { result, result == 0 ? std::string(service) : gai_strerror(result) };
	}

	result_string address::to_string(bool numeric_host_only) const
	{
		if(!_valid)
			return { EAI_FAMILY, "invalid" };

		constexpr uint32_t hostname_size = 256;
		char hostname[hostname_size];
		constexpr uint32_t service_size = 16;
		char service[service_size];

		auto flags = NI_NUMERICSERV | (numeric_host_only ? 0 : NI_NUMERICHOST);

		int result = getnameinfo(&(_address), _length, hostname, hostname_size, service, service_size, flags);
		return { result, result == 0 ? (std::string(hostname) + ':' + std::string(service)) : gai_strerror(result) };
	}

	uint16_t address::port_number() const
	{
		auto p = port();
		if(p.code != 0)
			return 0;

		uint32_t port_number = 0;
		try
		{
			port_number = std::stoul(p.value);
		}
		catch(std::invalid_argument&)	// Throw message? For now just keep value 0
		{
		}
		catch(std::out_of_range&)	// Throw message? For now just keep value 0
		{
		}

		return port_number;
	}

	// ----------------------------------------------------------------------
	// Address generation
	// ----------------------------------------------------------------------
	class address_info_helper
	{
		public:
			// Normal connection
			explicit address_info_helper(const std::string& host, const std::string& service, const addrinfo& info) :
				_result(nullptr),
				_error_code(0)
			{
				_error_code = getaddrinfo(host.c_str(), service.c_str(), &info, &_result);
			}

			// Host listener
			explicit address_info_helper(const std::string& service, const addrinfo& info) :
				_result(nullptr),
				_error_code(0)
			{
				_error_code = getaddrinfo(0, service.c_str(), &info, &_result);
			}

			~address_info_helper()
			{
				if(_result != nullptr)
					freeaddrinfo(_result);
			}

			bool valid() { return _error_code == 0; }
			int error_code() { return _error_code; }

			address retrieve(uint8_t index = 0)
			{
				auto ptr = _result;
				for( ; index > 0; index--)
				{
					if(ptr->ai_next == nullptr)
						return address::invalid();
					ptr = ptr->ai_next;
				}

				if(ptr == nullptr || ptr->ai_addr == nullptr)
					return address::invalid();
				return address { *(ptr->ai_addr), ptr->ai_addrlen, static_cast<sa_family_t>(ptr->ai_family) };
			}

		private:
			struct addrinfo* _result;
			int _error_code;
	};

	void set_hints(struct addrinfo& hints, protocol proto, ip_version ipv)
	{
		// Set IP version
		switch(ipv)
		{
			case ip_version::ipv4:
				hints.ai_family = AF_INET;
				break;
			case ip_version::ipv6:
				hints.ai_family = AF_INET6;
				break;
			default:
				hints.ai_family = AF_UNSPEC;
#ifdef AI_ALL
				hints.ai_flags |= AI_ALL;
#else
				hints.ai_flags |= 0x0100;	// For compatility with some Windows systems (L. V. Winkle book, chapter 5)
#endif
				break;
		}

		// Set protocol
		switch(proto)
		{
			case protocol::tcp: hints.ai_socktype = SOCK_STREAM;break;
			case protocol::udp: hints.ai_socktype = SOCK_DGRAM;break;
			default: hints.ai_socktype = 0;break;
		}

		// Any protocol for the socket type
		hints.ai_protocol = 0;
	}

	address create_address(std::string hostname, uint16_t port, protocol proto, bool resolve_hostname, ip_version ipv)
	{
		// Prepare hints struct
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));

		hints.ai_flags |= AI_ADDRCONFIG;	// Only check available addresses (i.e. only IPv4 if IPv6 is not supported)
		hints.ai_flags |= AI_NUMERICSERV;	// Do not resolve service name to port number
		if(!resolve_hostname)
			hints.ai_flags |= AI_NUMERICHOST;

		set_hints(hints, proto, ipv);

		address_info_helper helper { hostname, std::to_string(port), hints };
		return helper.retrieve();
	}

	address create_host_address(uint16_t port, protocol proto, ip_version ipv)
	{
		// Prepare hints struct
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));

		hints.ai_flags |= AI_ADDRCONFIG;	// Only check available addresses (i.e. only IPv4 if IPv6 is not supported)
		hints.ai_flags |= AI_NUMERICSERV;	// Do not resolve service name to port number
		hints.ai_flags |= AI_PASSIVE;		// Request wildcard-address

		set_hints(hints, proto, ipv);

		address_info_helper helper { std::to_string(port), hints };
		return helper.retrieve();
	}
}
