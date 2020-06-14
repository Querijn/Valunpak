#pragma once

#include <valunpak/config.hpp>
#include <valunpak/ue4_bin_file.hpp>

#include <map>

namespace valunpak
{
	class ue4_uexp;
	class ue4_uobject : public ue4_bin_file
	{
	public:
		bool open(ue4_uexp& a_parent, size_t& a_offset);

		class base_property
		{
		public:
			bool has_property_guid;
			u32 property_guid[4] = { 0 };
		};

	private:
		void reset();
		size_t read_internal();

		ue4_uexp* m_parent = nullptr;
		std::map<std::string, std::unique_ptr<base_property>> m_props;
	};
}
