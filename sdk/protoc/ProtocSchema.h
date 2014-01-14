#ifndef __PROTOC_H__
#define __PROTOC_H__

#include "protoc/ProtocFormatter.h"
#include "protoc/ProtocData.h"
#include "common/TXMLParser.h"

namespace Protoc {
	class ProtocSchema {
		NamespaceDescriptor *rootNs;
		bool ParseRoot ( const TXMLNode &node);
		bool ParsePack ( const TXMLNode &node);
		bool ParseType ( const TXMLNode &node);
		bool ParseField ( const TXMLNode &node, PacketDescriptor *pack);
		bool ParseTypes( const TXMLNode &node);
	public:
		ProtocSchema(NamespaceDescriptor*ns);
		bool ReadSchema (const char *filename);
		bool CheckIntegrity () const;
	};
}

#endif
