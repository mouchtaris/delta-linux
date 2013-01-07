// ASTNode.h
// Generic node covering all cases of Delta ASTs.
// ScriptFighter Project.
// A. Savidis, September 2009.
//
 
#ifndef	ASTNODE_H
#define	ASTNODE_H

#include "utypes.h"
#include "DDebug.h"
#include "TreeNode.h"
#include "ASTTags.h"
#include "AutoCollection.h"
#include "CompilerComponentDirectory.h"
#include <map>
#include <string>

/////////////////////////////////////////////////////////

// This is a source line that is taken during parsing,
// being the first line of an expression that is translated
// to quads, and is then forced as the line of the first
// generated quad of this expression.
#define	DELTA_AST_LINE_ATTRIBUTE				"line"

#define	DELTA_AST_STARTLINE_ATTRIBUTE			"startline"
#define	DELTA_AST_ENDLINE_ATTRIBUTE				"endline"
#define	DELTA_AST_STARTPOS_ATTRIBUTE			"startpos"
#define	DELTA_AST_ENDPOS_ATTRIBUTE				"endpos"

#define DELTA_AST_SOURCE_ATTRIBUTE				"source"
#define DELTA_AST_SOURCE_REFERENCES_ATTRIBUTE	"sourceReferences"

/////////////////////////////////////////////////////////

namespace AST {

class Node;

/////////////////////////////////////////////////////////

struct NodeList : public AutoCollectable {
	friend class AutoCollectableFactory<NodeList>;

	std::list<Node*> l;
	void Add (Node* x) { l.push_back(x); }

	private:
	NodeList (void) {}
	~NodeList(){}
};

typedef std::list<std::string>	IdList;
typedef std::list<std::string>	StringList;

#define	NIL_NODE		(Node*) 0
#define	NIL_NODELIST	(NodeList*) 0
#define	NIL_IDLIST		(IdList*) 0
#define	NIL_STRINGLIST	(StringList*) 0

/////////////////////////////////////////////////////////

class DCOMPLIB_CLASS Node : public TreeNode, public AutoCollectable {
	
	DFRIENDDESTRUCTOR()
	friend class Factory;

	public:
	struct Location {
		util_ui32 startLine;
		util_ui32 endLine;
		util_ui32 startPos;
		util_ui32 endPos;

		Location (void) : startLine(0), endLine(0), startPos(0), endPos(0) {}
		Location (
			util_ui32 startLine,
			util_ui32 endLine,
			util_ui32 startPos,
			util_ui32 endPos
		) : startLine(startLine), endLine(endLine), startPos(startPos), endPos(endPos) {}
	};

	typedef std::pair<std::string, util_ui32>	SourceInfo;
	typedef std::list<SourceInfo>				SourceInfoReferences;


	protected:
	virtual void				Delete (void) { DDELETE(this); }

	public:

	void						AddSourceReference	(const SourceInfo& info, bool front = true);
	void						AddSourceReferences	(const SourceInfoReferences& refs, bool front = true);
	const SourceInfoReferences*	GetSourceReferences	(void) const;

	void						SetLocation (const Location& location);
	const Location				GetLocation (void) const;

	void						SetSource (const std::string& source);
	const std::string			GetSource (void) const;

#define DECLARE_UINT_ATTRIBUTE(getter, setter)	\
	util_ui32 getter (void) const;				\
	void setter (util_ui32 val)

	DECLARE_UINT_ATTRIBUTE(GetLine, SetLine);
	DECLARE_UINT_ATTRIBUTE(GetStartLine, SetStartLine);
	DECLARE_UINT_ATTRIBUTE(GetEndLine, SetEndLine);
	DECLARE_UINT_ATTRIBUTE(GetStartPos, SetStartPos);
	DECLARE_UINT_ATTRIBUTE(GetEndPos, SetEndPos);

#undef DECLARE_UINT_ATTRIBUTE

	void				AddChildren (NodeList* nodes);
	void				AddChildrenEx (NodeList* nodes, const std::string& childTag, const std::string& childId);
	static Node*		Extend (Node* child, const std::string& tag, const std::string& childId);

	virtual TreeNode*	Clone	(void) const;

	private:
	Node*				New (const std::string& tag) const;
	Node (const std::string& tag);
	virtual ~Node(){}
};

/////////////////////////////////////////////////////////

class DCOMPLIB_CLASS Factory {
	private:
	AutoCollector* collector;
	
	public:
	void SetAutoCollector(AutoCollector* c) { collector = c; }

	NodeList*	NewNodeList	(void) const;
	Node*		NewNode		(const std::string& tag) const;

	Factory (void) : collector((AutoCollector*) 0) {}
	~Factory() {}
};

/////////////////////////////////////////////////////////

}	// AST namespace

/////////////////////////////////////////////////////////

#define ASTFACTORY_EX(component_directory)	\
	(*DNULLCHECK(UCOMPONENT_DIRECTORY_GET(*(component_directory), AST::Factory)))

#define ASTFACTORY	ASTFACTORY_EX(COMPONENT_DIRECTORY())

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
