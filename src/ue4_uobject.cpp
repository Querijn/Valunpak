#include <valunpak/ue4_uobject.hpp>
#include <valunpak/ue4_uexp.hpp>
#include <valunpak/ue4_uasset.hpp>
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
			vector4 = fnv("Vector4"),
			vector2d = fnv("Vector2D"),
			box2d = fnv("Box2D"),
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
			movie_scene_sequence_id = fnv("MovieSceneSequenceID"),
			movie_scene_segment = fnv("MovieSceneSegment"),
			section_evaluation_data_tree = fnv("SectionEvaluationDataTree"),
			movie_scene_frame_range = fnv("MovieSceneFrameRange"),
			movie_scene_evaluation_key = fnv("MovieSceneEvaluationKey"),
			movie_scene_float_value = fnv("MovieSceneFloatValue"),
			movie_scene_float_channel = fnv("MovieSceneFloatChannel"),
			movie_scene_evaluation_template = fnv("MovieSceneEvaluationTemplate"),
			skeletal_mesh_sampling_lod_built_data = fnv("SkeletalMeshSamplingLODBuiltData"),
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

		struct vector_element : base_struct_value_element
		{
			float x, y, z;
		};

		struct linear_color_element : base_struct_value_element
		{
			float r, g, b, a;
		};

		struct color_element : base_struct_value_element
		{
			u8 r, g, b, a;
		};

		struct box_element : base_struct_value_element
		{
			vector_element min;
			vector_element max;
			bool valid;
		};

		struct quat_element : base_struct_value_element
		{
			float x, y, z, w;
		};

		struct rotator_element : base_struct_value_element
		{
			float pitch;
			float yaw;
			float roll;
		};

		struct material_input_element : base_struct_value_element
		{
			i32 output_index;

			i32 input_name_index; // index from the table
			i32 input_name_number;

			i32 mask;
			i32 mask_r;
			i32 mask_g;
			i32 mask_b;
			i32 mask_a;

			i32 expression_name_index; // index from the table
			i32 expression_name_number;
		};

		struct vector_material_input_element : base_struct_value_element
		{
			material_input_element parent;
			bool use_constant;
			vector_element constant;
			u8 temp;
			u16 temp_type;
		};

		struct color_material_input_element : base_struct_value_element
		{
			material_input_element parent;
			bool use_constant;
			linear_color_element constant;
			u8 temp;
			u16 temp_type;
		};

		struct expression_input_element : material_input_element {};

