#include "core_package.hpp"
#include <algorithm>
#include <fstream>

namespace horizon {
	CorePackage::CorePackage(const std::string &filename, Pool &pool):
		package(Package::new_from_file(filename, pool)),
		m_filename(filename),
		parameter_program_code(package.parameter_program.get_code()),
		parameter_set(package.parameter_set)
	{
		rebuild();
		m_pool = &pool;
	}

	bool CorePackage::has_object_type(ObjectType ty) {
		switch(ty) {
			case ObjectType::JUNCTION:
			case ObjectType::POLYGON:
			case ObjectType::POLYGON_EDGE:
			case ObjectType::POLYGON_VERTEX:
			case ObjectType::POLYGON_ARC_CENTER:
			case ObjectType::PAD:
			case ObjectType::TEXT:
			case ObjectType::LINE:
			case ObjectType::ARC:

				return true;
			break;
			default:
				;
		}

		return false;
	}

	LayerProvider *CorePackage::get_layer_provider() {
		return &package;
	}

	Package *CorePackage::get_package(bool work) {
		return &package;
	}

	std::map<UUID, Junction> *CorePackage::get_junction_map(bool work) {
		return &package.junctions;
	}
	std::map<UUID, Line> *CorePackage::get_line_map(bool work) {
		return &package.lines;
	}
	std::map<UUID, Arc> *CorePackage::get_arc_map(bool work) {
		return &package.arcs;
	}
	std::map<UUID, Text> *CorePackage::get_text_map(bool work) {
		return &package.texts;
	}
	std::map<UUID, Polygon> *CorePackage::get_polygon_map(bool work) {
		return &package.polygons;
	}
	std::map<UUID, Hole> *CorePackage::get_hole_map(bool work) {
		return nullptr;
	}

	std::pair<Coordi,Coordi> CorePackage::get_bbox() {
		auto bb = package.get_bbox();
		int64_t pad = 1_mm;
		bb.first.x -= pad;
		bb.first.y -= pad;

		bb.second.x += pad;
		bb.second.y += pad;
		return bb;
	}

	std::string CorePackage::get_property_string(const UUID &uu, ObjectType type, ObjectProperty::ID property, bool *handled) {
		bool h = false;
		std::string r= Core::get_property_string(uu, type, property, &h);
		if(h)
			return r;
		switch(type) {
			case ObjectType::PAD :
				switch(property) {
					case ObjectProperty::ID::NAME :
						return package.pads.at(uu).name;
					case ObjectProperty::ID::VALUE :
						return package.pads.at(uu).pool_padstack->name;
					case ObjectProperty::ID::PAD_TYPE : {
						const auto ps = package.pads.at(uu).pool_padstack;
						switch(ps->type) {
							case Padstack::Type::MECHANICAL:	return "Mechanical";
							case Padstack::Type::BOTTOM:		return "Bottom";
							case Padstack::Type::TOP:			return "Top";
							case Padstack::Type::THROUGH:		return "Through";
							default:							return "Invalid";
						}
					} break;
					default :
						return "<invalid prop>";
				}
			break;
			default :
				return "<invalid type>";
		}
		return "<meh>";
	}
	void CorePackage::set_property_string(const UUID &uu, ObjectType type, ObjectProperty::ID property, const std::string &value, bool *handled) {
		if(tool_is_active())
			return;
		bool h = false;
		Core::set_property_string(uu, type, property, value, &h);
		if(h)
			return;
		switch(type) {
			case ObjectType::PAD :
				switch(property) {
					case ObjectProperty::ID::NAME :
						package.pads.at(uu).name = value;
					break;
					default :
						;
				}
			break;
			default :
				;
		}
		rebuild();
		set_needs_save(true);
	}

	void CorePackage::rebuild(bool from_undo) {
		package.apply_parameter_set({});
		Core::rebuild(from_undo);
	}

	void CorePackage::history_push() {
		history.push_back(std::make_unique<CorePackage::HistoryItem>(package));
	}

	void CorePackage::history_load(unsigned int i) {
		auto x = dynamic_cast<CorePackage::HistoryItem*>(history.at(history_current).get());
		package = x->package;
	}

	const Package *CorePackage::get_canvas_data() {
		return &package;
	}

	void CorePackage::commit() {
		set_needs_save(true);
	}

	void CorePackage::revert() {
		history_load(history_current);
		reverted = true;
	}

	json CorePackage::get_meta() {
		json j;
		std::ifstream ifs(m_filename);
		if(!ifs.is_open()) {
			throw std::runtime_error("file "  +m_filename+ " not opened");
		}
		ifs>>j;
		ifs.close();
		if(j.count("_imp")) {
			return j["_imp"];
		}
		return nullptr;
	}

	void CorePackage::save() {
		package.parameter_set = parameter_set;
		package.parameter_program.set_code(parameter_program_code);

		s_signal_save.emit();

		std::ofstream ofs(m_filename);
		if(!ofs.is_open()) {
			std::cout << "can't save symbol" <<std::endl;
			return;
		}
		json j = package.serialize();
		auto save_meta = s_signal_request_save_meta.emit();
		j["_imp"] = save_meta;

		ofs << std::setw(4) << j;
		ofs.close();

		set_needs_save(false);
	}
}
