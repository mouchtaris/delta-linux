#include <iostream>
#include "XmlParseActions.h"
#include "XmlLoaderErrorMsg.h"
#include "ReferenceTranslation.h"
#include "uptr.h"

template <typename T>
struct ptrftr {
	typedef T* type;
	static type const New(void) throw() {
		return 0x00;
	}
	static type& New (type& ptr) {
		unullify(ptr);
		return ptr;
	}
};
template <typename T>
typename ptrftr<T>::type ptrinit(T*& ptr) { return ptrftr<T>::New(ptr); }
#define NEWPTR(TYPE, VAR) TYPE* VAR = ptrftr<TYPE>::New()


static void					Set(DeltaTable&, DeltaValue const&, DeltaValue const&);

#define	CHARDATA_DOBJ_INDEX	"$CharData"

#define ATTRIBUTES_KEY "$Attributes"
static void _addAttrToObj (DeltaObject* obj, TagValue::Attribute& attribute, char const* tagname) {
	DeltaTable* attrHolder;
	const DeltaValue AttributesIndex(ATTRIBUTES_KEY);

	// Make sure an "$Attributes" field exists in the object
	DeltaValue attrHolderValue;
	DPTR(obj)->Get(AttributesIndex, &attrHolderValue);
	if (attrHolderValue.Type() == DeltaValue_Nil) {
		attrHolderValue.FromTable(attrHolder = DeltaTable::NewTable());
		DPTR(obj)->Set(AttributesIndex, attrHolderValue);
	}
	else {
		DASSERT(attrHolderValue.Type() == DeltaValue_Table);
		if (attrHolderValue.Type() != DeltaValue_Table)
			attrHolder = 0x00;
		else
			attrHolder = attrHolderValue.ToTable();
	}

	DeltaValue index(attribute.first->c_str());
	DeltaValue value(attribute.second->c_str());
	DeltaValue old_value;
	DPTR(attrHolder)->Get(index, &old_value);
	if (old_value.Type() != DeltaValue_Nil)
		XmlLoaderError("Name collision between attributes "
			"for name \"%s\" in tag \"%s\".",
			attribute.first->c_str(), tagname);
	else
		DPTR(attrHolder)->Set(index, value);

	udelete(attribute.first);
	udelete(attribute.second);
}

XmlElement* XmlLoaderActions::Manage_ElementWithContent(TagValue* STag,
													 DeltaObject* content,
													 TagValue::TagName const* ETag)
{
	NEWPTR(XmlElement, retValue);

	//Check if STag.Name == ETag
	if ( *(DPTR(STag)->GetName()) != *ETag )
		XmlLoaderError("Starting tag \"%s\" and ending tag \"%s\" do not match.",
			STag->GetName()->c_str(), ETag->c_str());

	else
	if (content) {
		retValue = DNEW(XmlElement);
		retValue->SetName(DPTR(DPTR(STag)->GetName()));

		//Add attlist to content;
		while ( DPTR(STag)->HasAttribute() ) {
			TagValue::Attribute attribute(DPTR(STag)->GetAttribute());
			_addAttrToObj(DPTR(content), attribute, DPTR(STag)->GetName()->c_str());
		}

		retValue->SetContent(content);
	}

	
	// Clean up
	udelete(STag);
	udelete(ETag);
	if (!retValue && content) // if there has been an error, also clean up "contant"
		DeltaObject::NativeCodeHelpers::GiveUp(content);

	return retValue;
}

XmlElement* XmlLoaderActions::Manage_ElementWithoutContent(TagValue* Tag)
{
	return Manage_ElementWithContent(
		Tag,
		DeltaObject::NativeCodeHelpers::NewObject(),
		DNEWCLASS(TagValue::TagName, (*DPTR(DPTR(Tag)->GetName())))
	);
}

TagValue* XmlLoaderActions::Manage_Stag(TagValue::TagName* name, TagValue* attlist)
{
	DPTR(attlist)->SetName(DPTR(name));
	return attlist;
}

TagValue* XmlLoaderActions::Manage_EmptyElemTag(TagValue::TagName* name, TagValue* attlist)
{
	return Manage_Stag(name, attlist);
}

