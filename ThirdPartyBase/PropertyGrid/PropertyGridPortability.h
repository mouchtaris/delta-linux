#ifndef PROPERTY_GRID_PORTABILITY
#define PROPERTY_GRID_PORTABILITY

#include <wx/defs.h>

#if !wxUSE_PROPGRID
#include <wx/window.h>
#include <wx/dialog.h>
#include <wx/textctrl.h>
#include <wx/toolbar.h>
#include <wx/stattext.h>

#include "propgrid.h"
#include "propdev.h"
#include "advprops.h"
#include "manager.h"

#define THIRD_PARTY_PROPGRID

//#define DECL WXDLLIMPEXP_PG
#define EXTRA_PROP_DECL WXDLLIMPEXP_PG
#define WX_PG_IMPLEMENT_EMPTY_VALIDATOR(PROP) wxValidator* wxPG_PROPCLASS(PROP)::DoGetValidator() const { return (wxValidator*) NULL; }
#define REMOVE_PROPERTY(x) Remove(wxPGId(x))
#define APPEND_CATEGORY(x) AppendCategory(x)

#define wxPGIdGen(PTR) wxPGId(PTR)
#define WX_PG_ID_IS_OK(ID)			ID.IsOk()
#define WX_PG_ID_GET_NAME(ID)		ID.GetName()
#define wxPGIdToPtr(ID)				ID.GetPropertyPtr()
#define GET_NEXT_SIBLING_PROPERTY	GetNextSibling

#define WX_PARENT_PROPERTY_ADD_CHILD(PARENT, CHILD)	(PARENT)->AddChild(CHILD);

#define GET_VALUE_AS_VARIANT GetValueAsVariant

#define PG_GET_VALUE_AS_STRING(p)	(p)->GetValueAsString(0)

#define PG_CREATE_PROP(PROP) PROP

#define PG_CLASS_INFO const wxPGPropertyClassInfo
#else
#ifdef WXMAKINGLIB_PROPGRID
    #define WXDLLIMPEXP_PG
#elif defined(WXMAKINGDLL_PROPGRID)
    #define WXDLLIMPEXP_PG WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_PG WXIMPORT
#else // not making nor using DLL
    #define WXDLLIMPEXP_PG
#endif

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>

#define wxPGId wxPGProperty*

#define DECL class WXDLLIMPEXP_PG
#define EXTRA_PROP_DECL class THIRD_PARTY_API

#define wxPG_CONSTFUNC(PROP) PROP
#define wxPG_PROPCLASS(PROP) PROP

#define WX_PG_DECLARE_VALIDATOR_METHODS()
#define WX_PG_IMPLEMENT_EMPTY_VALIDATOR(PROP)
#define WX_PG_IMPLEMENT_CLASSINFO(x, y)

#define REMOVE_PROPERTY(x) RemoveProperty(x)
#define APPEND_CATEGORY(x) Append(new wxPropertyCategory(x))

class wxParentPropertyClass: public wxPGProperty {
public:
	wxParentPropertyClass (const wxString& label, const wxString& name = wxString::FromAscii("")) throw():
		wxPGProperty(label, name) {}
	unsigned int GetCount() const { return GetChildCount(); }

	virtual wxVariant ChildChanged(wxVariant& thisValue, int childIndex, wxVariant& childValue) const {
		m_children[childIndex]->SetValue(childValue);
		return childValue;
	}

	virtual wxString ValueToString(wxVariant& value, int argFlags) const {
		wxString text;

		int i;
		int iMax = m_children.GetCount();
		int iMaxMinusOne = iMax-1;

		wxPGProperty* curChild = (wxPGProperty*) m_children.Item(0);

		for (i = 0; i < iMax; i++) {
			wxString s = curChild->ValueToString(curChild->GetValue(), argFlags|wxPG_COMPOSITE_FRAGMENT);
			if (curChild->GetChildCount() || s.Contains(wxT(";")))
				text += wxT("[") + s + wxT("]");
			else
				text += s;

			if (i < iMaxMinusOne) {
				curChild = (wxPGProperty*) m_children.Item(i+1);
				if (curChild->GetChildCount())
					text += wxT(" ");
				else
					text += wxT("; ");
			}
		}

		return text;
	}

