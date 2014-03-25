/*******************************************************************/
/* Uncomment define CPPTEST to test cpp.binary.xml compile results */
/* If you'd like to get complete test just implement BinaryWriter  */
/* and BinaryReader classes                                        */
/*******************************************************************/

// #define CPPTEST

#ifdef CPPTEST

class BinaryWriter {
public:
	void Write(char) {}
	void Write(char*, int size = 0) {}
	void Write(int) {}
	void Write(short) {}
	void Write(unsigned int) {}
	void Write(unsigned short) {}
};

class BinaryReader {
public:
	char ReadByte() { return 0; }
	int ReadInt32() { return 0;  }
	unsigned int ReadUInt32() { return 0; }
	unsigned short ReadUInt16() { return 0; }
	char * ReadBytes(int size) { return 0; }
	void Seek(int) {}
};

class IPacket {
	virtual void Serialize(BinaryWriter &writer) = 0;
	virtual IPacket *Deserialize(BinaryReader &reader) = 0;
	virtual int Size() const = 0;
};

#include <string.h>
#include "../bin/Packets.h"

static void CPPTest (){
	// fake objects
	BinaryWriter writer;
	BinaryReader reader;

	// packet
	Bunker::Client::Ok ok;
	ok.id = 10;
	ok.message = _strdup("hello");
	ok.Serialize(writer);

	// deserialize test
	IPacket *pack = Bunker::Client::Deserializer::Deserialize(reader);
}

#endif
