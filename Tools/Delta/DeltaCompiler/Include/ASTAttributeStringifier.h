// ASTAttributeStringifier.h
// Attribute stringifier for Delta AST.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#ifndef	ASTATTRIBUTESTRINGIFIER_H
#define	ASTATTRIBUTESTRINGIFIER_H

#include "TreeToStringVisitor.h"

namespace AST {

/////////////////////////////////////////////////////////

class AttributeStringifier : public ::AttributeStringifier  {

	public:
	static void Handle_Name (ATTR_STRINGIFIER_ARGS);
	static void Handle_Function (ATTR_STRINGIFIER_ARGS);

	static void Handle_DotIndexIdent  (ATTR_STRINGIFIER_ARGS);
	static void Handle_DotIndexOpString  (ATTR_STRINGIFIER_ARGS);
	static void Handle_BracketIndexOperatorMethod  (ATTR_STRINGIFIER_ARGS);

	static void Handle_DottedIdent (ATTR_STRINGIFIER_ARGS);
	static void Handle_OperatorIndex (ATTR_STRINGIFIER_ARGS);

	static void Handle_UsingByteCodeLibrary (ATTR_STRINGIFIER_ARGS);
	static void Handle_StringConst (ATTR_STRINGIFIER_ARGS);
	static void Handle_StringifyDottedIdents (ATTR_STRINGIFIER_ARGS);
	
	static void Handle_NumConst (ATTR_STRINGIFIER_ARGS);
	static void Handle_BoolConst (ATTR_STRINGIFIER_ARGS);

	static void	DefaultHandler (ATTR_STRINGIFIER_ARGS);

	public:

	AttributeStringifier (void);
	virtual ~AttributeStringifier(){}
};

} // namespace AST

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
