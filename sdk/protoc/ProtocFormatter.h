#ifndef __IPROTOCFORMATTER_H__
#define __IPROTOCFORMATTER_H__

#include <string>

#include "protoc/ProtocData.h"
#include "common/TXMLParser.h"

namespace Protoc {
	class ProtocFormatter {
	private:
		NamespaceDescriptor *rootNs;
		bool ParseRoot ( const TXMLNode &node);
		bool ParsePacketSnippet ( const std::string &snippet );
		bool ParsePacketCtor ( const std::string &snippet );
		bool ParsePacketField ( const TXMLNode &node );
		bool ParseDefault ( const TXMLNode &node );
		bool ParsePackeSize ( const std::string &snippet );
		bool ParsePackeSerializer ( const std::string &snippet );
		bool ParsePackeDeserializer ( const std::string &snippet );
		bool ParseParserCase ( const std::string &snippet );
	public:
		ProtocFormatter(NamespaceDescriptor *ns);
		bool ReadSnippets( const char*file);
		void Format ();
	};
}

#endif
