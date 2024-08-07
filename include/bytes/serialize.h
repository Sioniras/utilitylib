/////////////////////////////////////////////////////////////////////////
// Basic serialization definitions
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <vector>
#include <array>
#include <cstdint>
#include <algorithm>
#include <type_traits>

namespace bytes
{
	using byte = uint8_t;

	// Provides number of bytes for serialized object
	template <typename T> struct serialized_size { static constexpr std::size_t value = sizeof(T); };

	// Specialize with implementations of serialize and deserialize
	template <typename T, std::size_t size = serialized_size<T>::value>
	struct serializer
	{
			using value_type = T;
			using buffer_type = std::array<byte, size>;

			static buffer_type serialize(const T&);
			static void serialize_at(const T&, byte*);
			static T deserialize(const buffer_type&);
	};

	// A buffer to hold serialized data
	class serialized_data
	{
		public:
			// Constructor
			template <typename T>
			serialized_data(T entity) :
				_data(serializer<T>::serialize(entity))
			{
			}

			// Public interface
			byte* get() { return &_data[0]; }
			const byte* get() const { return &_data[0]; }
			std::size_t size() const { return _data.size(); }

		private:
			std::vector<byte> _data;
	};

	// Serialization of arithmetic types (integers and floating point numbers)
	template <typename T, std::size_t S = sizeof(T), typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
	constexpr auto serialize_at(const T& value, byte* destination) -> void
	{
		auto ptr = reinterpret_cast<const byte*>(&value);
		std::copy(ptr, ptr + S, destination);
	}

	template <typename T, std::size_t S = sizeof(T), typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
	constexpr auto serialize(const T& value) -> typename serializer<T,S>::buffer_type
	{
		auto a = typename serializer<T,S>::buffer_type {};
		serialize_at(value, &a[0]);
		return a;
	}

	template <typename T, std::size_t S = sizeof(T), typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
	constexpr auto deserialize(const typename serializer<T,S>::buffer_type& buffer) -> T
	{
		return *reinterpret_cast<const T*>(&buffer[0]);
	}

	// Serialization of strings
	template <typename T, std::size_t buffer_size, typename = typename std::enable_if_t<std::is_same_v<T,std::string>>>
	constexpr void serialize_at(const std::string& value, byte* destination)
	{
		// Don't forget to set the terminating null byte
		auto end = (value.size() < buffer_size) ? value.end() : value.begin() + buffer_size - 1;
		std::copy(value.begin(), end, destination);
		destination[buffer_size - 1] = 0;
	}

	template <typename T, std::size_t buffer_size, typename = typename std::enable_if_t<std::is_same_v<T,std::string>>>
	constexpr auto serialize(const std::string& value) -> std::array<byte, buffer_size>
	{
		auto a = std::array<byte, buffer_size> {};
		serialize_at<buffer_size>(value, &a[0]);
		return a;
	}

	template <typename T, std::size_t buffer_size, typename = typename std::enable_if_t<std::is_same_v<T,std::string>>>
	auto deserialize(const std::array<byte, buffer_size>& buffer) -> std::string
	{
		if (std::find(buffer.begin(), buffer.end(), 0) != buffer.end())
			return std::string { reinterpret_cast<const char*>(&buffer[0]) };

		return std::string { reinterpret_cast<const char*>(&buffer[0]), buffer_size };
	}

	// Template specializations for the serializer class
	template <typename T, std::size_t S> auto serializer<T,S>::serialize(const T& value) -> typename serializer<T,S>::buffer_type
	{
		return bytes::serialize<T,S>(value);
	}

	template <typename T, std::size_t S> auto serializer<T,S>::serialize_at(const T& value, byte* destination) -> void
	{
		bytes::serialize_at<T,S>(value, destination);
	}

	template <typename T, std::size_t S> auto serializer<T,S>::deserialize(const serializer<T,S>::buffer_type& buffer) -> T
	{
		return bytes::deserialize<T,S>(buffer);
	}
}
