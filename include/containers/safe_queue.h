/////////////////////////////////////////////////////////////////////////
// Thread-safe queue implemented using circular buffers
// 
// Similar to the circular_buffer class, but resizes to fit data.
//
// Thread-safe when used in a producer-consumer context, i.e. one thread
// may write and another thread may read (and these should always be the
// same two threads who either read or write, and cannot changes roles).
//
// Note: If the "checkoverflow" template parameter is set to true,
//       integer overflow in the counters will be prevented.
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <memory>
#include <vector>
#include <atomic>
#include <limits>

namespace utility
{
	namespace reallocation_policies
	{
		// Reallocation policies defines a static "next" method providing the minimum next reserved amount
		template <std::size_t amount> struct increment { constexpr static std::size_t next(std::size_t s) {return s + amount;} };
		struct doubling { constexpr static std::size_t next(std::size_t s) {return 2*s;} };
	}

	template <typename T, typename reallocation_policy = reallocation_policies::doubling, bool checkoverflow = true>
	class safe_queue
	{
		private:
			// Types / constants
			using counter_type = uint64_t;
			using buffer_type = std::vector<T>;
			constexpr static counter_type counter_max(std::size_t s) {return std::numeric_limits<counter_type>::max() - 2*s;}

		public:
			// Constructor / destructor
			safe_queue(std::size_t initial_size = 10) :
				_buffer(std::make_shared<buffer_type>(initial_size, T{})),
				_readIndex(0), _writeIndex(0) {}

			// Returns the number of elements stored in the buffer
			std::size_t size() const
			{
				const auto r = _readIndex.load();
				const auto w = _writeIndex.load();
				return w - r;
			}

			// Returns the amount of space left in the buffer before reallocation
			std::size_t capacity() const
			{
				return _buffer->size() - size();
			}

			// Checks whether the buffer has any elements
			bool empty() const
			{
				const auto r = _readIndex.load();
				const auto w = _writeIndex.load();
				return r == w;
			}

			// Pushes an element onto the buffer and returns true if successful, or false if buffer is full
			bool push(T element)
			{
				const auto r = _readIndex.load();	// Current read position
				const auto w = _writeIndex.load();	// New write position

				// Check whether write is catching up to the read pointer
				// Reads can be done before actually doing the write below, but that does not matter
				if(w+1 == r)
					increase_capacity();

				// Perform the actual update
				// Note: Memory order must ensure that the buffer assignment happens before write index increment
				(*_buffer)[w % _buffer->size()] = element;
				_writeIndex.store(w+1);
				return true;
			}

			// Reads an element from the buffer and provides it, advancing the read pointer
			// Returns false iff the buffer is empty
			bool read(T& result)
			{
				const auto current = _buffer;
				const auto r = _readIndex.load();	// Current read position
				const auto w = _writeIndex.load();	// Current write position

				// Check that the buffer is not empty
				if(r == w)
					return false;

				// Read the value and then increment read pointer (keep memory order!)
				result = (*current)[r % current->size()];
				_readIndex.store(r + 1);

				// Prevent integer overflow
				if constexpr (checkoverflow)
				{
					const auto cmax = counter_max(current->size());
					if(r > cmax)
					{
						// A race condition may be possible here (with e.g. the "w == r" check in "push"),
						// but it is still better than overflow if counters ever get that high.
						// Fixing it is not worth the overhead.
						_readIndex -= cmax;
						_writeIndex -= cmax;
					}
				}

				return true;
			}

			// Provides the next value without incrementing the read index, or returns false iff buffer is empty
			bool peek(T& result) const
			{
				const auto current = _buffer;
				const auto r = _readIndex.load();	// Current read position
				const auto w = _writeIndex.load();	// Current write position

				// Check that the buffer is not empty
				if(r == w)
					return false;

				// Read the value
				result = current->at(r % current->size());
				return true;
			}

			// Increments the read pointer without providing the element, or returns false iff buffer is empty
			bool pop()
			{
				// Get current read/write positions
				const auto r = _readIndex.load();
				const auto w = _writeIndex.load();

				// Check that the buffer is not empty
				if(r == w)
					return false;

				// Increment read pointer (keep memory order!)
				_readIndex.store(r + 1);
				return true;
			}

			// Clears the buffer
			void clear()
			{
				_readIndex.store(_writeIndex.load());
			}

			bool increase_capacity(std::size_t additional_required_entries = 1)
			{
				// Get new buffer filled with default-initialized elements
				const auto minimum_next_size = reallocation_policy::next(_buffer->size());
				const auto next_size = (minimum_next_size >= additional_required_entries ? minimum_next_size : additional_required_entries);
				auto buffer = std::make_shared<buffer_type>(next_size, T{});

				// Get current read/write positions
				const auto r = _readIndex.load();
				const auto w = _writeIndex.load();

				// Copy elements to the new buffer
				for(auto i = r; i < w; i++)
					(*buffer)[i % buffer->size()] = (*_buffer)[i % _buffer->size()];

				// Finally assign the new buffer once it is ready
				_buffer = buffer;
				return true;
			}

		private:
			std::shared_ptr<buffer_type> _buffer;
			std::atomic<counter_type> _readIndex;
			std::atomic<counter_type> _writeIndex;
	};
}
