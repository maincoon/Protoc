#include "common/TXMLParser.h"
#include "tinyxml/tinyxml.h"

TXMLParser::TXMLParser () {
	doc = new TiXmlDocument ();
	counter = new TXMLNodeCounter();
}

TXMLParser::~TXMLParser () {
	delete doc;
	delete counter;
}

int TXMLParser::numChilds () const {
	if ( counter->childNum < 0 ) {
		TiXmlHandle handle(doc->RootElement());
		TiXmlElement *element = handle.FirstChildElement().Element();
		if ( element ) {
			int cnt = 0;
			while ( element ) {
				element=element->NextSiblingElement();
				cnt ++;
			}
			counter->childNum = cnt;
		} else {
			counter->childNum = 0;
		}
		return counter->childNum;
	} else {
		return counter->childNum;
	}
}

int TXMLParser::attribCount () const {
	if ( counter->attribNum < 0 ) {
		TiXmlAttribute *attr = doc->RootElement()->FirstAttribute();
		if ( attr ) {
			int cnt = 0;
			while ( attr ) {
				 attr=attr->Next();
				cnt ++;
			}
			counter->attribNum = cnt;
		} else {
			counter->attribNum = 0;
		}
		return counter->attribNum;
	} else {
		return counter->attribNum;
	}
}

std::string TXMLParser::getData () const {
	TiXmlElement *element = doc->RootElement();
	if ( element ) {
		const char *text = element->GetText();
		if ( text ) {
			return text;
		} else {
			return "";
		}
	} else {
		return "";
	}
}

std::string TXMLParser::getName () const{
	const char *name = doc->RootElement()->Value();
	if ( name ) {
		return name;
	} else {
		return "";
	}
}

std::string TXMLParser::getAttrib ( const char *name ) const {
	const char *attr = doc->RootElement()->Attribute(name);
	if ( attr ) {
		return attr;
	} else {
		return "";
	}
}

bool TXMLParser::getAttrib ( int idx, std::string &name, std::string &value ) const {
	if ( idx < 0 || idx >= attribCount()) {
		return false;
	} else {
		int cnt = 0;
		TiXmlAttribute *attr = doc->RootElement()->FirstAttribute();
		while ( cnt != idx ) {
			attr = attr->Next();
			cnt ++;
		}
		name = attr->Name();
		value = attr->Value();
		return true;
	}
}

bool TXMLParser::getChild ( int idx, TXMLNode &node ) const {
	TiXmlHandle handle(doc->RootElement());
	TiXmlElement *element = handle.FirstChild().Element();
	if ( element ) {
		int cnt = 0;
		while ( element ) {
			if ( cnt == idx ) {
				node = TXMLNode(element);
				return true;
			} else {
				cnt ++;
			}
			element=element->NextSiblingElement();
		}
		return false;
	} else {
		return false;
	}
}

bool TXMLParser::getChild ( const char *name, TXMLNode &node ) const {
	TiXmlHandle handle(doc->RootElement());
	TiXmlElement *element = handle.FirstChild(name).Element();
	if ( element ) {
		node = TXMLNode(element);
		return true;
	} else {
		return false;
	}
}

bool TXMLParser::getRoot(TXMLNode &node ) {
	if ( !doc->Error()) {
		TiXmlHandle handle(doc->RootElement());
		TiXmlElement *element = handle.Element();
		node = TXMLNode(element);
		return true;
	} else {
		return false;
	}
}

bool TXMLParser::parseMessage ( const char *msg ) {
	doc->Parse(msg);
	counter->childNum = -1;
	counter->attribNum = -1;
	if (doc->Error()) {
		return false;
	} else {
		return true;
	}
}

const char * TXMLParser::getError () const {
	return doc->ErrorDesc();
}

// ------------------------------------------------------------------------
// XML node

int TXMLNode::numChilds () const {
	if (element) {
		if ( counter->childNum < 0 ) {
			TiXmlHandle handle(element);
			TiXmlElement *elem = handle.FirstChildElement().Element();
			if ( elem ) {
				int cnt = 0;
				while ( elem ) {
					 elem=elem->NextSiblingElement();
					cnt ++;
				}
				counter->childNum = cnt;
			} else {
				counter->childNum = 0;
			}
			return counter->childNum;
		} else {
			return counter->childNum;
		}
	} else {
		return 0;
	}
}

