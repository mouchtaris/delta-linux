// ASTChainOfSourceLineOriginInfoNode.h
// Custom AST node functionality.
// ScriptFighter Project.
// Y. Lilis, September 2013.
// Separated from AST::Node.
//
 
#ifndef	ASTCHAINOFSOURCELINEORIGININFO_H
#define	ASTCHAINOFSOURCELINEORIGININFO_H

#include "DeltaCompilerDefs.h"
#include "utypes.h"
#include "DDebug.h"
#include <string>
#include <list>
#include <map>

/////////////////////////////////////////////////////////

#define DELTA_AST_CHAIN_SOURCE_LINE_ORIGIN	"chainOfSourceLineOrigin"

/////////////////////////////////////////////////////////

namespace AST {

class Node;

/////////////////////////////////////////////////////////

struct DCOMPLIB_CLASS SourceLineOriginInfo {
	std::string symbolicURI;
	util_ui32	line;

	SourceLineOriginInfo(const std::string& symbolicURI, util_ui32 line) :
		symbolicURI(symbolicURI), line(line) {}
};

/////////////////////////////////////////////////////////

typedef std::list<SourceLineOriginInfo>	ChainOfSourceLineOriginInfo;

DCOMPLIB_FUNC void									AddSourceLineOrigin	(AST::Node* node, const SourceLineOriginInfo& info, bool front = true);
DCOMPLIB_FUNC void									AddChainOfSourceLineOrigin (AST::Node* node, const ChainOfSourceLineOriginInfo& refs, bool front = true);
DCOMPLIB_FUNC const ChainOfSourceLineOriginInfo*	GetChainOfSourceLineOrigin (const AST::Node* node);

/////////////////////////////////////////////////////////

typedef util_ui32 NodeSerialNo;
typedef std::map<NodeSerialNo, AST::ChainOfSourceLineOriginInfo> NodeToChainOfSourceLineOriginInfo;

/////////////////////////////////////////////////////////

}	// AST namespace

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
