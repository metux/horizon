#include "net.hpp"
#include "block.hpp"

namespace horizon {

	Net::Net(const UUID &uu, const json &j, Block &block): Net(uu ,j)
		{
		net_class = &block.net_classes.at(j.at("net_class").get<std::string>());
	}
	Net::Net(const UUID &uu, const json &j):
			uuid(uu),
			name(j.at("name").get<std::string>()),
			is_power(j.value("is_power", false))
		{
		net_class.uuid = j.at("net_class").get<std::string>();
	}

	Net::Net (const UUID &uu): uuid(uu){};

	UUID Net::get_uuid() const {
		return uuid;
	}

	json Net::serialize() const {
		json j;
		j["name"] = name;
		j["is_power"] = is_power;
		j["net_class"] = net_class->uuid;
		return j;
	}

	bool Net::is_named() const {
		return name.size()>0;
	}
}
