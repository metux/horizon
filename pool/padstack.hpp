#pragma once
#include "uuid.hpp"
#include "json.hpp"
#include "common.hpp"
#include "uuid_provider.hpp"
#include "polygon.hpp"
#include "hole.hpp"
#include "lut.hpp"
#include "shape.hpp"
#include "layer_provider.hpp"
#include "parameter/program.hpp"
#include <vector>
#include <map>
#include <fstream>
#include <set>

namespace horizon {
	using json = nlohmann::json;


	class Padstack : public UUIDProvider, public LayerProvider {
		public :
			class MyParameterProgram: public ParameterProgram {
				friend Padstack;
				private:
					ParameterProgram::CommandHandler get_command(const std::string &cmd) override;
					class Padstack *ps = nullptr;

					std::pair<bool, std::string> set_shape(const ParameterProgram::TokenCommand *cmd, std::deque<int64_t> &stack);
					std::pair<bool, std::string> set_hole(const ParameterProgram::TokenCommand *cmd, std::deque<int64_t> &stack);

				public:
					MyParameterProgram(class Padstack *p, const std::string &code);


			};


			enum class Type {TOP, BOTTOM, THROUGH, VIA, MECHANICAL};
			static const LutEnumStr<Padstack::Type> type_lut;

			Padstack(const UUID &uu, const json &j);
			Padstack(const UUID &uu);
			static Padstack new_from_file(const std::string &filename);

			json serialize() const ;

			Padstack(const Padstack &ps);
			void operator=(Padstack const &ps);

			UUID uuid;
			std::string name;
			Type type = Type::TOP;
			std::map<UUID, Polygon> polygons;
			std::map<UUID, Hole> holes;
			std::map<UUID, Shape> shapes;

			ParameterSet parameter_set;
			MyParameterProgram parameter_program;
			std::pair<bool, std::string> apply_parameter_set(const ParameterSet &ps);


			UUID get_uuid() const override;
			std::pair<Coordi, Coordi> get_bbox(bool copper_only=false) const;
			void expand_inner(unsigned int n_inner);
			const std::map<int, Layer> &get_layers() const override;

		private :
			void update_refs();
	};
}
