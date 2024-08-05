/////////////////////////////////////////////////////////////////////////
// Buffer implementation
/////////////////////////////////////////////////////////////////////////
#include <bytes/buffer.h>

namespace bytes
{
	// ----------------------------------------------------------------------
	// Constructors / destructor
	// ----------------------------------------------------------------------
	// Constructor
	buffer::buffer() : _buffer()
	{
	}

	// Destructor
	buffer::~buffer()
	{
	}

	// ----------------------------------------------------------------------
	// Copy construction / assignment
	// ----------------------------------------------------------------------
	// Copy-construction
	buffer::buffer(const buffer& b) : _buffer(b._buffer)
	{
	}

	// Copy-assignment
	buffer& buffer::operator=(const buffer& b)
	{
		_buffer = b._buffer;
		return *this;
	}

	// ----------------------------------------------------------------------
	// Move construction / assignment
	// ----------------------------------------------------------------------
	// Move-construction
	buffer::buffer(buffer&& b) : _buffer(std::move(b._buffer))
	{
	}

	// Move-assignment
	buffer& buffer::operator=(buffer&& b)
	{
		_buffer = std::move(b._buffer);
		return *this;
	}

	// ----------------------------------------------------------------------
	// Public interface
	// ----------------------------------------------------------------------
	// Reserve space to avoid reallocations
	void buffer::reserve(std::size_t size)
	{
		_buffer.reserve(size);
	}
}
