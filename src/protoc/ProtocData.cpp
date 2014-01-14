#include <sstream>

#include "protoc/ProtocData.h"
#include "protoc/Utils.h"

#include "common/TXMLParser.h"
#include "common/TLog.h"

using namespace Protoc;

// --------------------------------------------------------------------------
// Namespace descriptor implementation
// --------------------------------------------------------------------------

NamespaceDescriptor::NamespaceDescriptor() {
	// some defaults
	nsSeparator = ".";
}

NamespaceDescriptor::~NamespaceDescriptor() {
	// go cxx11 auto here
	for ( std::map<std::string, PacketDescriptor*>::iterator it = nsPackets.begin(); it != nsPackets.end(); it ++ ) {
		delete it->second;
	}
	for ( std::map<std::string, FieldTypeDescriptor*>::iterator it = nsTypes.begin(); it != nsTypes.end(); it ++ ) {
		delete it->second;
	}
}

PacketDescriptor * NamespaceDescriptor::FindPacketByName( const std::string &name ) const {
	// go cxx11 auto here
	std::map<std::string, PacketDescriptor*>::const_iterator it = nsPackets.find(name);
	if ( it != nsPackets.end()) {
		return it->second;
	} else {
		return 0;
	}
}

PacketDescriptor * NamespaceDescriptor::FindPacketNyId( int id ) const {
	// go cxx11 auto here
	for ( std::map<std::string, PacketDescriptor*>::const_iterator it = nsPackets.begin(); it != nsPackets.end(); it ++ ) {
		if ( it->second->packId == id ) {
			return it->second;
		}
	}
	return 0;
}

void NamespaceDescriptor::SetNamespace( const std::string &ns ) {
	nsName = ns;
	nsNameParts.clear();
	Utils::Split(nsName, '.', nsNameParts);
}

std::string NamespaceDescriptor::GetNameSpace() const {
	return nsName;
}

std::string NamespaceDescriptor::CompileHeader() const {
	std::string result = Utils::TemplateReplace(header, "ns", nsName);
	return result;
}

std::string NamespaceDescriptor::CompileFooter() const {
	std::string result = Utils::TemplateReplace(footer, "ns", nsName);
	return result;
}

std::string NamespaceDescriptor::CompileRootNS() {
	// compiling content
	std::string content = "";
	if ( nsNameParts.size() > 1 ) {
		content = CompileNextNS(1);
	} else {
		content = CompileContent();
	}
	// variables
	std::string result = Utils::TemplateReplace(nsRoot, "ns", nsName);
	result = Utils::TemplateReplace(result, "name", nsNameParts[0]);
	return Utils::TemplateReplace(result, "content", content);
}

std::string NamespaceDescriptor::CompileNextNS(int num) {
	std::string content = "";
	if ( num < (int) nsNameParts.size()-1) {
		content = CompileNextNS(num+1);
	} else {
		content = CompileContent();
	}
	std::string result = Utils::TemplateReplace(nsNext, "ns", nsName);
	result = Utils::TemplateReplace(result, "parent", GetNameSpacePartial(num));
	result = Utils::TemplateReplace(result, "name", nsNameParts[num]);
	return Utils::TemplateReplace(result, "content", content);
}

std::string NamespaceDescriptor::CompileParser() {
	std::string content = Utils::TemplateReplace(parser, "ns", nsName);
	std::string fields = CompileParserCases();
	return Utils::TemplateReplace(content, "fields", fields);
}

std::string NamespaceDescriptor::CompileParserCases() {
	std::string content = "";
	// go cxx11 auto here
	for ( std::map<std::string, PacketDescriptor*>::iterator it = nsPackets.begin(); it != nsPackets.end(); it ++ ) {
		if ( it->second->isAbstract == false ) {
			content += it->second->CompileParserCase(*this);
		}
	}
	return content;
}

std::string NamespaceDescriptor::Compile() {
	std::string result = CompileHeader();
	result += CompileRootNS();
	result += CompileFooter();
	return result;
}

std::string NamespaceDescriptor::CompileContent() {
	std::string content = CompilePackets();
	content += CompileParser();
	return content;
}

std::string NamespaceDescriptor::CompilePackets() {
	std::string content = "";
	// go cxx11 auto here
	for ( std::map<std::string, PacketDescriptor*>::iterator it = nsPackets.begin(); it != nsPackets.end(); it ++ ) {
		if ( it->second->isAbstract == false ) {
			content += it->second->Compile(*this);
			content += packSeparator;
		}
	}
	return content;
}

