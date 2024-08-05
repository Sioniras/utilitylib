/////////////////////////////////////////////////////////////////////////
// Basic serialization definitions
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include <array>
#include <cstdint>
#include <type_traits>

namespace bytes
{
	// Provides number of bytes for serialized object
	template <typename T> struct serialized_size { static constexpr std::size_t value = sizeof(T); };

	// Specialize with implementations of serialize and deserialize
	template <typename T>
	struct serializer
	{
			static constexpr std::size_t size = serialized_size<T>::value;

			using value_type = T;
			using buffer_type = std::array<uint8_t, size>;

			constexpr static buffer_type serialize(const T&);
			constexpr static T deserialize(const buffer_type&);
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
			uint8_t* get() { return &_data[0]; }
			const uint8_t* get() const { return &_data[0]; }
			std::size_t size() const { return _data.size(); }

		private:
			std::vector<uint8_t> _data;
	};

	// Serialization of arithmetic types (integers and floating point numbers)
	template <typename T, typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
	constexpr auto serialize(const T& value) -> typename serializer<T>::buffer_type
	{
		auto ptr = reinterpret_cast<const uint8_t*>(&value);
		auto a = typename serializer<T>::buffer_type {};
		std::copy(ptr, ptr+sizeof(T), &a[0]);
		return a;
	}

	template <typename T, typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
	constexpr auto deserialize(const typename serializer<T>::buffer_type& buffer) -> T
	{
		return *reinterpret_cast<const T*>(&buffer[0]);
	}

	// Template specializations for the serializer class
	template <typename T> constexpr auto serializer<T>::serialize(const T& value) -> typename serializer<T>::buffer_type
	{
		return bytes::serialize(value);
	}

	template <typename T> constexpr auto serializer<T>::deserialize(const serializer<T>::buffer_type& buffer) -> T
	{
		return bytes::deserialize(buffer);
	}
}
