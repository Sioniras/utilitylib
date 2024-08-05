/////////////////////////////////////////////////////////////////////////
// Helper for combining an ordered list of byte manipulations
/////////////////////////////////////////////////////////////////////////
#pragma once

namespace bytes
{
	template <typename... strategies>
	struct byte_strategy
	{
			using buffer_type = class buffer;

			static constexpr void apply(buffer_type& buffer)
			{
				apply<strategies...>(buffer);
			}

		// Implementation details
		private:
			template <typename U>
			static constexpr void apply(buffer_type& buffer)
			{
				U::apply(buffer);
			}

			template <typename U, typename... remaining>
			static constexpr void apply(buffer_type& buffer)
			{
				apply<remaining...>(U::apply(buffer));
			}
	};
}