void NamespaceDescriptor::AddPacket( PacketDescriptor *pack ) {
	nsPackets[pack->packName] = pack;
}

void NamespaceDescriptor::AddType( FieldTypeDescriptor* fType ) {
	nsTypes[fType->typeName] = fType;
}


std::string Protoc::NamespaceDescriptor::GetDefault(const std::string &name) const {
	// go cxx11 auto here
	std::map <std::string, FieldTypeDescriptor*>::const_iterator it = nsTypes.find(name);
	if ( it != nsTypes.end()) {
		return it->second->typeDefault;
	} else {
		return 0;
	}
}

FieldTypeDescriptor* NamespaceDescriptor::TypeByName( const std::string &name ) const {
	// go cxx11 auto here
	std::map <std::string, FieldTypeDescriptor*>::const_iterator it = nsTypes.find(name);
	if ( it != nsTypes.end()) {
		return it->second;
	} else {
		return 0;
	}

}

std::string NamespaceDescriptor::GetNameSpacePartial( int depth ) {
	if ( depth >= 0  && depth < (int) nsNameParts.size()) {
		std::string ret = nsNameParts[0];
		for ( int a = 1; a < depth; a ++ ) {
			ret += nsSeparator;
			ret += nsNameParts[a];
		}	
		return ret;
	} else {
		return nsName;
	}
}

// --------------------------------------------------------------------------
// Packet descriptor implementation
// --------------------------------------------------------------------------

PacketDescriptor::PacketDescriptor(NamespaceDescriptor *ns) {
	packParent = 0;
	nameSpace = ns;
	isAbstract = false;
	packId = 0;
}

PacketDescriptor::~PacketDescriptor() {
	// go cxx11 auto here
	for ( std::map<std::string, FieldDescriptor*>::iterator it = packFields.begin(); it != packFields.end(); it ++ ) {
		delete it->second;
	}
}

FieldDescriptor * PacketDescriptor::FindFieldByName( const std::string &name ) {
	std::map<std::string, FieldDescriptor*>::iterator it = packFields.find(name);
	if (it != packFields.end()) {
		return it->second;
	} else {
		return 0;
	}
}

std::string PacketDescriptor::Compile( const NamespaceDescriptor &ns ) {
	// compiling
	std::string fields = CompileFields(ns);
	std::string ctor = CompileCtor(ns);
	std::string serializer = CompileSerializer(ns);
	std::string deserializer = CompileDeserializer(ns);
	std::string size = CompileSize(ns);
	// replacing
	std::string content = Utils::TemplateReplace(pack, "ns", ns.GetNameSpace());
	content = Utils::TemplateReplace(content, "name", packName);
	content = Utils::TemplateReplace(content, "id", Utils::ToString(packId));
	content = Utils::TemplateReplace(content, "fields", fields);
	content = Utils::TemplateReplace(content, "serializer", serializer);
	content = Utils::TemplateReplace(content, "deserializer", deserializer);
	content = Utils::TemplateReplace(content, "parent", GetParentPackName());
	content = Utils::TemplateReplace(content, "size", size);
	return Utils::TemplateReplace(content, "ctor", ctor);
}

std::string PacketDescriptor::CompileParserCase( const NamespaceDescriptor &ns ) {
	std::string content = Utils::TemplateReplace(parserCase, "ns", ns.GetNameSpace());
	content = Utils::TemplateReplace(content, "name", packName);
	content = Utils::TemplateReplace(content, "id", Utils::ToString(packId));
	return Utils::TemplateReplace(content, "parent", GetParentPackName());
}

std::string PacketDescriptor::CompileCtor( const NamespaceDescriptor &ns ) {
	// not necessary for all languages
	if ( ctor.length()) {
		std::string content = Utils::TemplateReplace(ctor, "ns", ns.GetNameSpace());
		content = Utils::TemplateReplace(content, "id", Utils::ToString(packId));
		content = Utils::TemplateReplace(content, "name", packName);
		content = Utils::TemplateReplace(content, "parent", GetParentPackName());
		return Utils::TemplateReplace(content, "fields", CompileCtorFields(ns));
	} else {
		return "";
	}
}

