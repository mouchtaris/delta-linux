#ifndef _WX_PROPGRID_EXTRAPROPS_H_
#define _WX_PROPGRID_EXTRAPROPS_H_

#include "propgrid.h"
#include "propdev.h"
#include "advprops.h"

WX_PG_DECLARE_ARRAYSTRING_PROPERTY_WITH_DECL(wxFileListProperty,WXDLLIMPEXP_PG)
WX_PG_DECLARE_ARRAYSTRING_PROPERTY_WITH_DECL(wxDirectoryListProperty,WXDLLIMPEXP_PG)

extern WXDLLIMPEXP_PG wxPGProperty* wxPG_CONSTFUNC(wxGenericListProperty)(
	const wxString& label, const wxString& name, const wxArrayString& value, wxParentPropertyClass *valueCreator);

#endif // _WX_PROPGRID_EXTRAPROPS_H_