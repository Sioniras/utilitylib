#include <iostream>

#include <networking/tcp/connection.h>

int main(int argc, const char** argv)
{
	auto a = networking::create_address("127.0.0.1", 4242, networking::protocol::tcp);
	networking::tcp::connection c { a };

	std::cout << "Opened connection with state: " << c.state() << std::endl;

	if(c.state() == networking::tcp::connection::status::error)
	{
		std::cout << "Error message: " << c.error().message << std::endl;
	}

	c.close();

	std::cout << "State after close: " << c.state() << std::endl;

	return 0;
}
