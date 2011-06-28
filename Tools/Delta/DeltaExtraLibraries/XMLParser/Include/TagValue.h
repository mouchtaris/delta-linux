#ifndef	TAGVALUE_H
#define	TAGVALUE_H

#include <string>
#include <list>

class TagValue {
public:
	typedef	std::string									TagName;
	typedef std::string									AttrKey;
	typedef std::string									AttrValue;
	typedef std::pair<AttrKey*, AttrValue*>				Attribute;
	typedef std::list<Attribute>						AttributeList;


	void SetAttribute (AttrKey* name, AttrValue* attvalue)
		{ attlist.push_front(std::make_pair(name, attvalue)); }
	
	Attribute const GetAttribute (void) {
		Attribute const retValue = attlist.front();
		attlist.pop_front();
		return retValue;
	}

	bool HasAttribute(void) const
		{ return !attlist.empty(); }


	void SetName (TagName* name)
		{ tagName = name; }

	TagName* GetName (void) const
		{ return tagName; }


	TagValue(void) { }
	~TagValue(void) { }


private:
	TagName*				tagName;
	AttributeList			attlist;
};

#endif
