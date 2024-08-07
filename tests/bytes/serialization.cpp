///////////////////////////////////////////////////////////////////////
// Tests of the serialization framework
///////////////////////////////////////////////////////////////////////
#include <gtest/gtest.h>

#include <bytes/serialize.h>

namespace
{
	constexpr std::size_t string_buffer_length = 20;
}

struct test_struct
{
	public:
		int integer;
		uint32_t large_number;
		float real_number;
		std::string message;
		long counter;
};

template <> auto bytes::serializer<test_struct>::serialize(const test_struct& value) -> typename bytes::serializer<test_struct>::buffer_type
{
	buffer_type buffer {};
	uint8_t* ptr = &buffer[0];

	bytes::serializer<int>::serialize_at(value.integer, ptr);
	ptr += sizeof(test_struct::integer);

	bytes::serializer<uint32_t>::serialize_at(value.large_number, ptr);
	ptr += sizeof(test_struct::large_number);

	bytes::serializer<float>::serialize_at(value.real_number, ptr);
	ptr += sizeof(test_struct::real_number);

	bytes::serializer<std::string, string_buffer_length>::serialize_at(value.message, ptr);
	ptr += string_buffer_length;

	bytes::serializer<long>::serialize_at(value.counter, ptr);

	return buffer;
}

template <> auto bytes::serializer<test_struct>::deserialize(const bytes::serializer<test_struct>::buffer_type& buffer) -> test_struct
{
	const uint8_t* ptr = &buffer[0];
	test_struct result {};

	auto& location_integer = *reinterpret_cast<const bytes::serializer<int>::buffer_type*>(ptr);
	result.integer = bytes::serializer<int>::deserialize(location_integer);
	ptr += sizeof(test_struct::integer);

	auto& location_large_number = *reinterpret_cast<const bytes::serializer<uint32_t>::buffer_type*>(ptr);
	result.large_number = bytes::serializer<uint32_t>::deserialize(location_large_number);
	ptr += sizeof(test_struct::large_number);

	auto& location_real_number = *reinterpret_cast<const bytes::serializer<float>::buffer_type*>(ptr);
	result.real_number = bytes::serializer<float>::deserialize(location_real_number);
	ptr += sizeof(test_struct::real_number);

	using string_serializer = bytes::serializer<std::string, string_buffer_length>;
	auto& location_message = *reinterpret_cast<const string_serializer::buffer_type*>(ptr);
	result.message = string_serializer::deserialize(location_message);
	ptr += string_buffer_length;

	auto& location_counter = *reinterpret_cast<const bytes::serializer<long>::buffer_type*>(ptr);
	result.counter = bytes::serializer<long>::deserialize(location_counter);

	return result;
}

TEST(bytes, serialize_and_deserialize_struct)
{
	test_struct input {};

	input.integer = 42;
	input.large_number = static_cast<uint32_t>(0xDEADBEEF1337U);
	input.real_number = 1.23456e+42;
	input.message = "Hello World!";
	input.counter = 65536;

	auto intermediate = bytes::serializer<test_struct>::serialize(input);
	auto output = bytes::serializer<test_struct>::deserialize(intermediate);

	EXPECT_EQ(input.integer, output.integer);
	EXPECT_EQ(input.large_number, output.large_number);
	EXPECT_EQ(input.real_number, output.real_number);
	EXPECT_EQ(input.message, output.message);
	EXPECT_EQ(input.counter, output.counter);
}
