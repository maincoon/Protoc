#ifndef __DEV_XMLPARSER_H__
#define __DEV_XMLPARSER_H__

#include <string>

class TXMLNodeCounter  {
public:
	int childNum;
	int attribNum;
	TXMLNodeCounter () {
		childNum = -1;
		attribNum = -1;
	}
};

class TiXmlElement;
class TiXmlDocument;

// XML node will not delete XML entries
class TXMLNode {
	bool isCopy;
	TiXmlElement *element;
	TXMLNodeCounter *counter;
public:
	TXMLNode() {
		isCopy = false;
		element = 0;
		counter = new TXMLNodeCounter();
	}

	TXMLNode ( TiXmlElement *t ) {
		element = t;
		counter = new TXMLNodeCounter();
		isCopy = false;
	}

	TXMLNode ( const TXMLNode &node ) {
		element = node.element;
		counter = new TXMLNodeCounter();
		counter->childNum = node.counter->childNum;
		counter->attribNum = node.counter->attribNum;
		isCopy = false;
	}

	TXMLNode& operator = ( const TXMLNode &node );
	~TXMLNode ();
	
	TiXmlElement * getTag () const {
		return element;
	}

	int numChilds () const;
	int attribCount () const;
	std::string getData () const;
	std::string getName () const;
	bool getAttrib ( int, std::string &name, std::string &value ) const;
	std::string getAttrib ( const char *name ) const;
	bool getChild ( int idx,  TXMLNode &node ) const;
	bool getChild ( const char *name, TXMLNode &node ) const;
	std::string toXML () const;
	void Copy ();
};

// interface to XML parser functions and XML nodes
class TXMLParser {
	TiXmlDocument *doc;
	TXMLNodeCounter *counter;
public:
	TXMLParser ();
	~TXMLParser ();

	int numChilds () const;
	int attribCount () const;
	std::string getData () const;
	std::string getName () const;
	std::string getAttrib ( const char *name ) const;
	bool getAttrib ( int, std::string &name, std::string &value ) const;
	bool getChild ( int idx,  TXMLNode &node ) const;
	bool getChild ( const char *name, TXMLNode &node ) const;
	bool getRoot ( TXMLNode &node );
	bool parseMessage ( const char *msg );
	const char *getError () const;
};

#endif
