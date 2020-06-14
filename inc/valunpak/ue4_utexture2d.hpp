#pragma once

#include <valunpak/config.hpp>
#include <valunpak/ue4_bin_file.hpp>
#include <valunpak/ue4_uobject.hpp>

namespace valunpak
{
	class ue4_uexp;
	class ue4_utexture2d : public ue4_bin_file
	{
	public:
		bool open(ue4_uexp& a_parent, size_t& a_offset) noexcept;

	#pragma pack(push, 1)
		struct header
		{
			u8 flags[4];
			i32 is_cooked;
		};
	#pragma pack(pop)

	private:
		ue4_uobject m_object;
		header m_header;
		
		void reset();
		size_t read_internal();
	};
}
