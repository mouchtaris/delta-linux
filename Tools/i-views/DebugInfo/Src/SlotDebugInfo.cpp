#include "SlotDebugInfo.h"
#include <assert.h>

namespace iviews {

namespace dinfo{

SlotDebugInfo::SlotDebugInfo(
	const id_t				_id,
	const std::string &		_stringId,
	const bool				_isKeyAggregate,
	const std::string &		_keyString,
	const std::string &		_keyTypeInfoString,
	const graphs::Vertex *	_keyTargetVertex,
	const bool				_isContentAggergate,
	const std::string &		_contentString,
	const graphs::Vertex *	_contentTargetVertex
) :	id(_id),
	stringId(_stringId),
	isKeyAggregate(_isKeyAggregate),
	keyString(_keyString),
	keyTypeInfoString(_keyTypeInfoString),
	keyTargetVertex(const_cast<graphs::Vertex *>(_keyTargetVertex)),
	isContentAggergate(_isContentAggergate),
	contentString(_contentString),
	contentTargetVertex(const_cast<graphs::Vertex *>(_contentTargetVertex))

	{ keyTypeInfo = StringToKeyTypeInformation(keyTypeInfoString);	}

SlotDebugInfo::SlotDebugInfo(const SlotDebugInfo & dic) 
:	id(dic.id),
	stringId(dic.stringId),
	isKeyAggregate(dic.isKeyAggregate),
	keyString(dic.keyString),
	keyTypeInfo(dic.keyTypeInfo),
	keyTypeInfoString(dic.keyTypeInfoString),
	keyTargetVertex(dic.keyTargetVertex),
	isContentAggergate(dic.isContentAggergate),
	contentString(dic.contentString),
	contentTargetVertex(dic.contentTargetVertex)
{}


SlotDebugInfo::SlotDebugInfo() :
	id(0),
	stringId(""),
	isKeyAggregate(false),
	keyString(""),
	keyTypeInfo(NONE_PSEUDO_KEY),
	keyTypeInfoString(""),
	keyTargetVertex(0),
	isContentAggergate(false),
	contentString(""),
	contentTargetVertex(0){}


///---------- Method(s)
id_t SlotDebugInfo::Id() const 
	{ return id; }

//--------------------------------------------------------

void SlotDebugInfo::SetId(const id_t & _id)
	{ id = _id; }

//--------------------------------------------------------

const std::string &	SlotDebugInfo::GetIdRelevantToDebugInfo(void) const
	{ return stringId; }

//--------------------------------------------------------

void SlotDebugInfo::SetIdRelevantToDebugInfo(const std::string & id)
	{ stringId = id; }

//--------------------------------------------------------

bool SlotDebugInfo::IsKeyAggregate(void) const					
	{ return isKeyAggregate; }

//--------------------------------------------------------

void SlotDebugInfo::KeyAggregate(const bool b)
	{ isKeyAggregate = b; }

//--------------------------------------------------------
std::string	SlotDebugInfo::GetKeyString(void) const					
	{ return keyString; }

//--------------------------------------------------------

void SlotDebugInfo::SetKeyString(const std::string & keyStr)	
	{ keyString = keyStr; }

//--------------------------------------------------------
KeyTypeInformation	SlotDebugInfo::GetKeyTypeInfo(void) const					
	{ return keyTypeInfo; }

//--------------------------------------------------------

void SlotDebugInfo::SetKeyTypeInfo(const KeyTypeInformation _keyTypeInfo){
	keyTypeInfo			= _keyTypeInfo;
	keyTypeInfoString	= KeyTypeInformationToString(_keyTypeInfo);
}

//--------------------------------------------------------

std::string	SlotDebugInfo::GetStringKeyTypeInfo(void) const			
	{ return keyTypeInfoString; }

//--------------------------------------------------------

void SlotDebugInfo::SetStringKeyTypeInfo(const std::string & _keyTypeInfo) {
	keyTypeInfo			= StringToKeyTypeInformation(_keyTypeInfo);
	keyTypeInfoString	= keyTypeInfo;
}

//--------------------------------------------------------
graphs::Vertex * SlotDebugInfo::GetKeyTargetVertex(void) const				
	{ return keyTargetVertex; }

//--------------------------------------------------------

void SlotDebugInfo::SetKeyTargetVertex(graphs::Vertex* v)		
	{ keyTargetVertex = v;}

//--------------------------------------------------------

bool SlotDebugInfo::IsContentAggregate(void) const				
	{ return isContentAggergate; }

void SlotDebugInfo::ContentAggregate(const bool b)
	{ isContentAggergate = b; }

//--------------------------------------------------------

std::string	SlotDebugInfo::GetContentString(void) const				
	{ return contentString; }

//--------------------------------------------------------

void SlotDebugInfo::SetContentString(const std::string & conStr)
	{ contentString = conStr; }

//--------------------------------------------------------
graphs::Vertex * SlotDebugInfo::GetContentTargetVertex(void) const			
	{ return contentTargetVertex; }

//--------------------------------------------------------

void SlotDebugInfo::SetContentTargetVertex(graphs::Vertex * v)	
	{ contentTargetVertex = v; }

//--------------------------------------------------------

std::string	SlotDebugInfo::KeyTypeInformationToString(const KeyTypeInformation & info) {
	std::string str;
	switch(keyTypeInfo) {
		case OBJECT_DELEGATION_PSEUDO_KEY: 
			str = "OBJECT_DELEGATION_PSEUDO_KEY";
			break;
		case OBJECT_INHERITANCE_PSEUDO_KEY: 
			str = "OBJECT_INHERITANCE_PSEUDO_KEY";
			break;
		case OBJECT_OPERATOR_KEY: 
			str = "OBJECT_OPERATOR_KEY";
			break;
		case OBJECT_ATTRIBUTE_KEY: 
			str = "OBJECT_ATTRIBUTE_KEY";
			break;
		case EXTERNID_USER_PSEUDO_KEY: 
			str = "EXTERNID_USER_PSEUDO_KEY";
			break;
		case OBJECT_PSEUDO_KEY: 
			str = "OBJECT_PSEUDO_KEY";
			break;
		case AGGREGATE_PSEUDO_KEY: 
			str = "AGGREGATE_PSEUDO_KEY";
			break;
		case EXTERNID_PSEUDO_KEY: 
			str = "EXTERNID_PSEUDO_KEY";
			break;
		case NONE_PSEUDO_KEY: 
			str = "NONE_PSEUDO_KEY";
			break;
		default:
			assert(0);
	}
	return str;
}

//--------------------------------------------------------

KeyTypeInformation SlotDebugInfo::StringToKeyTypeInformation(const std::string & keyTypeInfo) {
	KeyTypeInformation info = NONE_PSEUDO_KEY;

	if (!keyTypeInfo.compare("OBJECT_PSEUDO_KEY"))
		info = OBJECT_PSEUDO_KEY;
	
	else if (!keyTypeInfo.compare("OBJECT_INHERITANCE_PSEUDO_KEY"))
		info = OBJECT_INHERITANCE_PSEUDO_KEY;
	
	else if (!keyTypeInfo.compare("OBJECT_DELEGATION_PSEUDO_KEY"))
		info = OBJECT_DELEGATION_PSEUDO_KEY;
	
	else if(!keyTypeInfo.compare("OBJECT_ATTRIBUTE_KEY"))
		info = OBJECT_ATTRIBUTE_KEY;
	
	else if(!keyTypeInfo.compare("OBJECT_OPERATOR_KEY"))
		info = OBJECT_OPERATOR_KEY;
	
	else if(!keyTypeInfo.compare("EXTERNID_USER_PSEUDO_KEY"))
		info = EXTERNID_USER_PSEUDO_KEY;
	
	else if(!keyTypeInfo.compare("EXTERNID_PSEUDO_KEY"))
		info = EXTERNID_PSEUDO_KEY;
	
	else if(!keyTypeInfo.compare("AGGREGATE_PSEUDO_KEY"))
		info = AGGREGATE_PSEUDO_KEY;
	
	else if (keyTypeInfo.empty())
		info = NONE_PSEUDO_KEY;
	else
		assert(0);

	return info;
}


}	//namespace dinfo
}	//namespace iviews