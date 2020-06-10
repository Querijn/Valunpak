#include <valunpak/pak_filesystem.hpp>
#include <valunpak/aes.hpp>

namespace valunpak
{
	namespace fs = std::filesystem;
	pak_filesystem::key_array pak_filesystem::m_keys;
	pak_filesystem::key_getter pak_filesystem::keys;

	pak_filesystem::pak_filesystem() { }
	pak_filesystem::~pak_filesystem() { }

	void pak_filesystem::open(const std::filesystem::path& a_root)
	{
		m_root = a_root;
		m_entries.clear();
		m_files.clear();

		valunpak::pak_file pak;
		for (auto& dir_entry : fs::recursive_directory_iterator(a_root))
		{
			auto path = dir_entry.path();
			if (path.has_extension() == false || path.extension() != ".pak")
				continue;

			add(path.generic_string());
		}
	}

	bool pak_filesystem::empty() const
	{
		return m_files.empty();
	}

	const pak_filesystem::file_entry_pair pak_filesystem::get_file(const std::filesystem::path& a_path) const
	{
		static file_entry_pair empty_pair = std::make_pair(nullptr, nullptr);

		auto index = m_entries.find(a_path);
		if (index == m_entries.end())
			return empty_pair;

		return *index->second;
	}

	bool pak_filesystem::add(std::string_view a_file_name, bin_file::read_mode_type a_read_mode) noexcept
	{
		static std::shared_ptr<pak_file> file = std::make_shared<pak_file>();
		if (file->open(a_file_name, a_read_mode) == false)
			return false;

		m_files.push_back(file);
		auto& pak = m_files.back();
		file = std::make_shared<pak_file>(); // Generate new ptr for next entry

		fs::path root = fs::absolute(m_root);
		fs::path mount_point = fs::relative(pak->get_mount_point(), root);
		for (auto& entry_pair : *pak)
			m_entries[mount_point / entry_pair.first] = std::make_unique<file_entry_pair>(pak, entry_pair.second);
		return true;
	}
	
	void pak_filesystem::key_getter::add(const std::vector<u8>& a_key)
	{
		return pak_filesystem::m_keys.push_back(std::make_unique<aes>(a_key.data()));
	}

	pak_filesystem::key_array::const_iterator pak_filesystem::key_getter::begin() const
	{
		return pak_filesystem::m_keys.begin();
	}

	pak_filesystem::key_array::const_iterator pak_filesystem::key_getter::end() const
	{
		return pak_filesystem::m_keys.end();
	}
}
