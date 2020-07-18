#pragma once

#include <valunpak/config.hpp>
#include <valunpak/ue4_base.hpp>
#include <valunpak/fnv1.hpp>

#include <map>

namespace valunpak
{
	class ue4_uexp;
	class ue4_uobject : public ue4_base
	{
	public:
		bool open(ue4_uexp& a_parent, size_t& a_offset);
		bool open(ue4_uexp& a_uexp, ue4_bin_file& a_parent, size_t& a_offset);

		class base_property
		{
		public:
			bool has_property_guid;
			u32 property_guid[4] = { 0 };

			virtual bool is_boolean() const { return false; }
			virtual bool get_bool_value(bool& a_out) const { a_out = false; return false; }
		};

		const base_property* get_prop(const char* a_name) const noexcept;

	private:
		void reset();
		size_t read_internal(bool a_is_element);

		enum class read_tag_result_type
		{
			failed,
			succeeded,
			no_entry,
		};

		enum class property_type : u64
		{
			byte_property = fnv("ByteProperty"),
			bool_property = fnv("BoolProperty"),
			int_property = fnv("IntProperty"),
			float_property = fnv("FloatProperty"),
			object_property = fnv("ObjectProperty"),
			name_property = fnv("NameProperty"),
			delegate_property = fnv("DelegateProperty"),
			double_property = fnv("DoubleProperty"),
			array_property = fnv("ArrayProperty"),
			struct_property = fnv("StructProperty"),
			str_property = fnv("StrProperty"),
			text_property = fnv("TextProperty"),
			interface_property = fnv("InterfaceProperty"),
			softobject_property = fnv("SoftObjectProperty"),
			uint64_property = fnv("UInt64Property"),
			uint32_property = fnv("UInt32Property"),
			uint16_property = fnv("UInt16Property"),
			int16_property = fnv("Int16Property"),
			int8_property = fnv("Int8Property"),
			map_property = fnv("MapProperty"),
			set_property = fnv("SetProperty"),
			enum_property = fnv("EnumProperty"),
		};

		struct property_tag
		{
			property_tag(property_type a_type) : type(a_type) {}
			property_tag() {}
			std::string name;
			property_type type;

			i32 size;
			i32 array_index;

			base_property* prop;
		};

		enum class property_read_mode
		{
			default_mode,
			array_mode
		};

		read_tag_result_type read_tag(property_tag& a_tag, size_t& a_offset, property_read_mode a_read_mode, bool a_should_read = true);

		bool read_struct_property(property_tag& a_tag, size_t& a_offset);
		bool read_property(property_tag& a_tag, property_type a_type, size_t& a_offset, property_read_mode a_read_mode);
		std::unique_ptr<base_property> make_property(property_type a_type);

		ue4_uexp* m_uexp = nullptr;
		std::map<std::string, std::unique_ptr<base_property>> m_props;
	};
}
