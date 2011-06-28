/**
 *	InternalDataObject.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "InternalDataObject.h"
#include "Streams.h"

#include <string.h>

namespace gui {

////////////////////////////////////////////////////////////////////////
// InternalDataObject class
//

InternalDataObject::InternalDataObject (const tchar* format, const std::string& data) :
	wxDataObjectSimple(wxDataFormat(format)), m_data(data)
{

}

//**********************************************************************

InternalDataObject::~InternalDataObject (void)
{

}

//**********************************************************************

size_t InternalDataObject::GetDataSize (void) const
{
	return m_data.size();
}

//**********************************************************************

bool InternalDataObject::SetData (size_t len, const void* buffer)
{
	m_data = std::string(static_cast<const char*>(buffer), len);
	return true;
}

//**********************************************************************

bool InternalDataObject::GetDataHere (void* buffer) const
{
	std::memcpy(buffer, m_data.c_str(), m_data.size());
	return true;
}

////////////////////////////////////////////////////////////////////////

} // namespace gui
