#pragma once

#include <valunpak/config.hpp>
#include <valunpak/ue4_bin_file.hpp>

namespace valunpak
{
	class ue4_ubulk;
	class ue4_bulkdata : public ue4_bin_file
	{
	public:
		ue4_bulkdata() : m_ubulk(nullptr) {}
		ue4_bulkdata(ue4_ubulk* a_ubulk) : m_ubulk(a_ubulk) {}
		bool open(ue4_bin_file& a_parent, ue4_ubulk* a_ubulk, size_t& a_offset) noexcept;
		bool open(ue4_bin_file& a_parent, size_t& a_offset) noexcept;

		const u8* data() const;
		size_t size() const;

		const std::vector<u8>& as_vector() const;

		enum flag_type : i32
		{
			none = 0,
			payload_at_end_of_file = 1 << 0,
			serialize_compressed_zlib = 1 << 1,
			force_single_element_serialization = 1 << 2,
			single_use = 1 << 3,
			unused = 1 << 5,
			force_inline_payload = 1 << 6,
			serialize_compressed = (serialize_compressed_zlib),
			force_stream_payload = 1 << 7,
			payload_in_seperate_file = 1 << 8,
			serialize_compressed_bit_window = 1 << 9,
			force__not_inline_payload = 1 << 10,
			optional_payload = 1 << 11,
			memory_mapped_payload = 1 << 12,
			size_64bit = 1 << 13,
			duplicate_non_optional_payload = 1 << 14,
			bad_data_version = 1 << 15,
			uses_io_dispatcher = 1 << 16,
			data_is_memory_mapped = 1 << 17,
		};

#pragma pack(push, 1)
		struct header
		{
			flag_type flags;
			i32 element_count;
			i32 size_on_disk;
			i64 offset_in_file;
		};
#pragma pack(pop)

	private:
		header m_header;
		std::vector<u8> m_data;
		ue4_ubulk* m_ubulk;

		void reset();
		size_t read_internal();
	};
}
