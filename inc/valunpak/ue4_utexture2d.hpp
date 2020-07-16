#pragma once

#include <valunpak/config.hpp>
#include <valunpak/ue4_bin_file.hpp>
#include <valunpak/ue4_uobject.hpp>
#include <valunpak/ue4_bulkdata.hpp>
#include <valunpak/vector_getter.hpp>

namespace valunpak
{
	class ue4_uexp;
	class ue4_utexture2d : public ue4_bin_file
	{
	public:
		bool open(ue4_uexp& a_uexp, ue4_bin_file* a_ubulk, size_t& a_offset) noexcept;
		bool to_file(const char* a_file_name) const noexcept override;

		// Doesn't match code format due to string compatibility
		enum class pixel_format
		{
			PF_Unknown = 0,
			PF_A32B32G32R32F = 1,
			PF_B8G8R8A8 = 2,
			PF_G8 = 3,
			PF_G16 = 4,
			PF_DXT1 = 5,
			PF_DXT3 = 6,
			PF_DXT5 = 7,
			PF_UYVY = 8,
			PF_FloatRGB = 9,
			PF_FloatRGBA = 10,
			PF_DepthStencil = 11,
			PF_ShadowDepth = 12,
			PF_R32_FLOAT = 13,
			PF_G16R16 = 14,
			PF_G16R16F = 15,
			PF_G16R16F_FILTER = 16,
			PF_G32R32F = 17,
			PF_A2B10G10R10 = 18,
			PF_A16B16G16R16 = 19,
			PF_D24 = 20,
			PF_R16F = 21,
			PF_R16F_FILTER = 22,
			PF_BC5 = 23,
			PF_V8U8 = 24,
			PF_A1 = 25,
			PF_FloatR11G11B10 = 26,
			PF_A8 = 27,
			PF_R32_UINT = 28,
			PF_R32_SINT = 29,
			PF_PVRTC2 = 30,
			PF_PVRTC4 = 31,
			PF_R16_UINT = 32,
			PF_R16_SINT = 33,
			PF_R16G16B16A16_UINT = 34,
			PF_R16G16B16A16_SINT = 35,
			PF_R5G6B5_UNORM = 36,
			PF_R8G8B8A8 = 37,
			PF_A8R8G8B8 = 38,
			PF_BC4 = 39,
			PF_R8G8 = 40,
			PF_ATC_RGB = 41,
			PF_ATC_RGBA_E = 42,
			PF_ATC_RGBA_I = 43,
			PF_X24_G8 = 44,
			PF_ETC1 = 45,
			PF_ETC2_RGB = 46,
			PF_ETC2_RGBA = 47,
			PF_R32G32B32A32_UINT = 48,
			PF_R16G16_UINT = 49,
			PF_ASTC_4x4 = 50,
			PF_ASTC_6x6 = 51,
			PF_ASTC_8x8 = 52,
			PF_ASTC_10x10 = 53,
			PF_ASTC_12x12 = 54,
			PF_BC6H = 55,
			PF_BC7 = 56,
			PF_R8_UINT = 57,
			PF_L8 = 58,
			PF_XGXR8 = 59,
			PF_R8G8B8A8_UINT = 60,
			PF_R8G8B8A8_SNORM = 61,
			PF_R16G16B16A16_UNORM = 62,
			PF_R16G16B16A16_SNORM = 63,
			PF_PLATFORM_HDR_0 = 64,
			PF_PLATFORM_HDR_1 = 65,
			PF_PLATFORM_HDR_2 = 66,
			PF_NV12 = 67,
			PF_R32G32_UINT = 68,
			PF_ETC2_R11_EAC = 69,
			PF_ETC2_RG11_EAC = 70,
			PF_MAX = 71,
		};

	#pragma pack(push, 1)
		struct header
		{
			u8 flags[4];
			i32 is_cooked;
		};

		struct platform_data_header
		{
			i32 width;
			i32 height;
			i32 slice_count;
		};

		struct platform_mip_map_footer
		{
			i32 width;
			i32 height;
			i32 slice_count;
		};
	#pragma pack(pop)

		struct platform_mipmap
		{
			i32 is_cooked;
			ue4_bulkdata data;
			platform_mip_map_footer sizes;
		};

		struct platform_data_element
		{
			platform_data_header header;
			pixel_format format;
			std::vector<platform_mipmap> mips;
			i32 first_mip_to_serialise;
		};

		using platform_data_array = std::vector<platform_data_element>;
		VALUNPAK_VECTOR_GETTER(ue4_utexture2d, platform_data_array, m_platform_data) platform_data;

	private:
		ue4_uobject m_object;
		header m_header;
		platform_data_array m_platform_data;
		
		void reset();
		bool read_internal(ue4_uexp& a_uexp, ue4_bin_file* a_ubulk, size_t& a_offset);
	};
}
