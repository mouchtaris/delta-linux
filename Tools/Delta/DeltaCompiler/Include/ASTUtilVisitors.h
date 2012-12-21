// ASTUtilVisitors.h
// Utility visitors for Delta ASTs.
// ScriptFighter Project.
// Y. Lilis, March 2012.
//

#ifndef	ASTUTILVISITORS_H
#define	ASTUTILVISITORS_H

#include "TreeVisitor.h"
#include "ASTTags.h"
#include "ASTNode.h"

namespace AST {

/////////////////////////////////////////////////////////

struct DCOMPLIB_CLASS Linearizer : public TreeVisitor {
	std::list<AST::Node*> nodes;
	static void Handle_All (AST_VISITOR_ARGS) { if (entering) ((Linearizer*) closure)->nodes.push_back((AST::Node*) node); }
	std::list<AST::Node*> operator()(AST::Node* root) { DASSERT(root); DPTR(root)->AcceptPreOrder(this); return nodes; }
	Linearizer (void) { SetDefaultHandler(&Handle_All, this); }
};

/////////////////////////////////////////////////////////

struct DCOMPLIB_CLASS SourceSetter : public TreeVisitor {
	const std::string source;
	static void Handle_All (AST_VISITOR_ARGS) { if (entering) ((AST::Node*) node)->SetSource(((SourceSetter*)closure)->source); }
	void operator()(AST::Node* root) { if (root) DPTR(root)->AcceptPreOrder(this); }
	SourceSetter (const std::string& source) : source(source) { SetDefaultHandler(&Handle_All, this); }
};

/////////////////////////////////////////////////////////

struct DCOMPLIB_CLASS LocationSetter : public TreeVisitor {
	AST::Node::Location				location;
	util_ui32						line;
	const std::string				source;
	AST::Node::SourceInfoReferences	references;
	static void Handle_All (AST_VISITOR_ARGS) {
		if (entering) {
			AST::Node* n = (AST::Node*) node;
			LocationSetter* instance = (LocationSetter*) closure;
			n->SetLocation(instance->location);
			n->SetLine(instance->line);
			n->SetSource(instance->source);
			n->AddSourceReferences(instance->references);
		}
	}
	void operator()(AST::Node* root) { if (root) DPTR(root)->AcceptPreOrder(this); }
	LocationSetter (
		const AST::Node::Location& location,
		util_ui32 line,
		const std::string& source,
		const AST::Node::SourceInfoReferences& references				
	) : location(location), line(line), source(source), references(references)
		{ DASSERT(line); SetDefaultHandler(&Handle_All, this); }
};

/////////////////////////////////////////////////////////

#define	AST_ATTRIBUTE_SERIAL	"serial"

//*****************************

struct DCOMPLIB_CLASS SerialProducer {
	util_ui32 currSerial;
	std::list<AST::Node*> operator()(AST::Node* root) {
		DASSERT(root);
		std::list<AST::Node*> nodes = Linearizer()(root);
		for (std::list<AST::Node*>::iterator i = nodes.begin(); i != nodes.end(); ++i)
			DPTR(*i)->SetAttribute(AST_ATTRIBUTE_SERIAL, ++currSerial);
		return nodes;
	}
	SerialProducer (void) : currSerial(0) {}
};

//*****************************

struct DCOMPLIB_CLASS SerialLocator : public TreeVisitor {
	AST::Node* node;
	util_ui32 serial;
	static void Handle_All (AST_VISITOR_ARGS) {
		if (const TreeAttribute* attr = DPTR(node)->GetAttribute(AST_ATTRIBUTE_SERIAL))
			if (attr->IsUInt() && attr->GetUInt() == ((SerialLocator*) closure)->serial) {
				((SerialLocator*) closure)->node = (AST::Node*) node;
				((SerialLocator*) closure)->Stop();
			}
	}
	AST::Node* operator()(AST::Node* root) { if (root) DPTR(root)->AcceptPreOrder(this); return node; }
	SerialLocator (util_ui32 serial) : node((AST::Node*) 0), serial(serial) { SetDefaultHandler(&Handle_All, this); }
};

//*****************************

struct DCOMPLIB_CLASS SerialMapper : public TreeVisitor {
	typedef std::map<util_ui32, Node*> Map;
	Map serials;
	static void Handle_All (AST_VISITOR_ARGS) {
		if (entering) {
			const TreeAttribute* attr = DPTR(node)->GetAttribute(AST_ATTRIBUTE_SERIAL);
			DASSERT(attr && attr->IsUInt());
			((SerialMapper*) closure)->serials[attr->GetUInt()] = (AST::Node*) node;
		}
	}
	Map operator()(AST::Node* root) { if (root) DPTR(root)->AcceptPreOrder(this); return serials; }
	SerialMapper (void) { SetDefaultHandler(&Handle_All, this); }
};

/////////////////////////////////////////////////////////

struct DCOMPLIB_CLASS SourceReferenceSetter {
	typedef std::map<util_ui32, AST::Node::SourceInfoReferences> SourceReferences;
	SourceReferences sourceRefs;

	void operator()(AST::Node* root) {
		DASSERT(root);
		if (!sourceRefs.empty()) {
			AST::SerialProducer()(root);
			const AST::SerialMapper::Map nodes = AST::SerialMapper()(root);
			for (SourceReferences::const_iterator i = sourceRefs.begin(); i != sourceRefs.end(); ++i) {
				AST::SerialMapper::Map::const_iterator iter = nodes.find(i->first);
				DASSERT(iter != nodes.end());
				DPTR(iter->second)->AddSourceReferences(i->second, false);
			}
		}
	}

	SourceReferenceSetter(const SourceReferences& sourceRefs) : sourceRefs(sourceRefs) {}
};

//*****************************

struct DCOMPLIB_CLASS SourceReferenceGetter {
	typedef std::map<util_ui32, AST::Node::SourceInfoReferences> SourceReferences;

	const SourceReferences operator()(AST::Node* root) {
		DASSERT(root);
		SourceReferences sourceRefs;
		const AST::SerialMapper::Map nodes = AST::SerialMapper()(root);
		for (AST::SerialMapper::Map::const_iterator i = nodes.begin(); i != nodes.end(); ++i)
			if (const AST::Node::SourceInfoReferences* refs = DPTR(i->second)->GetSourceReferences())
				sourceRefs[i->first] = *refs;
		return sourceRefs;
	}

	SourceReferenceGetter(void) {}
};

} // namespace AST

/////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
