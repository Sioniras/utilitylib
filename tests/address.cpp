///////////////////////////////////////////////////////////////////////
// Tests for the address class (generation and use).
///////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include <networking/socket.h>

TEST(networking_address, create_address_tcp_ipv4)
{
	constexpr uint16_t port = 2048;
	constexpr auto protocol = networking::protocol::tcp;
	constexpr auto ipv = networking::ip_version::ipv4;
	std::string hostname { "127.0.0.1" };

	auto addr = networking::create_address(hostname, port, protocol, false, ipv);

	EXPECT_TRUE(addr.valid());
	EXPECT_EQ(addr.port_number(), port);
	EXPECT_EQ(addr.port().code, 0);
	EXPECT_EQ(addr.port().value, std::to_string(port));
	EXPECT_EQ(addr.host(true).code, 0);
	EXPECT_EQ(addr.host(true).value, hostname);
}

TEST(networking_address, create_address_tcp_ipv4_localhost)
{
	constexpr uint16_t port = 22;
	constexpr auto protocol = networking::protocol::tcp;
	constexpr auto ipv = networking::ip_version::ipv4;
	std::string hostname { "localhost" };

	auto addr = networking::create_address(hostname, port, protocol, true, ipv);

	EXPECT_TRUE(addr.valid());
	EXPECT_EQ(addr.port_number(), port);
	EXPECT_EQ(addr.port().code, 0);
	EXPECT_EQ(addr.port().value, std::to_string(port));
	EXPECT_EQ(addr.host(false).code, 0);
	EXPECT_EQ(addr.host(false).value, hostname);
}
