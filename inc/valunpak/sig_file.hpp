#pragma once

#include <valunpak/config.hpp>
#include <valunpak/bin_file.hpp>

#include <memory>

namespace valunpak
{
	class sig_file : bin_file
	{
	public:
		bool open(std::string_view a_file_name) noexcept override;

		bool is_valid() const;

	private:

	};
}
