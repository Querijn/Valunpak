#include <valunpak/ue4_uobject.hpp>
#include <valunpak/ue4_uexp.hpp>

#include <debugbreak.h>

namespace valunpak
{
	struct struct_property : public ue4_uobject::base_property
	{
		std::string name;
		u32 guid[4];
	};

	struct bool_property : public ue4_uobject::base_property
	{
		u8 value;
	};

	struct enum_property : public ue4_uobject::base_property
	{
		std::string name;
	};

	struct array_property : public ue4_uobject::base_property
	{
		std::string name;
	};

	struct map_property : public ue4_uobject::base_property
	{
		std::string key_name;
		std::string value_name;
	};

	bool ue4_uobject::open(ue4_uexp& a_parent, size_t& a_offset)
	{
		m_parent = &a_parent;

		if (bin_file::open(a_parent, a_offset) == false)
			return false;

		size_t offset = read_internal();
		a_offset += offset;
		return offset != 0;
	}

	void ue4_uobject::reset()
	{
	}

#pragma optimize("", off)
	size_t ue4_uobject::read_internal()
	{
		return 1; // TODO: Properties are being read, needs to structure the data.

		size_t offset = 0;

		while (true)
		{
			std::string name;
			if (m_parent->read_table_name(name, *this, offset) == false)
				return 0;

			if (name == "None")
				return offset; // This is fine

			std::string type_name;
			i32 type_number;
			if (m_parent->read_table_name(type_name, type_number, *this, offset) == false)
				return 0;

			i32 size, array_index;
			if (read(size, offset) == false || read(array_index, offset) == false)
				return 0;

			auto pos = offset;
			i32 guid[4];

			auto& prop = m_props[name];
			if (type_number == 0)
			{
				if (type_name == "StructProperty")
				{
					std::unique_ptr<struct_property> struct_prop = std::make_unique<struct_property>();
					if (m_parent->read_table_name(struct_prop->name, *this, offset) == false)
						return 0;

					if (read_array(struct_prop->guid, 4, offset) == false)
						return 0;

					prop = std::move(struct_prop);
				}
				else if (type_name == "BoolProperty")
				{
					std::unique_ptr<bool_property> bool_prop = std::make_unique<bool_property>();
					if (read(bool_prop->value, offset) == false)
						return 0;

					prop = std::move(bool_prop);
				}
				else if (type_name == "ByteProperty" || type_name == "EnumProperty")
				{
					std::unique_ptr<enum_property> bool_prop = std::make_unique<enum_property>();
					if (m_parent->read_table_name(bool_prop->name, *this, offset) == false)
						return 0;

					prop = std::move(bool_prop);
				}
				else if (type_name == "ArrayProperty" || type_name == "SetProperty")
				{
					std::unique_ptr<enum_property> enum_prop = std::make_unique<enum_property>();
					if (m_parent->read_table_name(enum_prop->name, *this, offset) == false)
						return 0;

					prop = std::move(enum_prop);
				}
				else if (type_name == "MapProperty")
				{
					std::unique_ptr<map_property> map_prop = std::make_unique<map_property>();
					if (m_parent->read_table_name(map_prop->key_name, *this, offset) == false)
						return 0;

					if (m_parent->read_table_name(map_prop->value_name, *this, offset) == false)
						return 0;
					prop = std::move(map_prop);
					break;
				}
				else
				{
					prop = std::make_unique<base_property>();
				}
			}

			if (read(prop->has_property_guid, offset) == false)
				return 0;

			if (prop->has_property_guid)
			{
				if (read_array(prop->property_guid, 4, offset) == false)
					return 0;
			}

			// TODO: Finish this up
		}

		return offset;
	}
}

