#include <valunpak/ue4_utexture2d.hpp>
#include <valunpak/ue4_uexp.hpp>

#include <cassert>

#include <debugbreak.h>

namespace valunpak
{
	ue4_utexture2d::pixel_format to_pixel_format(const char* a_format)
	{
		switch (fnv(a_format))
		{
		case fnv("PF_Unknown"): return ue4_utexture2d::pixel_format::PF_Unknown;
		case fnv("PF_A32B32G32R32F"): return ue4_utexture2d::pixel_format::PF_A32B32G32R32F;
		case fnv("PF_B8G8R8A8"): return ue4_utexture2d::pixel_format::PF_B8G8R8A8;
		case fnv("PF_G8"): return ue4_utexture2d::pixel_format::PF_G8;
		case fnv("PF_G16"): return ue4_utexture2d::pixel_format::PF_G16;
		case fnv("PF_DXT1"): return ue4_utexture2d::pixel_format::PF_DXT1;
		case fnv("PF_DXT3"): return ue4_utexture2d::pixel_format::PF_DXT3;
		case fnv("PF_DXT5"): return ue4_utexture2d::pixel_format::PF_DXT5;
		case fnv("PF_UYVY"): return ue4_utexture2d::pixel_format::PF_UYVY;
		case fnv("PF_FloatRGB"): return ue4_utexture2d::pixel_format::PF_FloatRGB;
		case fnv("PF_FloatRGBA"): return ue4_utexture2d::pixel_format::PF_FloatRGBA;
		case fnv("PF_DepthStencil"): return ue4_utexture2d::pixel_format::PF_DepthStencil;
		case fnv("PF_ShadowDepth"): return ue4_utexture2d::pixel_format::PF_ShadowDepth;
		case fnv("PF_R32_FLOAT"): return ue4_utexture2d::pixel_format::PF_R32_FLOAT;
		case fnv("PF_G16R16"): return ue4_utexture2d::pixel_format::PF_G16R16;
		case fnv("PF_G16R16F"): return ue4_utexture2d::pixel_format::PF_G16R16F;
		case fnv("PF_G16R16F_FILTER"): return ue4_utexture2d::pixel_format::PF_G16R16F_FILTER;
		case fnv("PF_G32R32F"): return ue4_utexture2d::pixel_format::PF_G32R32F;
		case fnv("PF_A2B10G10R10"): return ue4_utexture2d::pixel_format::PF_A2B10G10R10;
		case fnv("PF_A16B16G16R16"): return ue4_utexture2d::pixel_format::PF_A16B16G16R16;
		case fnv("PF_D24"): return ue4_utexture2d::pixel_format::PF_D24;
		case fnv("PF_R16F"): return ue4_utexture2d::pixel_format::PF_R16F;
		case fnv("PF_R16F_FILTER"): return ue4_utexture2d::pixel_format::PF_R16F_FILTER;
		case fnv("PF_BC5"): return ue4_utexture2d::pixel_format::PF_BC5;
		case fnv("PF_V8U8"): return ue4_utexture2d::pixel_format::PF_V8U8;
		case fnv("PF_A1"): return ue4_utexture2d::pixel_format::PF_A1;
		case fnv("PF_FloatR11G11B10"): return ue4_utexture2d::pixel_format::PF_FloatR11G11B10;
		case fnv("PF_A8"): return ue4_utexture2d::pixel_format::PF_A8;
		case fnv("PF_R32_UINT"): return ue4_utexture2d::pixel_format::PF_R32_UINT;
		case fnv("PF_R32_SINT"): return ue4_utexture2d::pixel_format::PF_R32_SINT;
		case fnv("PF_PVRTC2"): return ue4_utexture2d::pixel_format::PF_PVRTC2;
		case fnv("PF_PVRTC4"): return ue4_utexture2d::pixel_format::PF_PVRTC4;
		case fnv("PF_R16_UINT"): return ue4_utexture2d::pixel_format::PF_R16_UINT;
		case fnv("PF_R16_SINT"): return ue4_utexture2d::pixel_format::PF_R16_SINT;
		case fnv("PF_R16G16B16A16_UINT"): return ue4_utexture2d::pixel_format::PF_R16G16B16A16_UINT;
		case fnv("PF_R16G16B16A16_SINT"): return ue4_utexture2d::pixel_format::PF_R16G16B16A16_SINT;
		case fnv("PF_R5G6B5_UNORM"): return ue4_utexture2d::pixel_format::PF_R5G6B5_UNORM;
		case fnv("PF_R8G8B8A8"): return ue4_utexture2d::pixel_format::PF_R8G8B8A8;
		case fnv("PF_A8R8G8B8"): return ue4_utexture2d::pixel_format::PF_A8R8G8B8;
		case fnv("PF_BC4"): return ue4_utexture2d::pixel_format::PF_BC4;
		case fnv("PF_R8G8"): return ue4_utexture2d::pixel_format::PF_R8G8;
		case fnv("PF_ATC_RGB"): return ue4_utexture2d::pixel_format::PF_ATC_RGB;
		case fnv("PF_ATC_RGBA_E"): return ue4_utexture2d::pixel_format::PF_ATC_RGBA_E;
		case fnv("PF_ATC_RGBA_I"): return ue4_utexture2d::pixel_format::PF_ATC_RGBA_I;
		case fnv("PF_X24_G8"): return ue4_utexture2d::pixel_format::PF_X24_G8;
		case fnv("PF_ETC1"): return ue4_utexture2d::pixel_format::PF_ETC1;
		case fnv("PF_ETC2_RGB"): return ue4_utexture2d::pixel_format::PF_ETC2_RGB;
		case fnv("PF_ETC2_RGBA"): return ue4_utexture2d::pixel_format::PF_ETC2_RGBA;
		case fnv("PF_R32G32B32A32_UINT"): return ue4_utexture2d::pixel_format::PF_R32G32B32A32_UINT;
		case fnv("PF_R16G16_UINT"): return ue4_utexture2d::pixel_format::PF_R16G16_UINT;
		case fnv("PF_ASTC_4x4"): return ue4_utexture2d::pixel_format::PF_ASTC_4x4;
		case fnv("PF_ASTC_6x6"): return ue4_utexture2d::pixel_format::PF_ASTC_6x6;
		case fnv("PF_ASTC_8x8"): return ue4_utexture2d::pixel_format::PF_ASTC_8x8;
		case fnv("PF_ASTC_10x10"): return ue4_utexture2d::pixel_format::PF_ASTC_10x10;
		case fnv("PF_ASTC_12x12"): return ue4_utexture2d::pixel_format::PF_ASTC_12x12;
		case fnv("PF_BC6H"): return ue4_utexture2d::pixel_format::PF_BC6H;
		case fnv("PF_BC7"): return ue4_utexture2d::pixel_format::PF_BC7;
		case fnv("PF_R8_UINT"): return ue4_utexture2d::pixel_format::PF_R8_UINT;
		case fnv("PF_L8"): return ue4_utexture2d::pixel_format::PF_L8;
		case fnv("PF_XGXR8"): return ue4_utexture2d::pixel_format::PF_XGXR8;
		case fnv("PF_R8G8B8A8_UINT"): return ue4_utexture2d::pixel_format::PF_R8G8B8A8_UINT;
		case fnv("PF_R8G8B8A8_SNORM"): return ue4_utexture2d::pixel_format::PF_R8G8B8A8_SNORM;
		case fnv("PF_R16G16B16A16_UNORM"): return ue4_utexture2d::pixel_format::PF_R16G16B16A16_UNORM;
		case fnv("PF_R16G16B16A16_SNORM"): return ue4_utexture2d::pixel_format::PF_R16G16B16A16_SNORM;
		case fnv("PF_PLATFORM_HDR_0"): return ue4_utexture2d::pixel_format::PF_PLATFORM_HDR_0;
		case fnv("PF_PLATFORM_HDR_1"): return ue4_utexture2d::pixel_format::PF_PLATFORM_HDR_1;
		case fnv("PF_PLATFORM_HDR_2"): return ue4_utexture2d::pixel_format::PF_PLATFORM_HDR_2;
		case fnv("PF_NV12"): return ue4_utexture2d::pixel_format::PF_NV12;
		case fnv("PF_R32G32_UINT"): return ue4_utexture2d::pixel_format::PF_R32G32_UINT;
		case fnv("PF_ETC2_R11_EAC"): return ue4_utexture2d::pixel_format::PF_ETC2_R11_EAC;
		case fnv("PF_ETC2_RG11_EAC"): return ue4_utexture2d::pixel_format::PF_ETC2_RG11_EAC;
		case fnv("PF_MAX"): return ue4_utexture2d::pixel_format::PF_MAX;
		}
	};

