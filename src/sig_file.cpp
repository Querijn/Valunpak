#include <valunpak/sig_file.hpp>

namespace valunpak
{
	bool sig_file::open(std::string_view a_file_name) noexcept
	{
		return bin_file::open(a_file_name);
	}
	
	bool sig_file::is_valid() const
	{
		return false;
	}
}