	virtual bool StringToValue(wxVariant& variant, const wxString& text, int argFlags) const
	{
		if ( !GetChildCount() )
			return false;

		unsigned int curChild = 0;
		unsigned int iMax = m_children.size();

		bool changed = false;

		wxString token;
		size_t pos = 0;

		const wxChar delimeter = wxS(';');

		size_t tokenStart = 0xFFFFFF;

		wxVariantList temp_list;
		wxVariant list(temp_list);

		int propagatedFlags = argFlags & (wxPG_REPORT_ERROR|wxPG_PROGRAMMATIC_VALUE);

		wxString::const_iterator it = text.begin();
		wxUniChar a;

		if ( it != text.end() )
			a = *it;
		else
			a = 0;

		for ( ;; ) 
		{
			// How many units we iterate string forward at the end of loop?
			// We need to keep track of this or risk going to negative
			// with it-- operation.
			unsigned int strPosIncrement = 1;

			if ( tokenStart != 0xFFFFFF )
			{
				// Token is running
				if ( a == delimeter || a == 0 )
				{
					token = text.substr(tokenStart,pos-tokenStart);
					token.Trim(true);
					size_t len = token.length();

					const wxPGProperty* child = Item(curChild);
					wxVariant variant(child->GetValue());
					wxString childName = child->GetBaseName();
					child->StringToValue(variant, token, propagatedFlags|wxPG_COMPOSITE_FRAGMENT);
					variant.SetName(childName);
					list.Append(variant);
					changed = true;
							
					curChild++;
					if ( curChild >= iMax )
						break;

					tokenStart = 0xFFFFFF;
				}
			}
			else
			{
				// Token is not running
				if ( a != wxS(' ') )
				{
					// Is this a group of tokens?
					if ( a == wxS('[') )
					{
						int depth = 1;

						if ( it != text.end() ) ++it;
						pos++;
						size_t startPos = pos;

						// Group item - find end
						while ( it != text.end() && depth > 0 )
						{
							a = *it;
							++it;
							pos++;

							if ( a == wxS(']') )
								depth--;
							else if ( a == wxS('[') )
								depth++;
						}

						token = text.substr(startPos,pos-startPos-1);

						if ( token.empty() )
							break;

						const wxPGProperty* child = Item(curChild);

						wxVariant oldChildValue = child->GetValue();
						wxVariant variant(oldChildValue);

						if ( (argFlags & wxPG_PROGRAMMATIC_VALUE) ||
							 (!child->HasFlag(wxPG_PROP_DISABLED) &&
							  !child->HasFlag(wxPG_PROP_READONLY)) )
						{
							wxString childName = child->GetBaseName();

							bool stvRes = child->StringToValue( variant, token,
																propagatedFlags );
							if ( stvRes || (variant != oldChildValue) )
							{
								variant.SetName(childName);
								list.Append(variant);

								changed = true;
							}
							else
							{
								// No changes...
							}
						}

						curChild++;
						if ( curChild >= iMax )
							break;

						tokenStart = 0xFFFFFF;
					}
					else
					{
						tokenStart = pos;

						if ( a == delimeter )
							strPosIncrement -= 1;
					}
				}
			}

			if ( a == 0 )
				break;

			it += strPosIncrement;

			if ( it != text.end() )
			{
				a = *it;
			}
			else
			{
				a = 0;
			}

			pos += strPosIncrement;
		}

		if ( changed )
			variant = list;

		return changed;
	}

};

#define wxPGIdGen(PTR) PTR
#define WX_PG_ID_IS_OK(ID)			((ID) != NULL)
#define WX_PG_ID_GET_NAME(ID)		(ID)->GetName()
#define wxPGIdToPtr(ID)				(ID)

#define WX_PARENT_PROPERTY_ADD_CHILD(PARENT, CHILD)	(PARENT)->AddPrivateChild(CHILD);

#define GET_VALUE_AS_VARIANT GetValue

#define PG_GET_VALUE_AS_STRING(p)	(p)->ValueToString((p)->GetValue(), 0)

#define PG_CREATE_PROP(PROP) new PROP

#define PG_CLASS_INFO wxClassInfo

typedef wxPropertyGridInterface wxPropertyContainerMethods;
#endif

#endif