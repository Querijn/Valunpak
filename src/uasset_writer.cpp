#include <valunpak/uasset_writer.hpp>

#include <valunpak/pak_filesystem.hpp>
#include <valunpak/ue4_uasset.hpp>
#include <valunpak/ue4_utexture2d.hpp>
#include <valunpak/ue4_ubulk.hpp>
#include <valunpak/ue4_uexp.hpp>
#include <valunpak/image_writer.hpp>
#include <valunpak/uasset_writer.hpp>

#include <filesystem>
namespace fs = std::filesystem;

namespace valunpak
{
	bool should_read(const std::string& name)
	{
		static bool start = false;
		if (!start && name != "ShooterGame/Content/Equippables/Gun_UIData_Base")
			return false;
		else
			start = true;
		return start;
	}

	bool write_uasset(const std::string& name, pak_filesystem& paks, const std::vector<std::string>& files_to_skip)
	{
		// Check if we need to skip this file.
		if (std::find(files_to_skip.begin(), files_to_skip.end(), name) != files_to_skip.end())
			return true;

		if (should_read(name) == false)
			return true;

		printf("Unpacking %s...\n", name.c_str());

		auto uasset_pak_entry = paks.get_file(name + ".uasset");
		VALUNPAK_REQUIRE(uasset_pak_entry.first);

		ue4_uasset uasset_bin;
		VALUNPAK_REQUIRE(uasset_pak_entry.first->get_file_data(*uasset_pak_entry.second, &uasset_bin));

		// Ubulk is optional
		auto ubulk_pak_entry = paks.get_file(name + ".ubulk");
		ue4_ubulk ubulk_bin;
		if (ubulk_pak_entry.first != nullptr)
			VALUNPAK_REQUIRE(ubulk_pak_entry.first->get_file_data(*ubulk_pak_entry.second, &ubulk_bin));
		else
		{
			ubulk_pak_entry = paks.get_file(name + ".m.ubulk");
			if (ubulk_pak_entry.first != nullptr)
				VALUNPAK_REQUIRE(ubulk_pak_entry.first->get_file_data(*ubulk_pak_entry.second, &ubulk_bin));
		}

		auto uexp_pak_entry = paks.get_file(name + ".uexp");
		VALUNPAK_REQUIRE(uexp_pak_entry.first);

		ue4_uexp uexp_bin(uasset_bin, ubulk_pak_entry.first ? &ubulk_bin : nullptr);
		if (uexp_pak_entry.first->get_file_data(*uexp_pak_entry.second, &uexp_bin) == false)
		{
			printf("%s failed to unpack.\n", name.c_str());
		}

		for (auto& file : uexp_bin.files)
		{
			if (file->is_texture())
			{
				fs::path output_name = fs::path("output") / (name + ".png");
				const ue4_utexture2d& texture = *(ue4_utexture2d*)file.get();
				write_image(texture, output_name);
			}
		}
		return true;
	}
}