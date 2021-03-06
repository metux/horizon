#include "rule_descr.hpp"


namespace horizon {
	const std::map<RuleID, RuleDescription> rule_descriptions = {
		{RuleID::HOLE_SIZE, {"Hole size", true}},
		{RuleID::TRACK_WIDTH, {"Track width", true}},
		{RuleID::CLEARANCE_COPPER, {"Copper clearance", true}},
		{RuleID::CLEARANCE_SILKSCREEN_EXPOSED_COPPER, {"Clearance\nSilkscreen - Exposed copper", false}},
		{RuleID::PARAMETERS, {"Parameters", false}},
		{RuleID::SINGLE_PIN_NET, {"Single pin nets", false}},
		{RuleID::VIA, {"Vias", true}},
		{RuleID::CLEARANCE_COPPER_NON_COPPER, {"Clearance Copper - Non-copper", true}},
		{RuleID::PLANE, {"Planes", true}},
	};
}
