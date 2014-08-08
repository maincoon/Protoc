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

	// packet field type descriptor
	class FieldTypeDescriptor {
	public:
		// type data
		std::string typeName;
		bool isImplemented;
		// snippet data
		std::string typeDefault;

		// Default constructor
		FieldTypeDescriptor() {
			isImplemented = false;
		}
	};

	// Packet field descriptor
	class FieldDescriptor {
	private:
		// field parent
		PacketDescriptor *pack;
	public:
		// field data
		FieldTypeDescriptor *fieldType;
		std::string fieldName;
		std::string fieldDefault;

		// snippets data
		std::string format;
		std::string serialize;
		std::string deserialize;
		std::string size;
		std::string ctor;
		std::string dtor;

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
		std::string CompileDtor(const NamespaceDescriptor &ns, int num) const;
	};

	// Packet descriptor
	class PacketDescriptor {
	public:
		// packet fields
		std::vector <FieldDescriptor*> packFields;

		// snippets data
		std::string pack;
		std::string ctor;
		std::string dtor;
		std::string size;
		std::string serializer;
		std::string deserializer;
		std::string parserCase;

		// packet data
		bool isAbstract;
		int packId;
		std::string packName;
		PacketDescriptor *packParent;
		NamespaceDescriptor *nameSpace;

		PacketDescriptor(NamespaceDescriptor *ns);
		~PacketDescriptor();

		// utilities
		FieldDescriptor *FindFieldByName( const std::string &name );
		bool ParsePacketField( const TXMLNode &node );
		std::string GetParentPackName() const;
		void AddField ( FieldDescriptor* field );
		std::string GetPackId() const;
		void CopyInheritedFields(std::vector <FieldDescriptor*> &copy);

		// compile snippets
		std::string Compile(const NamespaceDescriptor &ns);
		std::string CompileFields(const NamespaceDescriptor &ns);
		std::string CompileCtorFields(const NamespaceDescriptor &ns);
		std::string CompileDtorFields(const NamespaceDescriptor &ns);
		std::string CompileCtor(const NamespaceDescriptor &ns);
		std::string CompileDtor(const NamespaceDescriptor &ns);
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
		std::vector <PacketDescriptor*> nsPackets;
		std::map <std::string, FieldTypeDescriptor*> nsTypes;
		std::string nsName;

		// snippets data
		std::string header;
		std::string footer;
		std::string nsRoot;
		std::string nsNext;
		std::string packSeparator;
		std::string defaultParentName;
		std::string nsSeparator;
		std::string parser;

		NamespaceDescriptor();
		~NamespaceDescriptor();

		// utilities
		PacketDescriptor *FindPacketByName ( const std::string &name) const;
		PacketDescriptor *FindPacketNyId (int id) const;
		FieldTypeDescriptor* TypeByName ( const std::string &sType ) const;

		void SetNamespace( const std::string &ns );
		std::string GetNameSpace() const;
		void AddPacket(PacketDescriptor *pack);
		void AddType ( FieldTypeDescriptor* fType );
		std::string GetDefault(const std::string &name) const;
		std::string GetNameSpacePartial( int depth );

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
