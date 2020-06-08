#include <valunpak/sig_file.hpp>

namespace valunpak
{
	bool sig_file::open(std::string_view a_file_name, bin_file::read_mode_type a_read_mode) noexcept
	{
		return bin_file::open(a_file_name, a_read_mode);
	}
	
	bool sig_file::is_valid() const
	{
		return false;
	}
}