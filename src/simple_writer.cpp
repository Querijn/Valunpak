#include <valunpak/simple_writer.hpp>

#include <valunpak/pak_filesystem.hpp>
#include <valunpak/ue4_uobject.hpp>

#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

namespace valunpak
{
	bool write_default_file(const std::string& name, pak_filesystem& paks, const std::vector<std::string>& files_to_skip)
	{
		printf("Unpacking %s...\n", name.c_str());

		auto pak_entry = paks.get_file(name);
		VALUNPAK_REQUIRE(pak_entry.first);

		pak_entry.second->header.hash;

		fs::create_directories(fs::path(name).parent_path());
		std::vector<u8> data;
		VALUNPAK_REQUIRE(pak_entry.first->get_file_data(*pak_entry.second, data));

		std::ofstream output_file(name, std::ofstream::binary);
		output_file.write((char*)data.data(), data.size());

		return output_file.good();
	}
}