#include <valunpak/ue4_bin_file.hpp>

#include <limits>

#include <debugbreak.h>

namespace valunpak
{
	bool ue4_bin_file::read_fname(std::string& a_string, size_t& a_offset)
	{
		if (read_fstring(a_string, a_offset) == false)
			return false;

		a_offset += 4;
		return true;
	}

	bool ue4_bin_file::read_fstring(std::string& a_string, size_t& a_offset)
	{

		i32 stored_len;
		read(stored_len, a_offset);

		if (stored_len == 0)
		{
			a_string = "";
			return true;
		}
		else if (stored_len == std::numeric_limits<i32>::min())
		{
			return false;
		}

		if (stored_len < 0)
		{
			debug_break(); // TODO: Test
			stored_len = -stored_len;
			a_string.resize(static_cast<size_t>(stored_len) * 2); // u16 -> u8
			u16* data = reinterpret_cast<u16*>(a_string.data());
			return read_array(data, stored_len, a_offset);
		}

		std::vector<char> buffer(stored_len);
		if (read_buffer(buffer.data(), stored_len, a_offset) == false)
			return false;

		a_string = buffer.data();
		return true;
	}

	bool ue4_bin_file::read_fstring_path(std::filesystem::path& a_path, size_t& a_offset)
	{
		std::string buffer;
		bool result = read_fstring(buffer, a_offset);
		a_path = buffer;
		return result;
	}
}

