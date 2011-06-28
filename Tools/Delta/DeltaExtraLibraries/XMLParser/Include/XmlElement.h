#ifndef	XMLELEMENT_H
#define	XMLELEMENT_H

#include <string>
#include "DeltaObject.h"

class XmlElement {
public:
	typedef	std::string		Name;
	typedef DeltaObject		Content;


	void			SetName (Name* name)
						{ elementName = name; }
	Name*			GetName (void) const
						{ return elementName; }

	void			SetContent (Content* _content)
						{ content = _content; }
	Content*		GetContent (void) const
						{ return content; }

	XmlElement (void)	{ }
	~XmlElement (void)	{ }

private:
	Content*		content;
	Name*			elementName;
};

#endif