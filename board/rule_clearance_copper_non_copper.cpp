#include "rule_clearance_copper_non_copper.hpp"
#include "util.hpp"
#include "lut.hpp"
#include <sstream>

namespace horizon {


	RuleClearanceCopperNonCopper::RuleClearanceCopperNonCopper(const UUID &uu): Rule(uu) {
		id = RuleID::CLEARANCE_COPPER_NON_COPPER;
	}

	RuleClearanceCopperNonCopper::RuleClearanceCopperNonCopper(const UUID &uu, const json &j): Rule(uu, j),
			match(j.at("match")), layer(j.at("layer")), routing_offset(j.value("routing_offset", 0.05_mm)){
		id = RuleID::CLEARANCE_COPPER_NON_COPPER;
		{
			const json &o = j["clearances"];
			for (auto it = o.cbegin(); it != o.cend(); ++it) {
				const json &va = it.value();
				PatchType a = patch_type_lut.lookup(va.at("types").at(0));
				PatchType b = patch_type_lut.lookup(va.at("types").at(1));
				set_clearance(a, b, va.at("clearance"));
			}
		}
	}

	uint64_t RuleClearanceCopperNonCopper::get_clearance(PatchType pt_cu, PatchType pt_ncu) const {
		std::pair<PatchType, PatchType> key(pt_cu, pt_ncu);
		if(clearances.count(key)) {
			return clearances.at(key);
		}
		return .1_mm;
	}

	uint64_t RuleClearanceCopperNonCopper::get_max_clearance() const {
		uint64_t max_clearance = 0;
		for(auto &it: clearances) {
			max_clearance = std::max(max_clearance, it.second);
		}
		return max_clearance;
	}

	void RuleClearanceCopperNonCopper::set_clearance(PatchType pt_cu, PatchType pt_ncu, uint64_t c) {
		std::pair<PatchType, PatchType> key(pt_cu, pt_ncu);
		clearances[key] = c;
	}

	json RuleClearanceCopperNonCopper::serialize() const {
		json j = Rule::serialize();
		j["match"] = match.serialize();
		j["layer"] = layer;
		j["routing_offset"] = routing_offset;
		j["clearances"] = json::array();
		for(const auto &it: clearances) {
			json k;
			k["types"] = {patch_type_lut.lookup_reverse(it.first.first), patch_type_lut.lookup_reverse(it.first.second)};
			k["clearance"] = it.second;
			j["clearances"].push_back(k);
		}
		return j;
	}


	std::string RuleClearanceCopperNonCopper::get_brief(const class Block *block) const {
		std::stringstream ss;
		ss<<"Match " << match.get_brief(block) << "\n";
		ss<<"Layer " << layer;
		return ss.str();
	}

}
