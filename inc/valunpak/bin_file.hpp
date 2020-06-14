#pragma once

#include <valunpak/config.hpp>

#include <string_view>
#include <string>
#include <memory>
#include <vector>

namespace valunpak
{
	struct base_file_impl;
	class bin_file
	{
	public:
		bin_file();
		~bin_file();

		enum class read_mode_type
		{
			stream,
			in_memory,
		};

		virtual bool open(std::string_view a_file_name, read_mode_type a_read_mode = read_mode_type::stream) noexcept;
		virtual bool open(const std::vector<u8>& a_data) noexcept;
		virtual bool open(const u8* a_data, size_t a_size) noexcept;
		virtual bool open(bin_file& a_reader, size_t& a_offset) noexcept;

		bool read_buffer(char* a_buffer, size_t a_length, size_t& a_offset) const;
		bool read_string(std::string& a_string, size_t a_size, size_t& a_offset) const;
		size_t get_size() const;

		bool find_buffer(const u8* a_buffer, size_t a_size, size_t& a_offset) const;

		template<typename T>
		bool read(T& a_type, size_t& a_offset) const
		{
			return read_buffer(reinterpret_cast<char*>(&a_type), sizeof(T), a_offset);
		}

		template<typename T>
		bool read_array(T* a_array, size_t a_count, size_t& a_offset) const
		{
			return read_buffer(reinterpret_cast<char*>(a_array), a_count * sizeof(T), a_offset);
		}

		template<typename T>
		bool read_array(std::vector<T>& a_vector, size_t a_count, size_t& a_offset) const
		{
			a_vector.resize(a_count);
			if (read_array(a_vector.data(), a_count, a_offset)) // Use regular pointer array functionality.
				return true;

			a_vector.clear();
			return false;
		}

		template<typename T>
		bool find(T pod, size_t& a_offset) const
		{
			return find_buffer(reinterpret_cast<u8*>(&pod), sizeof(T), a_offset);
		}

	protected:
		std::string m_file_name = "";
		std::shared_ptr<base_file_impl> m_impl;
	};
}
