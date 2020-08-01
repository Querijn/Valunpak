#include <valunpak/image_writer.hpp>
#include <valunpak/ue4_utexture2d.hpp>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <s3tc.h>
#include <half.hpp>

#include <cmath>
#include <filesystem>
namespace fs = std::filesystem;

namespace valunpak
{
	u8 get_znormal(u8 x, u8 y)
	{
		auto xf = (x / 127.5f) - 1;
		auto yf = (y / 127.5f) - 1;
		auto zval = 1 - xf * xf - yf * yf;
		auto zval_ = (float)sqrt(zval > 0 ? zval : 0);
		zval = zval_ < 1 ? zval_ : 1;
		return (u8)((zval * 127) + 128);
	}

	void get_bc3_indices(u8* bytes, const u8* block, float* ref_sl)
	{
		bytes[0] = (u8)ref_sl[(u8)(block[0] & 0b00000111)];
		bytes[1] = (u8)ref_sl[(u8)((block[0] & 0b00111000) >> 3)];

		bytes[2] = (u8)ref_sl[(u8)(((block[1] & 0b00000001) << 2) | ((block[0] & 0b11 << 6) >> 6))];
		bytes[3] = (u8)ref_sl[(u8)((block[1] & 0b00001110) >> 1)];
		bytes[4] = (u8)ref_sl[(u8)((block[1] & 0b01110000) >> 4)];

		bytes[5] = (u8)ref_sl[(u8)(((block[2] & 0b00000011) << 1) | ((block[1] & 0b1 << 7) >> 7))];
		bytes[6] = (u8)ref_sl[(u8)((block[2] & 0b00011100) >> 2)];
		bytes[7] = (u8)ref_sl[(u8)((block[2] & 0b11100000) >> 5)];
	}

	void decode_bc3_block(u8* bytes, const u8* inp, size_t& a_offset)
	{
		u8 ref0 = inp[a_offset++];
		u8 ref1 = inp[a_offset++];

		float ref_sl[8] = { 0 };
		ref_sl[0] = ref0;
		ref_sl[1] = ref1;

		if (ref0 > ref1)
		{
			ref_sl[2] = (6 * ref_sl[0] + 1 * ref_sl[1]) / 7;
			ref_sl[3] = (5 * ref_sl[0] + 2 * ref_sl[1]) / 7;
			ref_sl[4] = (4 * ref_sl[0] + 3 * ref_sl[1]) / 7;
			ref_sl[5] = (3 * ref_sl[0] + 4 * ref_sl[1]) / 7;
			ref_sl[6] = (2 * ref_sl[0] + 5 * ref_sl[1]) / 7;
			ref_sl[7] = (1 * ref_sl[0] + 6 * ref_sl[1]) / 7;
		}
		else
		{
			ref_sl[2] = (4 * ref_sl[0] + 1 * ref_sl[1]) / 5;
			ref_sl[3] = (3 * ref_sl[0] + 2 * ref_sl[1]) / 5;
			ref_sl[4] = (2 * ref_sl[0] + 3 * ref_sl[1]) / 5;
			ref_sl[5] = (1 * ref_sl[0] + 4 * ref_sl[1]) / 5;
			ref_sl[6] = 0;
			ref_sl[7] = 255;
		}

		const u8* buf_block = inp + a_offset;
		a_offset += 3;
		get_bc3_indices(bytes, buf_block, ref_sl);

		buf_block = inp + a_offset;
		a_offset += 3;
		get_bc3_indices(bytes + 8, buf_block, ref_sl);
	}

	void decode_bc5(std::vector<u8>& ret, const u8* inp, size_t size, size_t width, size_t height)
	{
		size_t offset = 0;
		for (size_t y_block = 0; y_block < height / 4; y_block++)
		{
			size_t y_offset = y_block * 4;
			for (size_t x_block = 0; x_block < width / 4; x_block++)
			{
				size_t x_offset = x_block * 4;
				u8 r_bytes[16];
				u8 g_bytes[16];
				decode_bc3_block(r_bytes, inp, offset);
				decode_bc3_block(g_bytes, inp, offset);

				size_t i = 0;
				for (size_t y = 0; y < 4; y++)
				{
					size_t y_total = (y_offset + y) * width;
					for (size_t x = 0; x < 4; x++)
					{
						size_t x_total = x_offset + x;
						size_t pixel = (y_total + x_total) * 4;

						ret[pixel + 0] = r_bytes[i];
						ret[pixel + 1] = g_bytes[i];
						ret[pixel + 2] = get_znormal(r_bytes[i], g_bytes[i]);
						i++;
					}
				}
			}
		}
	}