TagValue* XmlLoaderActions::Manage_Attlist(TagValue::AttrKey* name,
										   TagValue::AttrValue* attvalue,
										  TagValue* attlist)
{
	DPTR(attlist)->SetAttribute(name, ReferenceTranslation::Singleton()->SubstituteReferences(attvalue));
	return attlist;
}

TagValue* XmlLoaderActions::Manage_AttlistNothing(void)
{
	NEWPTR(TagValue, retValue);
	return unew(retValue);
}

TagValue* XmlLoaderActions::Manage_AttlistSpaces(void)
{
	return Manage_AttlistNothing();
}

DeltaObject* XmlLoaderActions::Manage_ContentElement(DeltaObject* content,
												 XmlElement* element)
{
	NEWPTR(DeltaObject, retValue);

	if (element && content) {

		DeltaValue index(DPTR(DPTR(element)->GetName())->c_str());
		DeltaValue value(element->GetContent());

		Set(*content, index, value);

		retValue = content;
	}

	// (XML)element not needed any more (and nether are its included strings)
	if (content)
		DeltaObject::NativeCodeHelpers::GiveUp(element->GetContent());
	if (element) {
		DDELETE(element->GetName());
		udelete(element);
	}

	return retValue;
}

DeltaObject* XmlLoaderActions::Manage_ContentReference(DeltaObject* content,
												   std::string* reference)
{
	NEWPTR(DeltaObject, retValue);

	if (content) {
		std::string* str;
		if ((*DPTR(reference))[0] == '#')
			str = ReferenceTranslation::Singleton()->CharRefValue(*reference);
		else
			str = ReferenceTranslation::Singleton()->EntityRefValue(*reference);

		DeltaValue index(CHARDATA_DOBJ_INDEX);
		DeltaValue value(str);
		Set(*content, index, value);

		udelete(str);

		retValue = content;
	}

	udelete(reference);
	return retValue;
}

DeltaObject* XmlLoaderActions::Manage_ContentCdsect(DeltaObject* content,
												std::string* cdsection)
{
	NEWPTR(DeltaObject, retValue);

	if (content) {
		DeltaValue index(CHARDATA_DOBJ_INDEX);
		DeltaValue value(*ReferenceTranslation::Singleton()->SubstituteReferences(DPTR(cdsection)));
		Set(*content, index, value);

		retValue = content;
	}

	udelete(cdsection);
	return retValue;
}

DeltaObject* XmlLoaderActions::Manage_ContentChardata(DeltaObject* content,
												  std::string* chardata)
{
	NEWPTR(DeltaObject, retValue);

	if (content) {
		DeltaValue index(CHARDATA_DOBJ_INDEX);
		DeltaValue value(*chardata);
		Set(*content, index, value);

		retValue = content;
	}

	udelete(chardata);
	return retValue;
}

DeltaObject* XmlLoaderActions::Manage_ContentNothing(void)
{
	return DeltaObject::NativeCodeHelpers::NewObject();
}

// TODO -- unused
//void XmlLoaderActions::Manage_GEDecl(std::string* index,
//							  std::string* value)
//{
//	ReferenceTranslation::Singleton()->AddReference(*index, *value);
//}

// TODO -- unused
//void XmlLoaderActions::Manage_PEDecl(std::string* index,
//							  std::string* value)
//{
//	ReferenceTranslation::Singleton()->AddPEReference(*index, *value);
//}

static void Set(DeltaTable& table, DeltaValue const& index, DeltaValue const& value)
{
	NEWPTR(DeltaTable, itemsForIndex);
	
	DeltaValue prev_value;
	table.Get(index, &prev_value);
	if (prev_value.Type() == DeltaValue_Nil) {
		itemsForIndex = DeltaObject::NativeCodeHelpers::NewObject();
		table.Set(index, DeltaValue(itemsForIndex));
	}
	else {
		DASSERT( prev_value.Type() == DeltaValue_Table );
		itemsForIndex = prev_value.ToTable();
	}

	DeltaValue next_index((DeltaNumberValueType)DPTR(itemsForIndex)->Total());
	itemsForIndex->Set(next_index, value);
}
