#pragma once

#include <valunpak/config.hpp>

namespace valunpak
{
	class aes
	{
	public:
		aes(const u8* a_key);

		void decrypt(u8* a_data, size_t a_size);

	private:
		u32 m_key_buffer[60];
		i32 m_round_num;
	};
}
