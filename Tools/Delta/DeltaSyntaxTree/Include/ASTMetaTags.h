// ASTMetaTags.h
// Tags for metaprogramming annotation nodes of Delta ASTs.
// ScriptFighter Project.
// Y. Lilis, May 2011.
//
 
#ifndef	ASTMETATAGS_H
#define	ASTMETATAGS_H

/////////////////////////////////////////////////////////
// NODE TAGS (TYPES).
//
#define AST_TAG_QUOTED_ELEMENTS					"QuotedElements"
#define	AST_TAG_QUASI_QUOTES					"QuasiQuotes"
#define	AST_TAG_ESCAPE							"Escape"
#define	AST_TAG_INLINE							"Inline"
#define	AST_TAG_EXECUTE							"Execute"

/////////////////////////////////////////////////////////
// NODE ATTRIBUTES AND CHILD NAMES (FIELDS).
//
#define	AST_ATTRIBUTE_CARDINALITY				"cardinality"
#define AST_ATTRIBUTE_REF_COUNTER				"refCounter"

#define	CARDINALITY(node)	\
		(DPTR(DPTR(node)->GetAttribute(AST_ATTRIBUTE_CARDINALITY))->GetUInt())

/////////////////////////////////////////////////////////
// NODE ATTRIBUTES VALUES (ENUMERATED).
//

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

