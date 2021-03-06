#include "util.hpp"
#include <fstream>
#include <unistd.h>
#include <glibmm/miscutils.h>
#ifdef G_OS_WIN32
	#include <windows.h>
#endif


namespace horizon {
	void save_json_to_file(const std::string &filename, const json &j) {
		std::ofstream ofs(filename);
		if(!ofs.is_open()) {
			throw std::runtime_error("can't save json " + filename);
			return;
		}
		ofs << std::setw(4) << j;
		ofs.close();
	}

	int orientation_to_angle(Orientation o) {
		int angle=0;
		switch(o) {
			case Orientation::RIGHT: angle=0; break;
			case Orientation::LEFT: angle=32768; break;
			case Orientation::UP: angle=16384; break;
			case Orientation::DOWN: angle=49152; break;
		}
		return angle;
	}

	#ifdef G_OS_WIN32
	std::string get_exe_dir() {
		TCHAR szPath[MAX_PATH];
		if(!GetModuleFileName(NULL, szPath, MAX_PATH)) {
			throw std::runtime_error("can't find executable");
			return "";
        }
		else {
			return Glib::path_get_dirname(szPath);
		}
	}

	#else
	std::string get_exe_dir() {

		char buf[PATH_MAX];
		ssize_t len;
        if((len = readlink("/proc/self/exe", buf, sizeof(buf)-1)) != -1) {
        	buf[len] = '\0';
        	return Glib::path_get_dirname(buf);
        }
        else {
        	throw std::runtime_error("can't find executable");
			return "";
        }
	}
	#endif

	std::string coord_to_string(const Coordf &pos, bool delta) {
		std::ostringstream ss;
		if(delta) {
			ss << "Δ";
		}
		ss << "X:";
		if(pos.x >= 0) {
			ss << "+";
		}
		else {
			ss << "−"; //this is U+2212 MINUS SIGN, has same width as +
		}
		ss << std::fixed << std::setprecision(3)  << std::setw(7) << std::setfill('0') << std::internal << std::abs(pos.x/1e6) << " mm "; //NBSP
		if(delta) {
			ss << "Δ";
		}
		ss << "Y:";
		if(pos.y >= 0) {
			ss << "+";
		}
		else {
			ss << "−";
		}
		ss << std::setw(7) << std::abs(pos.y/1e6) << " mm"; //NBSP
		return ss.str();
	}

	std::string dim_to_string(int64_t x, bool with_sign) {
		std::ostringstream ss;
		if(with_sign) {
			if(x >= 0) {
				ss << "+";
			}
			else {
				ss << "−"; //this is U+2212 MINUS SIGN, has same width as +
			}
		}
		ss << std::fixed << std::setprecision(3)  << std::setw(7) << std::setfill('0') << std::internal << std::abs(x/1e6) << " mm"; //NBSP
		return ss.str();
	}

	std::string angle_to_string(int x, bool pos_only) {
		while(x<0) {
			x+=65536;
		}
		x %= 65536;
		if(!pos_only && x>32768)
			x -= 65536;

		std::ostringstream ss;
		if(x >= 0) {
			ss << "+";
		}
		else {
			ss << "−"; //this is U+2212 MINUS SIGN, has same width as +
		}
		ss << std::fixed << std::setprecision(3)  << std::setw(7) << std::setfill('0') << std::internal << std::abs((x/65536.0)*360) << " °"; //NBSP
		return ss.str();
	}

	int64_t round_multiple(int64_t x, int64_t mul) {
		return ((x+sgn(x)*mul/2)/mul)*mul;
	}

	bool endswith(const std::string &haystack, const std::string &needle) {
		auto pos = haystack.rfind(needle);
		if(pos == std::string::npos)
			return false;
		else
			return (haystack.size()-haystack.rfind(needle)) == needle.size();
	}
}
