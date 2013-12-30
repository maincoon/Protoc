#ifndef __PROTOCDATA_H__
#define __PROTOCDATA_H__

#include <string>
#include <map>
#include <vector>

#include "common/TXMLParser.h"

namespace Protoc {
	// forward declaration
	class NamespaceDescriptor;
	class PacketDescriptor;

	// Packet field descriptor
	class FieldDescriptor {
	private:
		// field parent
		PacketDescriptor *pack;
	public:
		// field types
		enum FieldType {
			None,
			Int,
			Numeric,
			String,
			Binary
		};

		// field data
		FieldType fieldType;
		std::string fieldName;
		std::string fieldDefault;

		// snippets data
		std::string format;
		std::string serialize;
		std::string deserialize;
		std::string size;
		std::string ctor;

		FieldDescriptor(PacketDescriptor *pd);

		// utilities
		bool ParseField( const TXMLNode &node );
		std::string CompileFragment ( const NamespaceDescriptor &ns, const std::string &fragment, int num) const;
		std::string GetDefault(const NamespaceDescriptor &ns) const;

		// compile snippets
		std::string CompileFormat( const NamespaceDescriptor &ns, int num ) const;
		std::string CompileSerialize( const NamespaceDescriptor &ns, int num) const;
		std::string CompileDeserialize( const NamespaceDescriptor &ns, int num) const;
		std::string CompileSize( const NamespaceDescriptor &ns, int num) const;
		std::string CompileCtor( const NamespaceDescriptor &ns, int num) const;
	};

	// Packet descriptor
	class PacketDescriptor {
	private:
		// packet fields
		std::map <std::string, FieldDescriptor*> packFields;
	public:
		// snippets data
		std::string pack;
		std::string ctor;
		std::string size;
		std::string serializer;
		std::string deserializer;
		std::string parserCase;

		// packet data
		bool isAbstract;
		int packId;
		std::string packName;
		PacketDescriptor *packParent;

		PacketDescriptor();
		~PacketDescriptor();

		// utilities
		FieldDescriptor *FindFieldByName( const std::string &name );
		bool ParsePacketField( const TXMLNode &node );
		std::string GetParentPackName() const;
		void AddField ( FieldDescriptor* field );
		std::string GetPackId() const;

		// compile snippets
		std::string Compile(const NamespaceDescriptor &ns);
		std::string CompileFields(const NamespaceDescriptor &ns);
		std::string CompileCtorFields(const NamespaceDescriptor &ns);
		std::string CompileCtor(const NamespaceDescriptor &ns);
		std::string CompileSize(const NamespaceDescriptor &ns);
		std::string CompileSerializer(const NamespaceDescriptor &ns);
		std::string CompileSerializerFields(const NamespaceDescriptor &ns);
		std::string CompileDeserializerFields(const NamespaceDescriptor &ns);
		std::string CompileDeserializer(const NamespaceDescriptor &ns);
		std::string CompileSizeFields(const NamespaceDescriptor &ns);
		std::string CompileParserCase(const NamespaceDescriptor &ns);
	};

	// namespace descriptor
	class NamespaceDescriptor {
	public:
		// default data;
		std::string defaultInt;
		std::string defaultNum;
		std::string defaultStr;
		std::string defaultBin;

		// namespace data
		std::vector <std::string> nsNameParts;
		std::map <std::string, PacketDescriptor*> nsPackets;
		std::string nsName;

		// snippets data
		std::string header;
		std::string footer;
		std::string nsRoot;
		std::string nsNext;
		std::string packSeparator;
		std::string parser;

		~NamespaceDescriptor();

		// utilities
		PacketDescriptor *FindPacketByName ( const std::string &name);
		PacketDescriptor *FindPacketNyId (int id);
		void SetNamespace( const std::string &ns );
		std::string GetNameSpace() const;
		void AddPacket(PacketDescriptor *pack);
		std::string GetDefault(FieldDescriptor::FieldType type) const;

		// compile snippets
		std::string CompileHeader() const;
		std::string CompileFooter() const;
		std::string CompileRootNS();
		std::string CompileNextNS( int num );
		std::string CompileParser();
		std::string CompileContent();
		std::string CompilePackets();
		std::string CompileParserCases ();

		// compile while shit
		std::string Compile();
	};
}

#endif
