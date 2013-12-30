#include <stdio.h>
#include <sstream>

#include "protoc/Utils.h"
#include "common/TLog.h"

using namespace Protoc;

bool Utils::ReadFIle( const char *name, std::string &result ) {
	FILE *out = fopen(name, "rt");
	if ( out ) {
		result = "";
		char buf[2048];
		while (fgets(buf, sizeof(buf), out)) {
			result.append(buf);
		}
		fclose(out);
		return true;
	} else {
		TLog::logErr("Can't open file %s\n", name);
		return false;
	}
}

FieldDescriptor::FieldType Protoc::Utils::TypeByName( const std::string &sType ) {
	if ( sType == "string" ) {
		return FieldDescriptor::String;
	} else if ( sType == "int" ) {
		return FieldDescriptor::Int;
	} else if ( sType == "numeric" ) {
		return FieldDescriptor::Numeric;
	} else if ( sType == "binary" ) {
		return FieldDescriptor::Binary;
	} else {
		return FieldDescriptor::None;
	}
}

std::string Protoc::Utils::TemplateReplace( const std::string &content, const std::string &name, const std::string &value ) {
	std::string result = "";
	std::string sig = "$" + name + "$";
	size_t spos = 0;
	size_t pos = content.find(sig, spos);
	if ( pos == std::string::npos) {
		// not found 
		result = content;
	} else {
		// found
		while (pos != std::string::npos) {
			result += content.substr(spos, pos-spos);
			result += value;
			spos = pos + sig.length();
			pos = content.find(sig, spos);
		}
		// copy last
		result += content.substr(spos, content.length()-spos);
	}
	return result;
}

std::vector<std::string> & Protoc::Utils::Split( const std::string &s, char delim, std::vector<std::string> &elems ) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::string Utils::ToString( int val ) {
	std::stringstream ss;
	ss << val;
	return ss.str();
}
