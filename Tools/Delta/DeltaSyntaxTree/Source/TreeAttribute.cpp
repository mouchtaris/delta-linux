// TreeAttribute.cpp
// Attribute support for TreeNodes.
// ScriptFighter Project.
// Y.Lilis, August 2011.
//

#include "TreeAttribute.h"
#include "ustrings.h"

/////////////////////////////////////////////////////////

void TreeAttribute::Clear (void) {
	if (IsString() && content.stringVal)
		udelete(content.stringVal);
	else if (IsStringList() && content.stringListVal)
		udelete(content.stringListVal);
	else if (IsPointer()) { 
		if (content.pointerVal->data && content.pointerVal->cleaner)
			(*content.pointerVal->cleaner)(content.pointerVal->data);
		udelete(content.pointerVal);
	}
}

//*********************

int TreeAttribute::GetInt (void) const {
	DASSERT(IsInt());
	return content.intVal;
}

void TreeAttribute::SetInt (int val) {
	Clear();
	type = AttributeTypeInt;
	content.intVal = val;
}

//*********************

util_ui32 TreeAttribute::GetUInt (void) const {
	DASSERT(IsUInt());
	return content.uintVal;
}

void TreeAttribute::SetUInt (util_ui32 val) {
	Clear();
	type = AttributeTypeUInt;
	content.uintVal = val;
}

//*********************

double TreeAttribute::GetDouble (void) const {
	DASSERT(IsDouble());
	return content.doubleVal;
}

void TreeAttribute::SetDouble (double val) {
	Clear();
	type = AttributeTypeDouble;
	content.doubleVal = val;
}

//*********************

bool TreeAttribute::GetBool (void) const {
	DASSERT(IsBool());
	return content.boolVal;
}

void TreeAttribute::SetBool (bool val) {
	Clear();
	type = AttributeTypeBool;
	content.boolVal = val;
}

//*********************

const std::string& TreeAttribute::GetString (void) const {
	DASSERT(IsString());
	return *content.stringVal;
}

void TreeAttribute::SetString (const std::string& str) {
	Clear();
	type = AttributeTypeString;
	content.stringVal = DNEWCLASS(std::string, (str));
}

//*********************

TreeAttribute::StringList& TreeAttribute::GetStringList (void) {
	DASSERT(IsStringList());
	return *content.stringListVal;
}

const TreeAttribute::StringList& TreeAttribute::GetStringList (void) const {
	DASSERT(IsStringList());
	return *content.stringListVal;
}

void TreeAttribute::SetStringList (const StringList& list) {
	Clear();
	type = AttributeTypeStringList;
	content.stringListVal = CopyStringList(list);
}

//*********************

void* TreeAttribute::GetPointer (void) {
	DASSERT(IsPointer());
	return content.pointerVal->data;
}

const void* TreeAttribute::GetPointer (void) const {
	DASSERT(IsPointer());
	return content.pointerVal->data;
}

void TreeAttribute::SetPointer (void* val, CopierFunc copier, CleanerFunc cleaner) {
	Clear();
	type = AttributeTypePointer;
	content.pointerVal = DNEWCLASS(PointerValue, (val, copier, cleaner));
}

//*********************

bool TreeAttribute::operator ==(const TreeAttribute& attr) {
	if(type != attr.type)
		return false;
	else
		switch(type) {
			case AttributeTypeInt:			return content.intVal	 == attr.content.intVal;
			case AttributeTypeDouble:		return content.doubleVal == attr.content.doubleVal;
			case AttributeTypeBool:			return content.boolVal	 == attr.content.boolVal;
			case AttributeTypeString:		return *content.stringVal	 == *attr.content.stringVal;
			case AttributeTypeStringList:	return *content.stringListVal == *attr.content.stringListVal;
			case AttributeTypePointer:		return content.pointerVal->data == attr.content.pointerVal->data;
			default:						DASSERT(false);	return false;	
		}
}

//*********************

const std::string TreeAttribute::toString(void) const {
	switch(type) {
		case AttributeTypeInt:			return unum2string(GetInt());
		case AttributeTypeUInt:			return unum2string(GetUInt());
		case AttributeTypeDouble:		return unum2string(GetDouble());
		case AttributeTypeBool:			return ubool2conststr(GetBool());
		case AttributeTypeString:		return "\"" + GetString() + "\"";
		case AttributeTypeStringList:	{
			std::string str;
			const StringList& l = GetStringList();
			bool first = true;
			for(StringList::const_iterator i = l.begin(); i != l.end(); ++i) {
				if (first)
					first = false;
				else
					str += ", ";
				str += *i;
			}
			return str;
		}
		case AttributeTypePointer:		return uconstructstr("0x%x", (util_ui32) content.pointerVal->data);
		default:						DASSERT(false);	return std::string();
	}
}

//*********************

TreeAttribute::TreeAttribute (util_i32 val)				: type(AttributeTypeInt)		{ content.intVal = val;		}
TreeAttribute::TreeAttribute (util_ui32 val)			: type(AttributeTypeUInt)		{ content.uintVal = val;	}
TreeAttribute::TreeAttribute (double val)				: type(AttributeTypeDouble)		{ content.doubleVal = val;	}
TreeAttribute::TreeAttribute (bool val)					: type(AttributeTypeBool)		{ content.boolVal = val;	}
TreeAttribute::TreeAttribute (const char *val)			: type(AttributeTypeString)		{ DASSERT(val); content.stringVal = DNEWCLASS(std::string, (val)); }
TreeAttribute::TreeAttribute (const std::string& val)	: type(AttributeTypeString)		{ content.stringVal = DNEWCLASS(std::string, (val.c_str())); }
TreeAttribute::TreeAttribute (StringList* val)			: type(AttributeTypeStringList)	{ DASSERT(val); content.stringListVal = val; }
TreeAttribute::TreeAttribute (const StringList& val)	: type(AttributeTypeStringList)	{ content.stringListVal = CopyStringList(val); }
TreeAttribute::TreeAttribute (void* val, CopierFunc copier, CleanerFunc cleaner) : type(AttributeTypePointer)
	{ content.pointerVal = DNEWCLASS(PointerValue, (val, copier, cleaner)); }

TreeAttribute::TreeAttribute (const TreeAttribute& attr) {
	if (this != &attr) {
		memcpy(this, &attr, sizeof(TreeAttribute));
		if (IsString())
			content.stringVal = DNEWCLASS(std::string, (*content.stringVal));
		else if (IsStringList())
			content.stringListVal = CopyStringList(*content.stringListVal);
		else if (IsPointer()) { 
			void* data = content.pointerVal->copier ? (*content.pointerVal->copier)(content.pointerVal->data) : content.pointerVal->data;
			content.pointerVal = DNEWCLASS(PointerValue, (data, content.pointerVal->copier, content.pointerVal->cleaner));
		}
	}
}

/////////////////////////////////////////////////////////
