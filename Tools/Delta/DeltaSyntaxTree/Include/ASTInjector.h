// ASTInjector.h
// AST injection logic for escapes and inlines.
// ScriptFighter Project.
// Y. Lilis, August 2011.
//

#ifndef	ASTINJECTOR_H
#define	ASTINJECTOR_H

#include "DeltaSyntaxTreeDefs.h"
#include "TreeVisitor.h"
#include "ucallbacks.h"
#include <list>
#include <map>

class SYNTAXTREELIB_CLASS ASTInjector {

	public:
	typedef std::list<std::string> Hierarchy;

	class Conversion {
		public:
		typedef ucallbackwithclosure<TreeNode* (*)(const std::string&, void*)> NodeCreatorFunc;
		typedef void (*NodeDeleterFunc)(TreeNode*&);

		protected:
		NodeCreatorFunc creator;
		NodeDeleterFunc deleter;

		TreeNode*	Create	(const std::string& tag) const { return creator(tag); }
		TreeNode*	Extend	(TreeNode* child, const std::string& tag, const std::string& childId = "") const;
		void		Delete	(TreeNode* node) const { DASSERT(deleter); (*deleter)(node); }

		public:

		TreeNode*	GetSingleNode (TreeNode* node) const;
		void		DefaultReplace (TreeNode* target, TreeNode* node) const;
		void		RemoveLastChildWithinQuasiQuotes(TreeNode *node, TreeNode **newAst) const;
			
		virtual bool IsValid	(TreeNode* target, TreeNode* node) const { return false; }
		virtual void operator()	(TreeNode* target, TreeNode* node, TreeNode** newAst) const { DASSERT(false); }

		Conversion (NodeCreatorFunc creator, NodeDeleterFunc deleter) : creator(creator), deleter(deleter) {}
		virtual ~Conversion() {}
	};

	typedef Conversion::NodeCreatorFunc NodeCreatorFunc;
	typedef Conversion::NodeDeleterFunc NodeDeleterFunc;

	private:
	NodeCreatorFunc creator;
	NodeDeleterFunc deleter;

	typedef std::map<std::string, Conversion*> ConversionMap;
	ConversionMap convertors;
	ConversionMap contextConversions;
	ConversionMap listConversions;

	typedef std::pair<Hierarchy, Conversion*>	HierarchyConversion;
	typedef std::list<HierarchyConversion>		HierarchyConversionList;
	HierarchyConversionList hierarchyConversions;

	bool MatchesHierarchy (TreeNode* node, const Hierarchy& hierarchy) const;
	void Delete	(TreeNode* node) const { DASSERT(deleter); (*deleter)(node); }

	public:
	void RegisterContextConversion (const std::string& parentTag, const std::string& childId, Conversion* conversion);
	void RegisterListConversion (const std::string& parentTag, Conversion* conversion);
	void RegisterHierarchyConversion (const Hierarchy& hierarchy, Conversion* conversion);

	void operator()(TreeNode* target, TreeNode* ast, TreeNode** newAst = (TreeNode**) 0) const;

	ASTInjector (NodeCreatorFunc creator, NodeDeleterFunc deleter = &TreeNode::Delete);
	~ASTInjector();
};

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.