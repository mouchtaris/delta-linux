/**
 *	InternalDataObject.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef GUI_INTERNALDATAOBJECT_H
#define GUI_INTERNALDATAOBJECT_H

#include "Common.h"
#include "LibraryAPIDecls.h"

#include <wx/dataobj.h>

namespace gui {

////////////////////////////////////////////////////////////////////////

class _BASE_API InternalDataObject : public wxDataObjectSimple {
public:
	InternalDataObject (const tchar* format, const std::string& data = std::string());
	~InternalDataObject (void);

	void				SetData (const std::string& data) { m_data = data; }
	const std::string&	GetData (void) const { return m_data; }

	virtual size_t	GetDataSize (void) const;
	virtual bool	SetData (size_t len, const void* buffer);
	virtual bool	GetDataHere (void* buffer) const;

private:
	std::string m_data;
};

////////////////////////////////////////////////////////////////////////

} // namespace gui

#endif // GUI_INTERNALDATAOBJECT_H
