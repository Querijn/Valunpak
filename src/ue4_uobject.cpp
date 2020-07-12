#include <valunpak/ue4_uobject.hpp>
#include <valunpak/ue4_uexp.hpp>
#include <valunpak/json_writer.hpp>

#include <debugbreak.h>

namespace valunpak
{
	struct struct_property : public ue4_uobject::base_property
	{
		enum class type
		{
			unknown = 0,
			int_point = fnv("IntPoint"),
			level_sequence_object_reference_map = fnv("LevelSequenceObjectReferenceMap"),
			gameplay_tag_container = fnv("GameplayTagContainer"),
			nav_agent_selector = fnv("NavAgentSelector"),
			quat = fnv("Quat"),
			vector_4 = fnv("Vector4"),
			vector_2d = fnv("Vector2D"),
			box_2d = fnv("Box2D"),
			box = fnv("Box"),
			vector = fnv("Vector"),
			rotator = fnv("Rotator"),
			guid = fnv("Guid"),
			soft_object_path = fnv("SoftObjectPath"),
			soft_class_path = fnv("SoftClassPath"),
			color = fnv("Color"),
			linear_color = fnv("LinearColor"),
			simple_curve_key = fnv("SimpleCurveKey"),
			rich_curve_key = fnv("RichCurveKey"),
			frame_number = fnv("FrameNumber"),
			smart_name = fnv("SmartName"),
			per_platform_float = fnv("PerPlatformFloat"),
			per_platform_int = fnv("PerPlatformInt"),
			date_time = fnv("DateTime"),
			timespan = fnv("Timespan"),
			movie_scene_track_identifier = fnv("MovieSceneTrackIdentifier"),
			movie_scene_segment_identifier = fnv("MovieSceneSegmentIdentifier"),
			movie_scene_sequence_i_d = fnv("MovieSceneSequenceID"),
			movie_scene_segment = fnv("MovieSceneSegment"),
			section_evaluation_data_tree = fnv("SectionEvaluationDataTree"),
			movie_scene_frame_range = fnv("MovieSceneFrameRange"),
			movie_scene_evaluation_key = fnv("MovieSceneEvaluationKey"),
			movie_scene_float_value = fnv("MovieSceneFloatValue"),
			movie_scene_float_channel = fnv("MovieSceneFloatChannel"),
			movie_scene_evaluation_template = fnv("MovieSceneEvaluationTemplate"),
			skeletal_mesh_sampling_l_o_d_built_data = fnv("SkeletalMeshSamplingLODBuiltData"),
			vector_material_input = fnv("VectorMaterialInput"),
			color_material_input = fnv("ColorMaterialInput"),
			expression_input = fnv("ExpressionInput"),
		};

#pragma pack(push, 1)
		struct base_struct_value_element {};

		struct int_point_element : base_struct_value_element
		{
			i32 x, y;
		};

		struct guid_element : base_struct_value_element
		{
			i32 num[4];
		};
#pragma pack(pop)

		std::string name;
		u32 guid[4] = { 0, 0, 0, 0 };

		type value_type = type::unknown;
		std::unique_ptr<base_struct_value_element> value = nullptr;
	};

	std::string to_json(struct_property::int_point_element& a_int_point)
	{
		return to_json
		({
			{ "x", (i64)a_int_point.x },
			{ "y", (i64)a_int_point.y }
			});
	}

	std::string to_json(struct_property::guid_element& a_guid)
	{
		return to_json
		(
			std::to_string(a_guid.num[0]) + "-" +
			std::to_string(a_guid.num[1]) + "-" +
			std::to_string(a_guid.num[2]) + "-" +
			std::to_string(a_guid.num[3])
		);
	}

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
		reset();
		m_parent = &a_parent;
		VALUNPAK_REQUIRE(bin_file::open(a_parent, a_offset));

		size_t offset = read_internal();
		if (offset == 0)
		{
			reset();
			return false;
		}

