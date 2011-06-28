#ifndef	XMLPARSEACTIONS_H
#define	XMLPARSEACTIONS_H

#include <string>
#include "DeltaObject.h"
#include "TagValue.h"
#include "XmlElement.h"


namespace XmlLoaderActions {

extern XmlElement*		Manage_ElementWithContent(TagValue* STag, DeltaObject* content, TagValue::TagName const* ETag);
extern XmlElement*		Manage_ElementWithoutContent(TagValue*);
extern TagValue*		Manage_Stag(TagValue::TagName*, TagValue*);
extern TagValue*		Manage_EmptyElemTag(TagValue::TagName*, TagValue*);

extern TagValue*		Manage_Attlist(TagValue::AttrKey*, TagValue::AttrValue*, TagValue*);
extern TagValue*		Manage_AttlistSpaces(void);
extern TagValue*		Manage_AttlistNothing(void);

extern DeltaObject*	Manage_ContentElement(DeltaObject*, XmlElement*);
extern DeltaObject*	Manage_ContentReference(DeltaObject*, std::string*);
extern DeltaObject*	Manage_ContentCdsect(DeltaObject*, std::string*);
extern DeltaObject*	Manage_ContentChardata(DeltaObject*, std::string*);
extern DeltaObject*	Manage_ContentNothing(void);

// TODO -- unused
//extern void				Manage_GEDecl(std::string*, std::string*);
//extern void				Manage_PEDecl(std::string*, std::string*);

}	//namespace XmlLoaderActions



#endif