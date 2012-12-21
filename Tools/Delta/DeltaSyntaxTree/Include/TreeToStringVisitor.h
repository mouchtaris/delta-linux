// TreeToStringVisitor.h
// A simple visitor to make a string out of a tree
// mainly for testing purposes.
// ScriptFighter Project.
// A. Savidis, September 2009.
//

#ifndef	TREETOSTRINGVISITOR_H
#define	TREETOSTRINGVISITOR_H

#include "TreeNode.h"
#include "TreeVisitor.h"
#include "utypes.h"
#include <string>

/////////////////////////////////////////////////////////

#define	ATTR_STRINGIFIER_ARGS				\
		const std::string&		attr,		\
		const TreeAttribute*	data,		\
		std::string*			result,		\
		void*					closure

class SYNTAXTREELIB_CLASS AttributeStringifier  {

	public:
	typedef void (*Handler)(ATTR_STRINGIFIER_ARGS);

	protected:
	typedef ucallbackwithclosure<Handler>			HandlerCallback;
	typedef std::map<std::string, HandlerCallback>	Handlers;
	Handlers handlers;
	HandlerCallback	defaultHandler;

	public:
	void						SetHandler (
									const std::string&	nodeTag, 
									Handler				f, 
									void*				c = (void*) 0
								);

	void						SetDefaultHandler (Handler f, void* c = (void*) 0)
									{ defaultHandler.set(f,c); }

	virtual const std::string	operator()(
									const std::string&		nodeTag, 
									const std::string&		attr, 
									const TreeAttribute*	data
								) const;

	AttributeStringifier (void){}
	virtual ~AttributeStringifier(){}
};

/////////////////////////////////////////////////////////

class SYNTAXTREELIB_CLASS TreeToStringVisitor : public TreeVisitor {

	private:
	util_ui32					tabstops;
	std::string					result;
	const AttributeStringifier&	attrStringifier;

	static void					ToStringHandler (
									TreeNode*			node, 
									const std::string&	id, 
									bool				entering, 
									void*				closure
								);

	public:
	const AttributeStringifier&	GetAttributeStringifier (void) const 
									{ return attrStringifier; }
	const std::string			operator()(TreeNode* node, const std::string& id = "");

	TreeToStringVisitor (const AttributeStringifier& stringifier = AttributeStringifier());
	virtual ~TreeToStringVisitor();
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