		a_offset += offset;
		return true;
	}

	void ue4_uobject::reset()
	{
		m_props.clear();
	}

	ue4_uobject::read_tag_result_type ue4_uobject::read_tag(ue4_uobject::property_tag& a_tag, size_t& a_offset)
	{
		VALUNPAK_REQUIRE_RET(m_parent->read_table_name(a_tag.name, *this, a_offset), read_tag_result_type::failed);
		if (a_tag.name == "None")
			return read_tag_result_type::no_entry; // This is fine, basically EOF

		std::string type_name;
		i32 type_number;
		VALUNPAK_REQUIRE_RET(m_parent->read_table_name(type_name, type_number, *this, a_offset), read_tag_result_type::failed);

		a_tag.type = (property_type)fnv(type_name.c_str());

		VALUNPAK_REQUIRE_RET(read(a_tag.size, a_offset) && read(a_tag.array_index, a_offset), read_tag_result_type::failed);

		auto& prop = m_props[a_tag.name];
		if (type_number == 0)
		{
			switch (a_tag.type)
			{
			case property_type::struct_property:
			{
				std::unique_ptr<struct_property> struct_prop = std::make_unique<struct_property>();
				VALUNPAK_REQUIRE_RET(m_parent->read_table_name(struct_prop->name, *this, a_offset), read_tag_result_type::failed);
				VALUNPAK_REQUIRE_RET(read_array(struct_prop->guid, 4, a_offset), read_tag_result_type::failed);
				prop = std::move(struct_prop);
				break;
			}

			case property_type::bool_property:
			{
				std::unique_ptr<bool_property> bool_prop = std::make_unique<bool_property>();
				VALUNPAK_REQUIRE_RET(read(bool_prop->value, a_offset), read_tag_result_type::failed);
				prop = std::move(bool_prop);
				break;
			}

			case property_type::byte_property:
			case property_type::enum_property:
			{
				std::unique_ptr<enum_property> bool_prop = std::make_unique<enum_property>();
				VALUNPAK_REQUIRE_RET(m_parent->read_table_name(bool_prop->name, *this, a_offset), read_tag_result_type::failed);
				prop = std::move(bool_prop);
				break;
			}

			case property_type::array_property:
			case property_type::set_property:
			{
				std::unique_ptr<enum_property> enum_prop = std::make_unique<enum_property>();
				VALUNPAK_REQUIRE_RET(m_parent->read_table_name(enum_prop->name, *this, a_offset), read_tag_result_type::failed);
				prop = std::move(enum_prop);
				break;
			}

			case property_type::map_property:
			{
				std::unique_ptr<map_property> map_prop = std::make_unique<map_property>();
				VALUNPAK_REQUIRE_RET(m_parent->read_table_name(map_prop->key_name, *this, a_offset), read_tag_result_type::failed);
				VALUNPAK_REQUIRE_RET(m_parent->read_table_name(map_prop->value_name, *this, a_offset), read_tag_result_type::failed);
				prop = std::move(map_prop);
				break;
			}

			default:
			{
				debug_break();
			}

			}
		}

		VALUNPAK_REQUIRE_RET(read(prop->has_property_guid, a_offset), read_tag_result_type::failed);

		if (prop->has_property_guid)
			VALUNPAK_REQUIRE_RET(read_array(prop->property_guid, 4, a_offset), read_tag_result_type::failed);

		a_tag.prop = prop.get();
		return read_tag_result_type::succeeded;
	}
	
	bool ue4_uobject::read_struct_property(ue4_uobject::property_tag& a_tag, size_t& a_offset)
	{
		auto prop = reinterpret_cast<struct_property*>(a_tag.prop);
		prop->value_type = (struct_property::type)(fnv(prop->name.c_str()));

		// Basically a simple read was necessary for these cases.
#define default_case(a_type_enum, a_elem) \
		case struct_property::type::a_type_enum:\
		{\
			auto t = std::make_unique<struct_property::a_elem>();\
			if (read(*t, a_offset) == false)\
				return false;\
			prop->value = std::move(t);\
			break;\
		}

		switch (prop->value_type)
		{
			default_case(int_point, int_point_element);
			default_case(guid, guid_element);

		default:
			debug_break();
			return false;
		};

#undef default_case
	}

	bool ue4_uobject::read_property(ue4_uobject::property_tag& a_tag, size_t& a_offset)
	{
		switch (a_tag.type)
		{
		case property_type::struct_property:
		{
			if (read_struct_property(a_tag, a_offset) == false)
				return false;
			break;
		}

		// These have been read in the tag already.
		case property_type::bool_property:
			break;

		case property_type::byte_property:
		case property_type::enum_property:
		case property_type::array_property:
		case property_type::set_property:
		case property_type::map_property:
			debug_break();
			return false;

		}

		return true;
	}

	size_t ue4_uobject::read_internal()
	{
		size_t offset = 0;

		while (true)
		{
			property_tag tag;
			auto tag_result = read_tag(tag, offset);
			if (tag_result == read_tag_result_type::failed)
				return 0;
			else if (tag_result == read_tag_result_type::no_entry)
				break; // We're done

			VALUNPAK_REQUIRE_RET(read_property(tag, offset), 0);
		}

		i32 has_guid;
		VALUNPAK_REQUIRE_RET(read(has_guid, offset), 0);
		if (has_guid)
			offset += sizeof(i32) * 4;
		return offset;
	}
}