	void decode_bc4(std::vector<u8>& ret, const u8* inp, size_t size, size_t width, size_t height)
	{
		size_t offset = 0;
		for (int y_block = 0; y_block < height / 4; y_block++)
		{
			size_t y_offset = y_block * 4;
			for (int x_block = 0; x_block < width / 4; x_block++)
			{
				size_t x_offset = x_block * 4;
				u8 r_bytes[16];
				decode_bc3_block(r_bytes, inp, offset);

				size_t i = 0;
				for (size_t y = 0; y < 4; y++)
				{
					size_t y_total = (y_offset + y) * width;
					for (size_t x = 0; x < 4; x++)
					{
						size_t x_total = x_offset + x;
						size_t pixel = (y_total + x_total) * 4;

						ret[pixel + 0] = 0;
						ret[pixel + 1] = 0;
						ret[pixel + 2] = 0;
						ret[pixel + 3] = r_bytes[i];
						i++;
					}
				}
			}
		}
	}

	void argb2rgba(u8* a_image, size_t a_size)
	{
		for (int i = 0; i < a_size; i += 4)
		{
			uint8_t a = a_image[i + 0];
			uint8_t r = a_image[i + 1];
			uint8_t g = a_image[i + 2];
			uint8_t b = a_image[i + 3];

			a_image[i + 0] = r;
			a_image[i + 1] = g;
			a_image[i + 2] = b;
			a_image[i + 3] = a;
		}
	}

	void bgra2rgba(u8* a_image, size_t a_size)
	{
		for (int i = 0; i < a_size; i += 4)
		{
			uint8_t b = a_image[i + 0];
			uint8_t g = a_image[i + 1];
			uint8_t r = a_image[i + 2];
			uint8_t a = a_image[i + 3];

			a_image[i + 0] = r;
			a_image[i + 1] = g;
			a_image[i + 2] = b;
			a_image[i + 3] = a;
		}
	}

	void g2rgba(u8* a_image, const u8* a_source, size_t a_size)
	{
		size_t img_size = a_size * 4;
		for (size_t i = 0, j = 0; i < img_size; i += 4, j++)
		{
			uint8_t g = a_source[j];

			a_image[i + 0] = g;
			a_image[i + 1] = g;
			a_image[i + 2] = g;
			a_image[i + 3] = 255;
		}
	}

#define half2float(value) half_float::detail::half2float<float>(value)

	void f16rgba2rgba(u8* a_image, const u16* a_source, size_t a_size)
	{
		size_t img_size = a_size * 4;
		for (int i = 0; i < img_size; i += 4)
		{
			float r = half2float(a_source[i + 0]) * 255;
			float g = half2float(a_source[i + 1]) * 255;
			float b = half2float(a_source[i + 2]) * 255;
			float a = half2float(a_source[i + 3]) * 255;

			a_image[i + 0] = r;
			a_image[i + 1] = g;
			a_image[i + 2] = b;
			a_image[i + 3] = a;
		}
	}
	
	bool write_image(const ue4_utexture2d& a_texture, const fs::path& a_output_name)
	{
		VALUNPAK_REQUIRE(a_texture.platform_data.empty() == false);
		auto platform_data = a_texture.platform_data[0];
		auto mip0 = platform_data.mips[0];
		auto& data = mip0.data;
		int comp = 4;

		std::vector<u8> img(mip0.sizes.width * mip0.sizes.height * 4);
		switch (platform_data.format)
		{
		case ue4_utexture2d::pixel_format::PF_DXT5:
			block_decompress_image_dxt5(mip0.sizes.width, mip0.sizes.height, data.data(), reinterpret_cast<unsigned long*>(img.data()));
			argb2rgba(img.data(), img.size());
			break;

		case ue4_utexture2d::pixel_format::PF_DXT1:
			block_decompress_image_dxt1(mip0.sizes.width, mip0.sizes.height, data.data(), reinterpret_cast<unsigned long*>(img.data()));
			argb2rgba(img.data(), img.size());
			break;

		case ue4_utexture2d::pixel_format::PF_B8G8R8A8:
			memcpy(img.data(), data.data(), img.size());
			bgra2rgba(img.data(), img.size());
			break;

		case ue4_utexture2d::pixel_format::PF_G8:
			g2rgba(img.data(), data.data(), mip0.sizes.width * mip0.sizes.height);
			break;

		case ue4_utexture2d::pixel_format::PF_FloatRGBA:
			f16rgba2rgba(img.data(), (const u16*)(data.data()), mip0.sizes.width * mip0.sizes.height);
			break;

		case ue4_utexture2d::pixel_format::PF_BC5:
			decode_bc5(img, data.data(), data.size(), mip0.sizes.width, mip0.sizes.height);
			bgra2rgba(img.data(), img.size());
			break;

		case ue4_utexture2d::pixel_format::PF_BC4:
			decode_bc4(img, data.data(), data.size(), mip0.sizes.width, mip0.sizes.height);
			break;

		default:
			debug_break(); // TODO
			break;
		}

		fs::create_directories(a_output_name.parent_path());
		stbi_write_png(a_output_name.generic_string().c_str(), mip0.sizes.width, mip0.sizes.height, comp, img.data(), 0);
	}
}