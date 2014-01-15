#include "common/TLog.h"
#include "common/TXMLParser.h"

#include "protoc/ProtocFormatter.h"
#include "protoc/Utils.h"

using namespace Protoc;

ProtocFormatter::ProtocFormatter( NamespaceDescriptor *ns ) {
	rootNs = ns;
}

bool ProtocFormatter::ReadSnippets( const char*file ) {
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
		TLog::logErr("Error processing snippets format file '%s'\n", file);
		return false;
	}
}

void ProtocFormatter::Format() {
	std::string content = rootNs->Compile();
	FILE *out = stdout;
	FILE *fout = 0;
	if ( outFile.length() > 0 ) {
		out = fopen (outFile.c_str(), "wt");
		fout = out;
		if ( !out ) {
			TLog::logErr("Can't open output file '%s'\n", outFile.c_str());
			return;
		} else {
			TLog::logMsg("Output file: '%s'...\n", outFile.c_str());
		}
	}
	fprintf ( out, "%s", content.c_str());
	if (fout) {
		TLog::logMsg("done.\n");
		fclose(fout);
	}
}

bool ProtocFormatter::ParseRoot( const TXMLNode &node ) {
	if ( node.getName() == "snippets" ) {
		// reading snippets data
		for( int a = 0; a < node.numChilds(); a ++ ) {
			TXMLNode child;
			node.getChild(a, child);
			std::string name = child.getName();
			// header
			if ( name == "Header") {
				rootNs->header = child.getData();
				continue;
			}
			// footer
			if (name == "Footer") {
				rootNs->footer = child.getData();
				continue;
			}
			// packet separator
			if ( name == "PackSeparator") {
				rootNs->packSeparator = child.getData();
				continue;
			}
			// packet separator
			if ( name == "NSSeparator") {
				rootNs->nsSeparator = child.getData();
				continue;
			}
			// root namespace
			if ( name== "NSRoot") {
				rootNs->nsRoot = child.getData();
				continue;
			}
			// nested namespace
			if ( name == "NSNext") {
				rootNs->nsNext = child.getData();
				continue;
			}
			// pack snippet
			if ( name == "Pack") {
				if (ParsePacketSnippet (child.getData())) {
					continue;
				} else {
					return false;
				}
			}
			// pack ctor
			if ( name == "Ctor") {
				if (ParsePacketCtor(child.getData())) {
					continue;
				} else {
					return false;
				}
			}
			// pack field
			if ( name == "Field") {
				if (ParsePacketField(child)) {
					continue;
				} else {
					return false;
				}
			}
			// pack size
			if ( name == "Size" ) {
				if (ParsePackeSize(child.getData())) {
					continue;
				} else {
					return false;
				}
			}
			// pack serializer
			if ( name == "Serializer" ) {
				if (ParsePackeSerializer(child.getData())) {
					continue;
				} else {
					return false;
				}
			}
			// pack deserializer
			if ( name == "Deserializer" ) {
				if (ParsePackeDeserializer(child.getData())) {
					continue;
				} else {
					return false;
				}
			}
			// namespace parser
			if ( name == "Parser") {
				rootNs->parser = child.getData();
				continue;
			}
			// namespace parser case
			if ( name == "ParserCase") {
				if (ParseParserCase(child.getData())) {
					continue;
				} else {
					return false;
				}
			}
			// default values
			if ( name == "Default") {
				if (ParseDefault(child)) {
					continue;
				} else {
					return false;
				}
			}
			// output file
			if ( name == "Output" ) {
				outFile = child.getAttrib("file");
				continue;
			}
			// unknown node
			TLog::logErr("Unknown node in 'snippets' - '%s'\n", name.c_str());
			return false;
		}
		return true;
	} else {
		TLog::logErr("Root node must be 'snippets'\n");
		return false;
	}
}

bool ProtocFormatter::ParsePacketSnippet( const std::string &snippet ) {
	if ( rootNs->nsPackets.size()) {
		for ( size_t a = 0; a < rootNs->nsPackets.size(); a ++ ) {
			rootNs->nsPackets[a]->pack = snippet;
		}
		return true;
	} else {
		TLog::logErr("Schema must contain at leas one packet definition!\n");
		return false;
	}
}

bool ProtocFormatter::ParsePacketCtor( const std::string &snippet ) {
	if ( rootNs->nsPackets.size()) {
		for ( size_t a = 0; a < rootNs->nsPackets.size(); a ++ ) {
			rootNs->nsPackets[a]->ctor = snippet;
		}
		return true;
	} else {
		TLog::logErr("Schema must contain at leas one packet definition!\n");
		return false;
	}
}

bool ProtocFormatter::ParsePacketField( const TXMLNode &node ) {
	if ( rootNs->nsPackets.size()) {
		// set field for all 
		for ( size_t a = 0; a < rootNs->nsPackets.size(); a ++ ) {
			PacketDescriptor *pack = rootNs->nsPackets[a];
			if (pack->ParsePacketField(node)) {
				continue;
			} else {
				TLog::logErr("Packet '%s' field parsing failed.\n", pack->packName.c_str());
				return false;
			}
		}
		return true;
	} else {
		TLog::logErr("Schema must contain at leas one packet definition!\n");
		return false;
	}
}

bool ProtocFormatter::ParsePackeSize( const std::string &snippet ) {
	if ( rootNs->nsPackets.size()) {
		for ( size_t a = 0; a < rootNs->nsPackets.size(); a ++ ) {
			rootNs->nsPackets[a]->size = snippet;
		}
		return true;
	} else {
		TLog::logErr("Schema must contain at leas one packet definition!\n");
		return false;
	}
}

bool ProtocFormatter::ParsePackeSerializer( const std::string &snippet ) {
	if ( rootNs->nsPackets.size()) {
		for ( size_t a = 0; a < rootNs->nsPackets.size(); a ++ ) {
			rootNs->nsPackets[a]->serializer = snippet;
		}
		return true;
	} else {
		TLog::logErr("Schema must contain at leas one packet definition!\n");
		return false;
	}
}

bool ProtocFormatter::ParsePackeDeserializer( const std::string &snippet ) {
	if ( rootNs->nsPackets.size()) {
		for ( size_t a = 0; a < rootNs->nsPackets.size(); a ++ ) {
			rootNs->nsPackets[a]->deserializer = snippet;
		}
		return true;
	} else {
		TLog::logErr("Schema must contain at leas one packet definition!\n");
		return false;
	}
}

bool ProtocFormatter::ParseParserCase( const std::string &snippet ) {
	if ( rootNs->nsPackets.size()) {
		for ( size_t a = 0; a < rootNs->nsPackets.size(); a ++ ) {
			rootNs->nsPackets[a]->parserCase = snippet;
		}
		return true;
	} else {
		TLog::logErr("Schema must contain at leas one packet definition!\n");
		return false;
	}
}

bool ProtocFormatter::ParseDefault( const TXMLNode &node ) {
	std::string sType = node.getAttrib("type");
	std::string val = node.getData();
	FieldTypeDescriptor *fType = rootNs->TypeByName(sType);
	if ( fType != 0) {
		fType->typeDefault = val;
		return true;
	} else {
		TLog::logErr("Unknown type '%s' in Default node '%s'\n", sType.c_str(), node.toXML().c_str());
		return false;
	}
}
