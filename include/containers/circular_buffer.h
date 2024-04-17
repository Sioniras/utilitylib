/////////////////////////////////////////////////////////////////////////
// Circular buffer
// 
// Thread-safe when used in a producer-consumer context, i.e. one thread
// may write and another thread may read (and these should always be the
// same two threads who either read or write, and cannot changes roles).
//
// Note: If the "checkoverflow" template parameter is set to true,
//       integer overflow in the counters will be prevented.
/////////////////////////////////////////////////////////////////////////
#pragma once

#include <array>
#include <atomic>
#include <limits>

namespace utility
{
	template <typename T, std::size_t N, bool checkoverflow = true>
	class circular_buffer
	{
		private:
			using counter_type = uint64_t;
			constexpr static inline counter_type counter_max = std::numeric_limits<counter_type>::max() - 2*N;

		public:
			// Constructor / destructor
			circular_buffer() : _buffer{}, _readIndex(0), _writeIndex(0) {}

			// Returns the number of elements stored in the buffer
			std::size_t size() const
			{
				const auto r = _readIndex.load();
				const auto w = _writeIndex.load();

				// Write will always be ahead, and not by more than N as that would be an overflow
				return w - r;
			}

			// Returns the amount of space left in the buffer for new writes
			std::size_t available() const
			{
				return N - size();
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
					return false;

				// Perform the actual update
				// Note: Memory order must ensure that the buffer assignment happens before write index increment
				_buffer[w % N] = element;
				_writeIndex.store(w+1);
				return true;
			}

			// Reads an element from the buffer and provides it, advancing the read pointer
			// Returns false iff the buffer is empty
			bool read(T& result)
			{
				const auto r = _readIndex.load();	// Current read position
				const auto w = _writeIndex.load();	// New write position

				// Check that the buffer is not empty
				if(r == w)
					return false;

				// Read the value and then increment read pointer (keep memory order!)
				result = _buffer[r % N];
				_readIndex.store(r + 1);

				// Prevent integer overflow
				if constexpr (checkoverflow)
				{
					if(r > counter_max)
					{
						// A race condition may be possible here (with e.g. the "w == r" check in "push"),
						// but it is still better than overflow if counters ever get that high.
						// Fixing it is not worth the overhead.
						_readIndex -= counter_max;
						_writeIndex -= counter_max;
					}
				}

				return true;
			}

			// Provides the next value without incrementing the read index, or returns false iff buffer is empty
			bool peek(T& result)
			{
				const auto r = _readIndex.load();	// Current read position
				const auto w = _writeIndex.load();	// New write position

				// Check that the buffer is not empty
				if(r == w)
					return false;

				// Read the value
				result = _buffer[r % N];
				return true;
			}

			// Increments the read pointer without providing the element, or returns false iff buffer is empty
			bool pop()
			{
				const auto r = _readIndex.load();	// Current read position
				const auto w = _writeIndex.load();	// New write position

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

		private:
			std::array<T,N> _buffer;
			std::atomic<counter_type> _readIndex;
			std::atomic<counter_type> _writeIndex;
	};
}
