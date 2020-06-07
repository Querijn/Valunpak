#pragma once

#include <valunpak/config.hpp>

#include <vector>

namespace valunpak
{
	class sha1
	{
	public:
		sha1();
		sha1(const sha1& a_copy);

		void process(const u8* a_data, size_t a_size);

		static sha1 from_data(const u8* a_data, size_t a_size);
		static sha1 from_data(const std::vector<u8>& a_data);

		static sha1 from_digest(const u8 (&a_digest)[20]);
		static sha1 from_digest(const u32 (&a_digest)[5]);

		bool operator!=(const sha1& a_other) const;
		bool operator==(const sha1& a_other) const;

	private:
		union
		{
			struct { u8 m_digest8[20]; };
			struct { u32 m_digest[5]; };
		};
	};
}
