#include <stdio.h>

#include "protoc/ProtocData.h"
#include "protoc/ProtocSchema.h"
#include "protoc/ProtocFormatter.h"

using namespace Protoc;

int main  ( int f, char **ff) {
	if ( f < 3 ) {
		printf ( "usage: %s <schema.xml> <snippets.xml> [output file]\n", ff[0]);
	} else {
		NamespaceDescriptor nsDesc;
		ProtocSchema protoc(&nsDesc);
		if (protoc.ReadSchema(ff[1])) {
			ProtocFormatter formatter(&nsDesc);
			if ( formatter.ReadSnippets(ff[2])) {
				if ( protoc.CheckIntegrity()) {
					if (f == 4) {
						formatter.Format(ff[3]);
					} else {
						formatter.Format();
					}
				}
			}
		}
	}
	return 0;
}