#pragma pack(pop)

		struct uobject_element : base_struct_value_element
		{
			ue4_uobject object;
		};

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

	struct byte_base_property : public ue4_uobject::base_property
	{
		std::string name;
	};

	struct byte_property		: public byte_base_property				{ std::string value; };
	struct byte_array_property	: public byte_base_property				{ u8 value; };

	struct bool_property		: public ue4_uobject::base_property		{ u8 value; };
	struct name_property		: public ue4_uobject::base_property		{ std::string value; };
	struct float_property		: public ue4_uobject::base_property		{ float value; };
	struct int_property			: public ue4_uobject::base_property		{ i32 value; };

	struct int16_property : public ue4_uobject::base_property { i16 value; };
	struct int64_property : public ue4_uobject::base_property { i64 value; };

	struct uint16_property : public ue4_uobject::base_property { u16 value; };
	struct uint32_property : public ue4_uobject::base_property { u32 value; };
	struct uint64_property : public ue4_uobject::base_property { u64 value; };

	struct object_property : public ue4_uobject::base_property
	{
		ue4_uasset::package_index value;
	};

	struct array_property : public ue4_uobject::base_property
	{
		std::string name;
	};

	struct set_property : public ue4_uobject::base_property
	{
		std::string name;
	};

	struct map_property : public ue4_uobject::base_property
	{
		std::string key_name;
		std::string value_name;
	};

	bool ue4_uobject::open(ue4_uexp& a_uexp, size_t& a_offset)
	{
		reset();
		m_uexp = &a_uexp;
		VALUNPAK_REQUIRE(bin_file::open(a_uexp, a_offset));

		size_t offset = read_internal(false);
		if (offset == 0)
		{
			reset();
			return false;
		}

		a_offset += offset;
		return true;
	}

	bool ue4_uobject::open(ue4_uexp& a_uexp, ue4_bin_file& a_parent, size_t& a_offset)
	{
		reset();
		m_uexp = &a_uexp;
		size_t offset = a_offset;
		VALUNPAK_REQUIRE(bin_file::open(a_parent, offset));

		offset = read_internal(true);
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

	std::unique_ptr<ue4_uobject::base_property> ue4_uobject::make_property(property_type a_type)
	{
#define CONSTRUCT_CASE(a_elem) case property_type::a_elem: \
		return std::make_unique<a_elem>();

		switch (a_type)
		{
			CONSTRUCT_CASE(byte_property);
			CONSTRUCT_CASE(bool_property);
			CONSTRUCT_CASE(int_property);
			CONSTRUCT_CASE(float_property);
			CONSTRUCT_CASE(object_property);
			CONSTRUCT_CASE(name_property);
			CONSTRUCT_CASE(array_property);
			CONSTRUCT_CASE(map_property);
			CONSTRUCT_CASE(set_property);

			CONSTRUCT_CASE(struct_property);
			/*CONSTRUCT_CASE(str_property);
			CONSTRUCT_CASE(text_property);
			CONSTRUCT_CASE(interface_property);
			CONSTRUCT_CASE(softobject_property);
			CONSTRUCT_CASE(uint64_property);
			CONSTRUCT_CASE(uint32_property);
			CONSTRUCT_CASE(uint16_property);
			CONSTRUCT_CASE(int16_property);
			CONSTRUCT_CASE(int8_property);
			CONSTRUCT_CASE(enum_property);
			CONSTRUCT_CASE(delegate_property);
			CONSTRUCT_CASE(double_property);*/

		default:
			VALUNPAK_REQUIRE_RET(false, nullptr);
		};
	}

	ue4_uobject::read_tag_result_type ue4_uobject::read_tag(property_tag& a_tag, size_t& a_offset, property_read_mode a_read_mode, bool a_should_read)
	{
		size_t start_offset = a_offset;

		i32 type_number = 0;
		if (a_should_read)
		{
			VALUNPAK_REQUIRE_RET(m_uexp->read_table_name(a_tag.name, *this, a_offset), read_tag_result_type::failed);
			if (a_tag.name == "None")
				return read_tag_result_type::no_entry; // This is fine, basically EOF

			std::string type_name;
			VALUNPAK_REQUIRE_RET(m_uexp->read_table_name(type_name, type_number, *this, a_offset), read_tag_result_type::failed);

			a_tag.type = (property_type)fnv(type_name.c_str());

			VALUNPAK_REQUIRE_RET(read(a_tag.size, a_offset), read_tag_result_type::failed);
			VALUNPAK_REQUIRE_RET(read(a_tag.array_index, a_offset), read_tag_result_type::failed);
		}

#define SIMPLE_CONSTRUCT_CASE(a_type) \
			case property_type::a_type: \
			prop = std::make_unique<a_type>(); \
			break;

		auto& prop = m_props[a_tag.name];
		if (type_number == 0)
		{
			switch (a_tag.type)
			{
			case property_type::struct_property:
			{
				std::unique_ptr<struct_property> struct_prop = std::make_unique<struct_property>();
				VALUNPAK_REQUIRE_RET(m_uexp->read_table_name(struct_prop->name, *this, a_offset), read_tag_result_type::failed);
				VALUNPAK_REQUIRE_RET(!a_should_read || read_array(struct_prop->guid, 4, a_offset), read_tag_result_type::failed);
				prop = std::move(struct_prop);
				break;
			}

			case property_type::bool_property:
			{
				std::unique_ptr<bool_property> bool_prop = std::make_unique<bool_property>();
				VALUNPAK_REQUIRE_RET(!a_should_read || read(bool_prop->value, a_offset), read_tag_result_type::failed);
				prop = std::move(bool_prop);
				break;
			}

			case property_type::byte_property:
			{
				std::unique_ptr<byte_base_property> byte_prop;
				if (a_read_mode == property_read_mode::default_mode)
					byte_prop = std::make_unique<byte_property>();
				else if (a_read_mode == property_read_mode::array_mode)
					byte_prop = std::make_unique<byte_array_property>();
				else
					VALUNPAK_REQUIRE_RET(false, read_tag_result_type::failed); // TODO
				VALUNPAK_REQUIRE_RET(!a_should_read || m_uexp->read_table_name(byte_prop->name, *this, a_offset), read_tag_result_type::failed);
				prop = std::move(byte_prop);
				break;
			}

			case property_type::array_property:
			case property_type::set_property:
			{
				std::unique_ptr<array_property> array_prop = std::make_unique<array_property>();
				VALUNPAK_REQUIRE_RET(!a_should_read || m_uexp->read_table_name(array_prop->name, *this, a_offset), read_tag_result_type::failed);
				prop = std::move(array_prop);
				break;
			}

			case property_type::map_property:
			{
				std::unique_ptr<map_property> map_prop = std::make_unique<map_property>();
				VALUNPAK_REQUIRE_RET(!a_should_read || m_uexp->read_table_name(map_prop->key_name, *this, a_offset), read_tag_result_type::failed);
				VALUNPAK_REQUIRE_RET(!a_should_read || m_uexp->read_table_name(map_prop->value_name, *this, a_offset), read_tag_result_type::failed);
				prop = std::move(map_prop);
				break;
			}

			SIMPLE_CONSTRUCT_CASE(object_property);
			SIMPLE_CONSTRUCT_CASE(name_property);
			SIMPLE_CONSTRUCT_CASE(int_property);
			SIMPLE_CONSTRUCT_CASE(float_property);
			SIMPLE_CONSTRUCT_CASE(uint16_property);

			case property_type::delegate_property:
			case property_type::double_property:
			case property_type::str_property:
			case property_type::text_property:
			case property_type::interface_property:
			case property_type::softobject_property:
			case property_type::uint64_property:
			case property_type::uint32_property:
			case property_type::int16_property:
			case property_type::int8_property:
			case property_type::enum_property:
			{
				size_t debug_offset = get_debug_offset(a_offset);
				VALUNPAK_REQUIRE_RET(false, read_tag_result_type::failed); // TODO
				break;
			}

			default:
				return read_tag_result_type::no_entry;

			}
		}

		if (prop == nullptr)
			prop = std::make_unique<base_property>();

		VALUNPAK_REQUIRE_RET(!a_should_read || (read(prop->has_property_guid, a_offset)), read_tag_result_type::failed);

		if (a_should_read && prop->has_property_guid)
			VALUNPAK_REQUIRE_RET(read_array(prop->property_guid, 4, a_offset), read_tag_result_type::failed);

		a_tag.prop = prop.get();
		VALUNPAK_REQUIRE_RET(a_should_read || start_offset == a_offset, read_tag_result_type::failed); // if a_should_read is false, a_offset should not have advanced.
		return read_tag_result_type::succeeded;
#undef SIMPLE_CONSTRUCT_CASE
	}

	bool ue4_uobject::read_struct_property(ue4_uobject::property_tag& a_tag, size_t& a_offset)
	{
		auto prop = (struct_property*)a_tag.prop;
		prop->value_type = (struct_property::type)(fnv(prop->name.c_str()));

		// Basically a simple read was necessary for these cases.
#define default_case(a_type_enum, a_elem) \
		case struct_property::type::a_type_enum:\
		{\
			auto t = std::make_unique<struct_property::a_elem>();\
			VALUNPAK_REQUIRE(read(*t, a_offset));\
			prop->value = std::move(t);\
			break;\
		}

		switch (prop->value_type)
		{
			default_case(int_point, int_point_element);
			default_case(guid, guid_element);
			default_case(vector, vector_element);
			default_case(linear_color, linear_color_element);
			default_case(color, color_element);
			default_case(box, box_element);
			default_case(quat, quat_element);
			default_case(rotator, rotator_element);
			default_case(vector_material_input, vector_material_input_element);
			default_case(color_material_input, color_material_input_element);
			default_case(expression_input, expression_input_element);

		case struct_property::type::unknown:
		case struct_property::type::level_sequence_object_reference_map:
		case struct_property::type::gameplay_tag_container:
		case struct_property::type::nav_agent_selector:
		case struct_property::type::vector4:
		case struct_property::type::vector2d:
		case struct_property::type::box2d:
		case struct_property::type::soft_object_path:
		case struct_property::type::soft_class_path:
		case struct_property::type::simple_curve_key:
		case struct_property::type::rich_curve_key:
		case struct_property::type::frame_number:
		case struct_property::type::smart_name:
		case struct_property::type::per_platform_float:
		case struct_property::type::per_platform_int:
		case struct_property::type::date_time:
		case struct_property::type::timespan:
		case struct_property::type::movie_scene_track_identifier:
		case struct_property::type::movie_scene_segment_identifier:
		case struct_property::type::movie_scene_sequence_id:
		case struct_property::type::movie_scene_segment:
		case struct_property::type::section_evaluation_data_tree:
		case struct_property::type::movie_scene_frame_range:
		case struct_property::type::movie_scene_evaluation_key:
		case struct_property::type::movie_scene_float_value:
		case struct_property::type::movie_scene_float_channel:
		case struct_property::type::movie_scene_evaluation_template:
		case struct_property::type::skeletal_mesh_sampling_lod_built_data:
			VALUNPAK_REQUIRE(false); // TODO
			break;

		default:
		{
			auto t = std::make_unique<struct_property::uobject_element>();
			VALUNPAK_REQUIRE(t->object.open(*m_uexp, *this, a_offset));
			prop->value = std::move(t);
			break;
		}
		};

#undef default_case
		return true;
	}

	bool ue4_uobject::read_property(ue4_uobject::property_tag& a_tag, property_type a_type, size_t& a_offset, property_read_mode a_read_mode)
	{
#define DEFAULT_VAL_CASE(a_type) case property_type::a_type:\
		{\
			auto prop = (a_type*)a_tag.prop;\
			VALUNPAK_REQUIRE(read(prop->value, a_offset));\
			break;\
		}

		switch (a_type)
		{
		case property_type::struct_property:
			VALUNPAK_REQUIRE(read_struct_property(a_tag, a_offset));
			break;

		case property_type::bool_property:
		{
			if (a_read_mode == property_read_mode::default_mode)
			{
				// Do nothing.
			}
			else if (a_read_mode == property_read_mode::array_mode)
			{
				auto prop = (bool_property*)a_tag.prop;
				VALUNPAK_REQUIRE(read(prop->value, a_offset));
				prop->value = !!prop->value;
			}
			else
			{
				VALUNPAK_REQUIRE(false); // TODO
			}
			break;
		}

		case property_type::enum_property:
		case property_type::byte_property:
		{
			if (a_read_mode == property_read_mode::default_mode)
			{
				auto prop = (byte_property*)a_tag.prop;
				VALUNPAK_REQUIRE(m_uexp->read_table_name(prop->value, *this, a_offset));
			}
			else
			{
				VALUNPAK_REQUIRE(false); // TODO
			}
			break;
		}

		case property_type::name_property:
		{
			auto prop = (name_property*)a_tag.prop;
			VALUNPAK_REQUIRE(m_uexp->read_table_name(prop->value, *this, a_offset));
			break;
		}

		case property_type::array_property:
		{
			auto prop = (array_property*)a_tag.prop;

			i32 length;
			VALUNPAK_REQUIRE(read(length, a_offset));

			property_type inner_type = (property_type)fnv(prop->name.c_str());
			property_tag tag(inner_type);
			bool is_struct = inner_type == property_type::struct_property;
			VALUNPAK_REQUIRE(read_tag(tag, a_offset, property_read_mode::array_mode, is_struct) == read_tag_result_type::succeeded);
			for (int i = 0; i < length; i++)
				VALUNPAK_REQUIRE(read_property(tag, inner_type, a_offset, property_read_mode::array_mode));
			break;
		}

		DEFAULT_VAL_CASE(object_property);
		DEFAULT_VAL_CASE(float_property);
		DEFAULT_VAL_CASE(int_property);
		DEFAULT_VAL_CASE(uint16_property);

		case property_type::set_property:
		case property_type::map_property:
		default:
			size_t real_offset = get_debug_offset(a_offset);
			VALUNPAK_REQUIRE(false);
			return false;

		}

		return true;
	}

	size_t ue4_uobject::read_internal(bool a_is_element)
	{
		size_t offset = 0;

		while (true)
		{
			property_tag tag;
			size_t asdreal_offset = get_debug_offset(offset);
			auto tag_result = read_tag(tag, offset, property_read_mode::default_mode);
			if (tag_result == read_tag_result_type::failed)
				return 0;
			else if (tag_result == read_tag_result_type::no_entry)
				break; // We're done

			size_t real_offset = get_debug_offset(offset);
			size_t start_offset = offset;
			VALUNPAK_REQUIRE_RET(read_property(tag, tag.type, offset, property_read_mode::default_mode), 0);


			size_t actual_size = offset - start_offset;
			if (tag.type != property_type::struct_property)
				VALUNPAK_REQUIRE(actual_size == tag.size);
			else // TODO: Properly parse structs
				offset = start_offset + tag.size;
		}

		if (!a_is_element)
		{
			i32 has_guid;
			VALUNPAK_REQUIRE_RET(read(has_guid, offset), 0);
			if (has_guid)
				offset += sizeof(i32) * 4;
		}
		
		return offset;
	}
}
