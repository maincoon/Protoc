#include <stdio.h>
#include <stdlib.h>

#include "common/TLog.h"
#include "common/TXMLParser.h"

#include "protoc/ProtocSchema.h"
#include "protoc/Utils.h"

using namespace Protoc;

ProtocSchema::ProtocSchema(NamespaceDescriptor*ns) {
	rootNs = ns;
}
bool ProtocSchema::CheckIntegrity () const {
	// check all types has fields implementation
	for ( std::map<std::string, FieldTypeDescriptor*>::iterator it = rootNs->nsTypes.begin(); it != rootNs->nsTypes.end(); it ++ ) {
		if (it->second->isImplemented == false) {
			TLog::logErr("No Field node implemented for type='%s'!\n", it->second->typeName.c_str());
			return false;
		}
	}
	// all seems to be ok
	return true;
}

bool ProtocSchema::ReadSchema( const char *file) {
	std::string xml = "";
	if ( Utils::ReadFIle(file, xml)) {
		TXMLParser parser;
		if ( parser.parseMessage(xml.c_str())) {
			TXMLNode root;
			parser.getRoot(root);
			return ParseRoot(root);
		} else {
			TLog::logErr("Can't parse XML: %s\n", parser.getError());
			return false;
		}
	} else {
		TLog::logErr("Error processing schema file '%s'\n", file);
		return false;
	}
}

bool ProtocSchema::ParseRoot( const TXMLNode &node ) {
	if ( node.getName() == "namespace" ) {
		std::string ns = node.getAttrib("name");
		if ( ns.length()) {
			rootNs->SetNamespace(ns);
			for( int a = 0; a < node.numChilds(); a ++ ) {
				TXMLNode child;
				node.getChild(a, child);
				std::string cName = child.getName();
				// i miss U switch(string)
				if ( cName == "pack" || cName == "abstract") {
					// packets
					if (!ParsePack(child)) {
						TLog::logErr("Packet node parsing failed '%s'!\n", child.toXML().c_str());
						return false;
					}
				} else if (cName == "types") {
					// types
					if (!ParseTypes(child)) {
						TLog::logErr("Types node parsing failed '%s'!\n", child.toXML().c_str());
						return false;
					}
				} else {
					// anything else
					TLog::logErr("Unknown 'namespace' child '%s'!\n", child.toXML().c_str());
					return false;
				}
			}
			return true;
		} else {
			TLog::logErr("Node 'namespace' must contain 'name' attribute\n");
			return false;
		}
	} else {
		TLog::logErr("Root node must be 'namespace'\n");
		return false;
	}
}

bool ProtocSchema::ParseTypes( const TXMLNode &node ) {
	if (node.getName() == "types") {
		// parsing types
		for ( int a = 0; a < node.numChilds(); a ++ ) {
			TXMLNode child;
			node.getChild(a,child);
			if ( child.getName() == "type" ) {
				if (ParseType(child)) {
					continue;
				} else {
					TLog::logErr("Unexpected node in 'types' - %s\n", child.getName().c_str());
					return false;
				}
			} else {
				TLog::logErr("Unexpected node in 'types' - %s\n", child.getName().c_str());
				return false;
			}
		}
		// whatever
		return true;
	} else {
		TLog::logErr("Expecting node 'types'\n");
		return false;
	}
}

bool ProtocSchema::ParseType ( const TXMLNode &node ) {
	if ( node.getName() == "type") {
		std::string name = node.getAttrib("name");
		if (rootNs->TypeByName(name) == 0 ) {
			FieldTypeDescriptor *desc = new FieldTypeDescriptor();
			desc->typeName = name;
			rootNs->AddType(desc);
			return true;
		} else {
			TLog::logErr("Type '%s' already named at %s\n", name.c_str(), node.toXML().c_str());
			return false;
		}
	} else {
		TLog::logErr("Expected node 'type' at %s", node.toXML().c_str());
		return false;
	}
}


bool ProtocSchema::ParsePack( const TXMLNode &node ) {
	std::string nName = node.getName();
	if (  nName == "pack" || nName == "abstract" ) {
		PacketDescriptor *pack = new PacketDescriptor(rootNs);
		// set packet is abstract
		if (  nName == "abstract" ) {
			pack->isAbstract = true;
		}
		// check parameters
		pack->packId = atoi(node.getAttrib("id").c_str());
		// do not need id for abstract packets
		if ( pack->packId != 0  || pack->isAbstract == true ) {
			if ( !rootNs->FindPacketNyId(pack->packId) || pack->isAbstract == true ) {
				pack->packName = node.getAttrib("name");
				if ( pack->packName != "" ) {
					if (!rootNs->FindPacketByName(pack->packName)) {
						std::string ext = node.getAttrib("extends");
						// resolving inheritance
						if ( ext != "" ) {
							pack->packParent = rootNs->FindPacketByName(ext);
							if ( !pack->packParent) {
								TLog::logErr("Packet 'extends' %s not defined\n", ext.c_str());
								return false;
							}
						}
						// read fields
						for ( int a = 0; a < node.numChilds(); a ++ ) {
							TXMLNode child;
							node.getChild(a,child);
							if (!ParseField(child, pack)) {
								TLog::logErr("Packet '%s' id='%d' field parsing failed\n", pack->packName.c_str(), pack->packId);
								return false;
							}
						}
						// got it
						rootNs->AddPacket(pack);
						return true;
					} else {
						TLog::logErr("Packet 'name' %s duplicated\n", pack->packName.c_str());
						return false;
					}					
				} else {
					TLog::logErr("Empty packet 'name' id=%d\n", pack->packId);
					return false;
				}
			} else {
				TLog::logErr("Packet 'id' %d duplicated\n", pack->packId);
				return false;
			}
		} else {
			TLog::logErr("Packet 'id' must not be 0 or empty\n");
			return false;
		}
	} else {
		TLog::logErr("Expecting node 'pack' or 'abstract'\n");
		return false;
	}
}

bool ProtocSchema::ParseField( const TXMLNode &node, PacketDescriptor *pack ) {
	if ( node.getName() == "field") {
		FieldDescriptor *field = new FieldDescriptor(pack);
		// field name
		field->fieldName = node.getAttrib("name");
		if ( field->fieldName.length()) {
			// check same names
			if ( !pack->FindFieldByName(field->fieldName)) {
				// check parent fields
				if ( pack->packParent != 0) {
					if ( pack->packParent->FindFieldByName(field->fieldName)) {
						TLog::logErr("Parent packet '%s' already has field named '%s'\n", pack->packParent->packName.c_str(), field->fieldName.c_str());
						delete field;
						return false;
					}
				}
				// parse field type
				std::string sType = node.getAttrib("type");
				field->fieldType = rootNs->TypeByName(sType);
				if ( field->fieldType != 0) {
					// field default value
					field->fieldDefault = node.getAttrib("default");
					// adding field in collection
					pack->AddField(field);
					return true;
				} else {
					TLog::logErr("Unknown packet field '%s' type '%s'\n", field->fieldName.c_str(), sType.c_str());
					delete field;
					return false;
				}
			} else {
				TLog::logErr("Packet field '%s' already defined\n", field->fieldName.c_str());
				delete field;
				return false;
			}
		} else {
			TLog::logErr("No field name specified for packet '%s'\n", pack->packName.c_str());
			return false;
		}
	} else {
		TLog::logErr("Unknown packet field '%s'\n", node.getName().c_str());
		return false;
	}
}