	bool ue4_utexture2d::open(ue4_uexp& a_uexp, ue4_bin_file* a_ubulk, size_t& a_offset) noexcept
	{
		reset();
		VALUNPAK_REQUIRE(ue4_bin_file::open(a_uexp, a_offset));

		size_t offset = 0;
		VALUNPAK_REQUIRE(m_object.open(a_uexp, offset)); // Open the UObject header

		if (read_internal(a_uexp, a_ubulk, offset) == false) // Read the texture
		{
			reset();
			return false;
		}

		a_offset += offset;
		return true;
	}

	void ue4_utexture2d::reset()
	{
		memset(&m_header, 0, sizeof(m_header));
	}

	bool ue4_utexture2d::read_internal(ue4_uexp& a_uexp, ue4_bin_file* a_ubulk, size_t& a_offset)
	{
		VALUNPAK_REQUIRE(read(m_header, a_offset));
		if (!m_header.is_cooked)
			return a_offset;

		std::string pixel_format_name;
		while (true)
		{
			VALUNPAK_REQUIRE(a_uexp.read_table_name(pixel_format_name, *this, a_offset));
			if (pixel_format_name == "None")
				break;

			a_offset += sizeof(i64); // SkipOffset

			platform_data_element elem;
			VALUNPAK_REQUIRE(read(elem.header, a_offset));

			// Might as well reuse the previous string...
			VALUNPAK_REQUIRE(read_fstring(pixel_format_name, a_offset));
			elem.format = to_pixel_format(pixel_format_name.c_str());

			a_offset += sizeof(elem.first_mip_to_serialise);
			elem.first_mip_to_serialise = 0;

			// Mip
			i32 mip_count;
			VALUNPAK_REQUIRE(read(mip_count, a_offset));
			for (i32 i = 0; i < mip_count; i++)
			{
				platform_mipmap mip;
				VALUNPAK_REQUIRE(read(mip.is_cooked, a_offset));
				VALUNPAK_REQUIRE(mip.data.open(*this, a_offset));
				VALUNPAK_REQUIRE(read(mip.sizes, a_offset));

				elem.mips.push_back(mip);
			}

			platform_data.push_back(elem);
			a_offset += sizeof(i32);
		}

		return a_offset;
	}
}