std::string PacketDescriptor::CompileSize( const NamespaceDescriptor &ns ) {
	std::string content = Utils::TemplateReplace(size, "ns", ns.GetNameSpace());
	content = Utils::TemplateReplace(content, "id", Utils::ToString(packId));
	content = Utils::TemplateReplace(content, "name", packName);
	content = Utils::TemplateReplace(content, "parent", GetParentPackName());
	return Utils::TemplateReplace(content, "fields", CompileSizeFields(ns));
}

std::string PacketDescriptor::CompileSerializerFields( const NamespaceDescriptor &ns ) {
	std::string content = "";
	int cnt = 0;
	// go cxx11 auto here
	for ( std::map<std::string, FieldDescriptor*>::iterator it = packFields.begin(); it != packFields.end(); it ++ ) {
		content += it->second->CompileSerialize(ns, cnt++);
	}
	return content;
}

std::string PacketDescriptor::CompileDeserializerFields( const NamespaceDescriptor &ns ) {
	std::string content = "";
	int cnt = 0;
	// go cxx11 auto here
	for ( std::map<std::string, FieldDescriptor*>::iterator it = packFields.begin(); it != packFields.end(); it ++ ) {
		content += it->second->CompileDeserialize(ns, cnt++);
	}
	return content;
}

std::string PacketDescriptor::CompileSerializer( const NamespaceDescriptor &ns ) {
	// compile inherited fields
	std::string fields = "";
	if ( packParent ) {
		fields += packParent->CompileSerializerFields(ns);
	}
	// compile my fields
	fields += CompileSerializerFields(ns);
	// finalizing
	std::string content = Utils::TemplateReplace( serializer, "ns", ns.GetNameSpace());
	content = Utils::TemplateReplace( content, "id", Utils::ToString(packId));
	content = Utils::TemplateReplace( content, "name", packName);
	content = Utils::TemplateReplace( content, "parent", GetParentPackName());
	return Utils::TemplateReplace( content, "fields", fields);;
}

std::string PacketDescriptor::CompileDeserializer( const NamespaceDescriptor &ns ) {
	// compile inherited fields
	std::string fields = "";
	if ( packParent ) {
		fields += packParent->CompileDeserializerFields(ns);
	}
	// compile my fields
	fields += CompileDeserializerFields(ns);
	// finalizing
	std::string content = Utils::TemplateReplace( deserializer, "ns", ns.GetNameSpace());
	content = Utils::TemplateReplace( content, "id", Utils::ToString(packId));
	content = Utils::TemplateReplace( content, "name", packName);
	content = Utils::TemplateReplace( content, "parent", GetParentPackName());
	return Utils::TemplateReplace( content, "fields", fields);;
}

std::string PacketDescriptor::CompileFields( const NamespaceDescriptor &ns ) {
	// compile inherited fields
	std::string content = "";
	if ( packParent ) {
		content += packParent->CompileFields(ns);
	}
	int cnt = 0;
	// compile my fields
	// go cxx11 auto here
	for ( std::map<std::string, FieldDescriptor*>::iterator it = packFields.begin(); it != packFields.end(); it ++ ) {
		content += it->second->CompileFormat(ns, cnt++);
	}
	return content;
}

std::string PacketDescriptor::GetParentPackName() const {
	if ( packParent ) {
		return packParent->packName;
	} else {
		return "";
	}
}

std::string PacketDescriptor::CompileCtorFields( const NamespaceDescriptor &ns ) {
	// compile inherited fields
	std::string content = "";
	if ( packParent ) {
		content += packParent->CompileCtorFields(ns);
	}
	int cnt = 0;
	// go cxx11 auto here
	for ( std::map<std::string, FieldDescriptor*>::iterator it = packFields.begin(); it != packFields.end(); it ++ ) {
		content += it->second->CompileCtor(ns, cnt++);
	}
	return content;
}

std::string PacketDescriptor::CompileSizeFields( const NamespaceDescriptor &ns )
{
	// compile inherited fields
	std::string content = "";
	if ( packParent ) {
		content += packParent->CompileSizeFields(ns);
		if (  packFields.size()) {
			content += "+";
		}
	}
	size_t cnt = 0;
	// go cxx11 auto here
	for ( std::map<std::string, FieldDescriptor*>::iterator it = packFields.begin(); it != packFields.end(); it ++, cnt ++ ) {
		std::string sz = it->second->CompileSize(ns, cnt);
		if  (sz.length()) {
			content += sz;
			if ( cnt < packFields.size() - 1 ) {
				content += "+";
			}
		}
	}
	return content;
}

