#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include "protoc/ProtocData.h"

namespace Protoc {
	class Utils {
	public:
		static bool ReadFIle ( const char *name, std::string &result);
		static std::string TemplateReplace ( const std::string &content, const std::string &name, const std::string &value );
		static std::vector<std::string> &Split(const std::string &s, char delim, std::vector<std::string> &elems);
		static std::string ToString( int val );
	};
}

#endif
