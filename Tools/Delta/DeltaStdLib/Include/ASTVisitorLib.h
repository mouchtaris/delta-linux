// ASTVisitorLib.h
// Visitor for ast traversal.
// ScriptFighter Project.
// Y. Lilis, August 2011.
//

#ifndef	ASTVISITORLIB_H
#define	ASTVISITORLIB_H

#include "DeltaCollectableContainer.h"
#include "DeltaLibraryObjectCreator.h"
#include "DeltaStdLibTemplatesUtils.h"
#include "DeltaStdClassNames.h"
#include "TreeVisitor.h"
#include <map>

//////////////////////////////////////////////////////////////////////

#define HANDLER_TAG				"handlers"
#define CONTEXT_HANDLER_TAG		"context_handlers"
#define DEFAULT_HANDLER_TAG		"default_handler"

class DVM_CLASS TreeVisitorWrapper : public CollectableContainer {
	public:
	typedef std::pair<DeltaValue, TreeVisitorWrapper*>	Handler;
	typedef std::multimap<std::string, Handler*>		HandlerMap;
	
	protected:
	DYNAMIC_CAST_SINGLEBASE_INLINE_IMPL(CollectableContainer, ASTVISITOR_TYPE_STR)

	private:
	TreeVisitor* visitor;
	HandlerMap handlers;

	void	ConvertToStringHandler (
				void (DeltaValue::*	converter)(DeltaString*) const, 
				DeltaString*			at, 
				void*					closure
			) {
			
			TreeVisitorWrapper* adapter = VGET_INST(TreeVisitorWrapper, closure, ASTVISITOR_TYPE_STR);
			DASSERT(adapter && adapter == this);

			if (GetPrintState().IsInProgress())
				at->Add(uconstructstr("self_%s", ASTVISITOR_TYPE_STR));
			else {

				DELTA_CONTAINER_TOSTRING_PROCESSING();
				DPTR(at)->Add(uconstructstr("%s(0x%x)[", ASTVISITOR_TYPE_STR, GetSerialNo()));
				
				typedef std::list< std::pair<std::string, std::string> > List;
				List l;
				l.push_back(std::make_pair(HANDLER_TAG, "Handlers"));
				l.push_back(std::make_pair(CONTEXT_HANDLER_TAG, "Context Handlers"));
				l.push_back(std::make_pair(DEFAULT_HANDLER_TAG, "Default Handler"));
				
				for (List::const_iterator iter = l.begin(); iter != l.end(); ++iter) {
					HandlerMap::const_iterator begin = handlers.lower_bound(iter->first);
					HandlerMap::const_iterator end = handlers.upper_bound(iter->first);
					if (begin != end) {	// Only add content for categories with handlers
						at->Add(uconstructstr("%s{", ucstringarg(iter->second)));
						bool needComma = false;
						for (HandlerMap::const_iterator i = begin; i != end; ++i) {
							if (needComma)
								at->Add(",");
							else
								needComma = true;
							((i->second->first).*converter)(at);
						}
						at->Add("}");
					}
				}
				at->Add("]");
		}
	}

	public:
	TreeVisitor* GetVisitor (void) const { return visitor; }

	void AddHandler (const std::string& tag, Handler* handler) {
		if (tag == DEFAULT_HANDLER_TAG) {	//only one default handler
			HandlerMap::iterator begin = handlers.lower_bound(DEFAULT_HANDLER_TAG);
			HandlerMap::iterator end = handlers.upper_bound(DEFAULT_HANDLER_TAG);
			std::for_each(begin, end, udestroy_second<HandlerMap::value_type>());
			handlers.erase(begin, end);
		}
		handlers.insert(HandlerMap::value_type(tag, handler));
		SetContained(&DPTR(handler)->first);
	}

	const HandlerMap& GetHandlers (void) const { return handlers; }

	//CollectableContainer method implementation

	virtual void		ExplicitDestructor (void) {
							std::for_each(handlers.begin(), handlers.end(), udestroy_second<HandlerMap::value_type>());
							handlers.clear();
							DDELETE(visitor);
						}
	virtual void		DeleteMemory (void) { DDELETE(this); }

	virtual void		ConvertToString (DeltaString* at, void* closure) { 
							DASSERT(DeltaExternId::UseConvertToString());  
							ConvertToStringHandler(&DeltaValue::ConvertToString, at, closure);
						}

	virtual void		ConvertToStringBySkippingUserCode (DeltaString* at, void* closure) { 
							DASSERT(DeltaExternId::UseConvertToStringBySkippingUserCode());  
							ConvertToStringHandler(
								&DeltaValue::ConvertToStringBySkippingUserCode, 
								at, 
								closure
							);
						}

	virtual void ToValue (DeltaValue* at);

	TreeVisitorWrapper (void) : visitor(DNEW(TreeVisitor)) {}
	~TreeVisitorWrapper() {}
};

//////////////////////////////////////////////////////////////////////

#define _DLIB_GET_ASTVISITOR_WRAPPER_EX_(name)																\
	DLIB_ARGEX(name##_val, ASTVISITOR_TYPE_STR);															\
	TreeVisitorWrapper* name = VGET_INST(TreeVisitorWrapper, DLIB_ARGVAL(name##_val), ASTVISITOR_TYPE_STR);


#define	DLIB_GET_ASTVISITOR_EX(name)				\
	TreeVisitor* name = (TreeVisitor*) 0;			\
	{												\
		_DLIB_GET_ASTVISITOR_WRAPPER_EX_(wrapper)	\
		name = wrapper->GetVisitor();				\
	}

#define	DLIB_GET_ASTVISITOR		DLIB_GET_ASTVISITOR_EX(visitor)

//////////////////////////////////////////////////////////////////////

extern void Install_DeltaASTVisitor_Lib (DeltaObject* mainAPI);
extern void CleanUp_DeltaASTVisitor_Lib (void);

#endif	// Do not add stuff beyond this point.