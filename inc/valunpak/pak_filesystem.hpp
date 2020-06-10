#pragma once

#include <valunpak/config.hpp>
#include <valunpak/pak_file.hpp>

#include <vector>
#include <memory>
#include <string_view>
#include <map>
#include <filesystem>

namespace valunpak
{
	class aes;
	class pak_filesystem
	{
	public:
		using key_array = std::vector<std::unique_ptr<aes>>;
		using file_entry_pair = std::pair<std::shared_ptr<pak_file>, std::shared_ptr<pak_file::entry>>;
		pak_filesystem();
		~pak_filesystem();

		void open(const std::filesystem::path& a_root);
		bool empty() const;

		const file_entry_pair get_file(const std::filesystem::path& a_path) const;

		struct key_getter
		{
			void add(const std::vector<u8>& a_key);
			key_array::const_iterator begin() const;
			key_array::const_iterator end() const;
		};

		static key_getter keys;
		friend struct key_getter;
	private:
		static key_array m_keys;

		std::filesystem::path m_root;
		std::vector<std::shared_ptr<pak_file>> m_files;
		std::map<std::filesystem::path, std::unique_ptr<file_entry_pair>> m_entries;

		bool add(std::string_view a_file_name, bin_file::read_mode_type a_read_mode = bin_file::read_mode_type::stream) noexcept;
	};
}
