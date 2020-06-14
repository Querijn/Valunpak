#include <valunpak/sha1.hpp>

#include <cstring>

namespace valunpak
{
	u32 byteswap(u32 a)
	{
		a = ((a & 0x000000FF) << 24) |
			((a & 0x0000FF00) << 8) |
			((a & 0x00FF0000) >> 8) |
			((a & 0xFF000000) >> 24);
		return a;
	}

	/*u64 byteswap(u64 a)
	{
		a = ((a & 0x00000000000000FFULL) << 56) |
			((a & 0x000000000000FF00ULL) << 40) |
			((a & 0x0000000000FF0000ULL) << 24) |
			((a & 0x00000000FF000000ULL) << 8) |
			((a & 0x000000FF00000000ULL) >> 8) |
			((a & 0x0000FF0000000000ULL) >> 24) |
			((a & 0x00FF000000000000ULL) >> 40) |
			((a & 0xFF00000000000000ULL) >> 56);
		return a;
	}*/

	// Templated constexpr rotate left
	template<unsigned int count, typename T>
	constexpr static inline T rol(T value)
	{
		const unsigned int mask = (8 * sizeof(T) - 1);
		static_assert(std::is_unsigned<T>::value, "Rotate Left only makes sense for unsigned types");
		static_assert(count <= mask, "Rotate Left can only take a value below or equal to its bit width.");
		return (value << count) | (value >> ((-count) & mask));
	}

	sha1::sha1()
	{
		m_digest[0] = 0x67452301;
		m_digest[1] = 0xEFCDAB89;
		m_digest[2] = 0x98BADCFE;
		m_digest[3] = 0x10325476;
		m_digest[4] = 0xC3D2E1F0;
	}
	
	sha1::sha1(const sha1& a_copy)
	{
		for (size_t i = 0; i < 5; i++)
			m_digest[i] = a_copy.m_digest[i];
	}

	// https://en.wikipedia.org/wiki/SHA-1#SHA-1_pseudocode
	void sha1::process(const u8* a_data, size_t a_size)
	{
		static const size_t block_count = 64;

		// Create a set of 0s in which we paste our data
		size_t padding = (block_count - a_size) % block_count; // Size needs to be a multiple of 64
		std::vector<u8> data(a_size + padding);
		memcpy(data.data(), a_data, a_size);

		// Message has to end with 0x80
		data[a_size] = 0x80;
		
		// Finally, add the bitcount to the end.
		u32 size = byteswap((u32)a_size * 8);
		auto back = a_size + padding - sizeof(size);
		memcpy(data.data() + back, &size, sizeof(size));

		m_digest[0] = 0x67452301;
		m_digest[1] = 0xEFCDAB89;
		m_digest[2] = 0x98BADCFE;
		m_digest[3] = 0x10325476;
		m_digest[4] = 0xC3D2E1F0;

		auto end = data.data() + data.size();
		for (u8* ptr = data.data(); ptr != end; ptr += 64)
		{
			u32* block = reinterpret_cast<u32*>(ptr);

			u32 word[80];
			size_t i = 0;
			for (; i < 16; i++)
				word[i] = byteswap(block[i]);

			for (; i < 80; i++)
				word[i] = rol<1>((word[i - 3] ^ word[i - 8] ^ word[i - 14] ^ word[i - 16]));

			u32 a = m_digest[0];
			u32 b = m_digest[1];
			u32 c = m_digest[2];
			u32 d = m_digest[3];
			u32 e = m_digest[4];

			for (i = 0; i < 80; i++)
			{
				u32 f = 0;
				u32 k = 0;

				if (i < 20)
				{
					f = (b & c) | (~b & d);
					k = 0x5A827999;
				}
				else if (i < 40) 
				{
					f = b ^ c ^ d;
					k = 0x6ED9EBA1;
				}
				else if (i < 60)
				{
					f = (b & c) | (b & d) | (c & d);
					k = 0x8F1BBCDC;
				}
				else
				{
					f = b ^ c ^ d;
					k = 0xCA62C1D6;
				}

				u32 temp = rol<5>(a) + f + e + k + word[i];
				e = d;
				d = c;
				c = rol<30>(b);
				b = a;
				a = temp;
			}

			m_digest[0] += a;
			m_digest[1] += b;
			m_digest[2] += c;
			m_digest[3] += d;
			m_digest[4] += e;
		}
	}
	
	sha1 sha1::from_data(const u8* a_data, size_t a_size)
	{
		sha1 result;
		result.process(a_data, a_size);
		return result;
	}
	sha1 sha1::from_data(const std::vector<u8>& a_data)
	{
		sha1 result;
		result.process(a_data.data(), a_data.size());
		return result;
	}

	sha1 sha1::from_digest(const u8(&a_digest)[20])
	{
		sha1 result;
		for (size_t i = 0; i < 20; i++)
			result.m_digest8[i] = a_digest[i];
		return result;
	}

	sha1 sha1::from_digest(const u32(&a_digest)[5])
	{
		sha1 result;
		for (size_t i = 0; i < 5; i++)
			result.m_digest[i] = a_digest[i];
		return result;
	}
	
	bool sha1::operator!=(const sha1& a_other) const
	{
		for (size_t i = 0; i < 5; i++)
			if (m_digest[i] != a_other.m_digest[i])
				return true;

		return false;
	}
	
	bool sha1::operator==(const sha1& a_other) const
	{
		return operator!=(a_other) == false;
	}
}
