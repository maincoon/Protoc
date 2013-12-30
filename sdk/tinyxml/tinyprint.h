#include "tinyxml.h"

#ifndef TINY_PRINT_XML_INCLUDED
#define TINY_PRINT_XML_INCLUDED

// ----------------------------------------------------------------------
// STDOUT dump and indenting utility functions
// ----------------------------------------------------------------------
void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 );
void dump_to_stdout(const char* pFilename);

#endif
