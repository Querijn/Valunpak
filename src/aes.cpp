#include <valunpak/aes.hpp>

#include <rijndael/rijndael.hpp>

namespace valunpak
{
	aes::aes(const u8* a_key)
	{
		m_round_num = rijndael_key_setup_dec(m_key_buffer, a_key, 256);
	}

	void aes::decrypt(u8* a_data, size_t a_size)
	{
		for (u32 i = 0; i < a_size; i += 16)
			rijndael_decrypt(m_key_buffer, m_round_num, a_data + i, a_data + i);
	}
}