bool PacketDescriptor::ParsePacketField( const TXMLNode &node ) {
	// getting type
	std::string sType = node.getAttrib("type");
	FieldTypeDescriptor *fType = nameSpace->TypeByName(sType);
	if ( fType != 0 ) {
		// go cxx11 auto here
		for ( std::map<std::string, FieldDescriptor*>::iterator it = packFields.begin(); it != packFields.end(); it ++ ) {
			FieldDescriptor *field = it->second;
			if ( field->fieldType == fType ) {
				if (field->ParseField(node)) {
					continue;
				} else {
					TLog::logErr("Snippet Field '%s' parsing failed.\n", field->fieldName.c_str());
					return false;
				}
			}
		}
		// all ok
		return true;
	} else {
		TLog::logErr("Unknown field type '%s' in snippet Field definition'%s'.\n", sType.c_str(), node.toXML().c_str());
		return false;
	}
}

void PacketDescriptor::AddField( FieldDescriptor* field ) {
	packFields[field->fieldName] = field;
}

// --------------------------------------------------------------------------
// Field descriptor implementation
// --------------------------------------------------------------------------

std::string FieldDescriptor::CompileFragment( const NamespaceDescriptor &ns, const std::string &fragment, int num ) const {
	std::string content = Utils::TemplateReplace(fragment, "ns", ns.GetNameSpace());
	content = Utils::TemplateReplace(content, "pack", pack->packName);
	content = Utils::TemplateReplace(content, "id", Utils::ToString(pack->packId));
	content = Utils::TemplateReplace(content, "name", fieldName);
	content = Utils::TemplateReplace(content, "num", Utils::ToString(num));
	content = Utils::TemplateReplace(content, "parent", pack->GetParentPackName());
	content = Utils::TemplateReplace(content, "default", GetDefault(ns));
	return Utils::TemplateReplace(content, "size", CompileSize(ns, num));

}

std::string FieldDescriptor::CompileFormat( const NamespaceDescriptor &ns, int num ) const {
	return CompileFragment(ns, format, num);
}

std::string FieldDescriptor::CompileCtor( const NamespaceDescriptor &ns, int num ) const {
	return CompileFragment(ns, ctor, num);
}

std::string FieldDescriptor::CompileSerialize( const NamespaceDescriptor &ns, int num ) const {
	return CompileFragment(ns, serialize, num);
}

std::string FieldDescriptor::CompileDeserialize( const NamespaceDescriptor &ns, int num ) const {
	return CompileFragment(ns, deserialize, num);
}

std::string FieldDescriptor::CompileSize( const NamespaceDescriptor &ns, int num ) const {
	// stop recursion here so do not use CompileFragment call
	std::string content = Utils::TemplateReplace(size, "ns", ns.GetNameSpace());
	content = Utils::TemplateReplace(content, "pack", pack->packName);
	content = Utils::TemplateReplace(content, "id", Utils::ToString(pack->packId));
	content = Utils::TemplateReplace(content, "name", fieldName);
	content = Utils::TemplateReplace(content, "num", Utils::ToString(num));
	content = Utils::TemplateReplace(content, "parent", pack->GetParentPackName());
	return Utils::TemplateReplace(content, "default", GetDefault(ns));
}

bool FieldDescriptor::ParseField( const TXMLNode &node ) {
	for ( int a = 0; a < node.numChilds(); a ++ ) {
		TXMLNode child;
		node.getChild(a, child);
		std::string nName = child.getName();
		// field definition
		if ( nName == "Format" ) {
			format = child.getData();
			continue;
		}
		// field serialize definition
		if ( nName == "Serialize" ) {
			serialize = child.getData();
			continue;
		}
		// field serialize definition
		if ( nName == "Deserialize" ) {
			deserialize = child.getData();
			continue;
		}
		// field size definition
		if ( nName == "Size" ) {
			size = child.getData();
			continue;
		}
		// field size definition
		if ( nName == "Ctor" ) {
			ctor = child.getData();
			continue;
		}
		// unknown node
		TLog::logErr("Unknown node '%s' in Field definition.\n", nName.c_str());
		return false;
	}
	// all ok
	return true;
}

FieldDescriptor::FieldDescriptor( PacketDescriptor *pd ) {
	pack = pd;
	fieldType = 0;
}

std::string FieldDescriptor::GetDefault( const NamespaceDescriptor &ns) const {
	if ( fieldDefault.length()) {
		return fieldDefault;
	} else {
		return ns.GetDefault(fieldType->typeName);
	}
}
