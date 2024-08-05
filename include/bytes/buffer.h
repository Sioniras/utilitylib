/////////////////////////////////////////////////////////////////////////
// Byte buffer definition
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include <cstdint>

namespace bytes
{
	class buffer
	{
		public:
			// Constructors / destructor
			buffer();
			~buffer();

			// Copy construction/assignment
			buffer(const buffer&);
			buffer& operator=(const buffer&);

			// Move construction/assignment
			buffer(buffer&&);
			buffer& operator=(buffer&&);

			// Public interface
			uint8_t* get() { return &_buffer[0]; }
			const uint8_t* get() const { return &_buffer[0]; }
			std::size_t size() const { return _buffer.size(); }

			void reserve(std::size_t);

		private:
			std::vector<uint8_t> _buffer;
	};
}