int TXMLNode::attribCount () const {
	if ( element ) {
		if ( counter->attribNum < 0 ) {
			TiXmlAttribute *attr = element->FirstAttribute();
			if ( attr ) {
				int cnt = 0;
				while ( attr ) {
					 attr=attr->Next();
					cnt ++;
				}
				counter->attribNum = cnt;
			} else {
				counter->attribNum = 0;
			}
			return counter->attribNum;
		} else {
			return counter->attribNum;
		}
	} else {
		return 0;
	}
}

std::string TXMLNode::getData () const {
	if ( element ) {
		const char *text = element->GetText();
		if ( text ) {
			return text;
		} else {
			return "";
		}
	} else {
		return "";
	}
}

std::string TXMLNode::getName () const {
	if ( element ) {
		const char *name = element->Value();
		if ( name ) {
			return name;
		} else {
			return "";
		}
	} else {
		return "";
	}
}

std::string TXMLNode::getAttrib ( const char *name ) const {
	if ( element ) {
		const char *attr = element->Attribute(name);
		if ( attr ) {
			return attr;
		} else {
			return "";
		}
	} else {
		return "";
	}
}

bool TXMLNode::getAttrib ( int idx, std::string &name, std::string &value ) const {
	if ( element ) {
		if ( idx < 0 || idx >= attribCount()) {
			return false;
		} else {
			int cnt = 0;
			TiXmlAttribute *attr = element->FirstAttribute();
			while ( cnt != idx ) {
				attr = attr->Next();
				cnt ++;
			}
			name = attr->Name();
			value = attr->Value();
			return true;
		}
	} else {
		return false;
	}
}

bool TXMLNode::getChild ( int idx, TXMLNode &node ) const {
	if ( element ) {
		TiXmlHandle handle(element);
		TiXmlElement *elem = handle.FirstChildElement().Element();
		if ( elem ) {
			int cnt = 0;
			while ( elem ) {
				if ( cnt == idx ) {
					node = TXMLNode(elem);
					return true;
				} else {
					cnt ++;
				}
				elem=elem->NextSiblingElement();
			}
			return false;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

bool TXMLNode::getChild ( const char *name, TXMLNode &node ) const {
	if ( element ) {
		TiXmlHandle handle(element);
		TiXmlElement *elem = handle.FirstChild(name).Element();
		if ( elem ) {
			node = TXMLNode(elem);
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}

std::string TXMLNode::toXML() const {
	int a =0;
	bool hasData = false;
	std::string xml ="<" + getName();
	std::string xmlData;

	std::string name, value;
	for ( a = 0; a < attribCount(); a ++ ) {
		getAttrib(a, name, value);
		xml += " ";
		xml += name;
		xml += "=\"";
		xml += value;
		xml += "\"";
	}

	if (getData().length()) {
		xmlData += getData();
		hasData = true;
	}

	if ( numChilds()) {
		hasData = true;
		for ( a = 0; a < numChilds(); a ++ ) {
			TXMLNode child;
			getChild(a,child);
			xmlData += child.toXML();
		}
	}

	if ( hasData ) {
		// closing tag
		xml += ">";
		xml += xmlData;
		xml += "</" + getName() + ">";
	} else {
		xml += "/>";
	}

	return xml;
}

TXMLNode& TXMLNode::operator = ( const TXMLNode &node ) {
	if (counter) {
		delete counter;
		if ( isCopy ) {
			delete element;
		}
	}
	isCopy = false;
	element = node.element;
	counter = new TXMLNodeCounter();
	counter->childNum = node.counter->childNum;
	counter->attribNum = node.counter->attribNum;
	return *this;
}

TXMLNode::~TXMLNode () {
	delete counter;
	if ( isCopy ) {
		delete element;
	}
}

void TXMLNode::Copy () {
	if ( isCopy ) {
		delete element;
	}
	element = new TiXmlElement(*element);
	isCopy = true;
}
