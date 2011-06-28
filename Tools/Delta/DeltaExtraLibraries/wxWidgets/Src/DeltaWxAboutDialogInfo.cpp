#include "DeltaWxAboutDialogInfo.h"
#include "DeltaWxIcon.h"
#include "DDebug.h"
#include "DeltaVirtualMachine.h"
#include "DeltaTable.h"
#include "DeltaLibFuncBinder.h"
#include "DeltaStdLibTemplates.h"
#include "DeltaLibraryObjectCreator.h"
#include "udynamiclibloader.h"
#include "wxWrapperUtilFunctions.h"
//

////////////////////////////////////////////////////////////////

#define WX_FUNC_DEF(name) WX_FUNC_DEF1(aboutdialoginfo, name)
#define WX_FUNC(name) WX_FUNC1(aboutdialoginfo, name)

WX_FUNC_DEF(construct)
WX_FUNC_DEF(destruct)
WX_FUNC_DEF(addartist)
WX_FUNC_DEF(adddeveloper)
WX_FUNC_DEF(adddocwriter)
WX_FUNC_DEF(addtranslator)
WX_FUNC_DEF(setartists)
WX_FUNC_DEF(setcopyright)
WX_FUNC_DEF(setdescription)
WX_FUNC_DEF(setdevelopers)
WX_FUNC_DEF(setdocwriters)
WX_FUNC_DEF(seticon)
WX_FUNC_DEF(setlicence)
WX_FUNC_DEF(setlicense)
WX_FUNC_DEF(setname)
WX_FUNC_DEF(settranslators)
WX_FUNC_DEF(setversion)
WX_FUNC_DEF(setwebsite)

WX_FUNCS_START
	WX_FUNC(construct),
	WX_FUNC(destruct),
	WX_FUNC(addartist),
	WX_FUNC(adddeveloper),
	WX_FUNC(adddocwriter),
	WX_FUNC(addtranslator),
	WX_FUNC(setartists),
	WX_FUNC(setcopyright),
	WX_FUNC(setdescription),
	WX_FUNC(setdevelopers),
	WX_FUNC(setdocwriters),
	WX_FUNC(seticon),
	WX_FUNC(setlicence),
	WX_FUNC(setlicense),
	WX_FUNC(setname),
	WX_FUNC(settranslators),
	WX_FUNC(setversion),
	WX_FUNC(setwebsite)
WX_FUNCS_END

////////////////////////////////////////////////////////////////

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "destruct", "setwebsite")

DLIB_WX_TOEXTERNID_AND_INSTALLALL_FUNCS_BASE(AboutDialogInfo, "aboutdialoginfo")

////////////////////////////////////////////////////////////////

static bool GetKeys (void* val, DeltaValue* at) 
{
	return DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo) ?
		DPTR(getter)->GetAllKeys(val, at) : false ;
}

static bool GetName (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	WX_SETSTRING_EX(*at, info->GetName());
	return true;
}

static bool GetVersion (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	WX_SETSTRING_EX(*at, info->GetVersion());
	return true;
}

static bool GetDescription (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	WX_SETSTRING_EX(*at, info->GetDescription());
	return true;
}

static bool GetCopyright (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	WX_SETSTRING_EX(*at, info->GetCopyright());
	return true;
}

static bool GetLicence (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	WX_SETSTRING_EX(*at, info->GetLicence());
	return true;
}

static bool GetIcon (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	DeltaWxIcon *retval = DNEWCLASS(DeltaWxIcon, (new wxIcon(info->GetIcon())));
	WX_SETOBJECT_EX(*at, Icon, retval);
	return true;
}

static bool GetWebSiteURL (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	WX_SETSTRING_EX(*at, info->GetWebSiteURL());
	return true;
}

static bool GetWebSiteDescription (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	WX_SETSTRING_EX(*at, info->GetWebSiteDescription());
	return true;
}

static bool GetDevelopers (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	wxArrayString strarray = info->GetDevelopers();
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, num = (int)strarray.GetCount(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		WX_SETSTRING_EX(content, strarray[i])
		at->ToTable()->Set(index, content);
	}
	return true;
}

static bool GetDocWriters (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	wxArrayString strarray = info->GetDocWriters();
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, num = (int)strarray.GetCount(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		WX_SETSTRING_EX(content, strarray[i])
		at->ToTable()->Set(index, content);
	}
	return true;
}

static bool GetArtists (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	wxArrayString strarray = info->GetArtists();
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, num = (int)strarray.GetCount(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		WX_SETSTRING_EX(content, strarray[i])
		at->ToTable()->Set(index, content);
	}
	return true;
}

static bool GetTranslators (void* val, DeltaValue* at) 
{
	wxAboutDialogInfo *info = DLIB_WXTYPECAST_BASE(AboutDialogInfo, val, aboutdialoginfo);
	wxArrayString strarray = info->GetTranslators();
	at->FromTable(DNEW(DELTA_OBJECT));
	for (int i = 0, num = (int)strarray.GetCount(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), content;
		WX_SETSTRING_EX(content, strarray[i])
		at->ToTable()->Set(index, content);
	}
	return true;
}

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",				&GetKeys,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "name",				&GetName,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "version",			&GetVersion,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "description",		&GetDescription,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "copyright",			&GetCopyright,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "licence",			&GetLicence,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "icon",				&GetIcon,				DELTA_GETBYSTRING_NO_PRECOND	},
	{ "webSiteURL",			&GetWebSiteURL,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "webSiteDescription",	&GetWebSiteDescription,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "developers",			&GetDevelopers,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "docWriters",			&GetDocWriters,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "artists",			&GetArtists,			DELTA_GETBYSTRING_NO_PRECOND	},
	{ "translators",		&GetTranslators,		DELTA_GETBYSTRING_NO_PRECOND	}
};

WX_LIBRARY_FUNCS_IMPLEMENTATION(AboutDialogInfo, aboutdialoginfo);

////////////////////////////////////////////////////////////////

DLIB_FUNC_START(aboutdialoginfo_construct, 0, Nil)
	DeltaWxAboutDialogInfo *info = DNEWCLASS(DeltaWxAboutDialogInfo, (new wxAboutDialogInfo()));
	WX_SETOBJECT(AboutDialogInfo, info)
}

DLIB_FUNC_START(aboutdialoginfo_destruct, 1, Nil)
	DLIB_WXDELETE(aboutdialoginfo, AboutDialogInfo, info)
}

DLIB_FUNC_START(aboutdialoginfo_addartist, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETSTRING(artist)
	info->AddArtist(artist);
}

DLIB_FUNC_START(aboutdialoginfo_adddeveloper, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETSTRING(developer)
	info->AddDeveloper(developer);
}

DLIB_FUNC_START(aboutdialoginfo_adddocwriter, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETSTRING(docwriter)
	info->AddDocWriter(docwriter);
}

DLIB_FUNC_START(aboutdialoginfo_addtranslator, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETSTRING(translator)
	info->AddTranslator(translator);
}

DLIB_FUNC_START(aboutdialoginfo_setartists, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETTABLE(artists)
	wxArrayString strarray;
	for (int i = 0, num = artists->Total(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), value;
		artists->Get(index, &value);
		if (value.Type() == DeltaValue_String) {
			strarray.Add(wxString(value.ToString().c_str(), wxConvUTF8));
		}
	}
	info->SetArtists(strarray);
}

DLIB_FUNC_START(aboutdialoginfo_setcopyright, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETSTRING(copyright)
	info->SetCopyright(copyright);
}

DLIB_FUNC_START(aboutdialoginfo_setdescription, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETSTRING(desc)
	info->SetDescription(desc);
}

DLIB_FUNC_START(aboutdialoginfo_setdevelopers, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETTABLE(developers)
	wxArrayString strarray;
	for (int i = 0, num = developers->Total(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), value;
		developers->Get(index, &value);
		if (value.Type() == DeltaValue_String) {
			strarray.Add(wxString(value.ToString().c_str(), wxConvUTF8));
		}
	}
	info->SetDevelopers(strarray);
}

DLIB_FUNC_START(aboutdialoginfo_setdocwriters, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETTABLE(docwriters)
	wxArrayString strarray;
	for (int i = 0, num = docwriters->Total(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), value;
		docwriters->Get(index, &value);
		if (value.Type() == DeltaValue_String) {
			strarray.Add(wxString(value.ToString().c_str(), wxConvUTF8));
		}
	}
	info->SetDocWriters(strarray);
}

DLIB_FUNC_START(aboutdialoginfo_seticon, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	DLIB_WXGET_BASE(icon, Icon, icon)
	info->SetIcon(*icon);
}

DLIB_FUNC_START(aboutdialoginfo_setlicence, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETSTRING(licence)
	info->SetLicence(licence);
}

DLIB_FUNC_START(aboutdialoginfo_setlicense, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETSTRING(licence)
	info->SetLicence(licence);
}

DLIB_FUNC_START(aboutdialoginfo_setname, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETSTRING(name)
	info->SetName(name);
}

DLIB_FUNC_START(aboutdialoginfo_settranslators, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETTABLE(translators)
	wxArrayString strarray;
	for (int i = 0, num = translators->Total(); i < num; ++i) {
		DeltaValue index((DeltaNumberValueType)i), value;
		translators->Get(index, &value);
		if (value.Type() == DeltaValue_String) {
			strarray.Add(wxString(value.ToString().c_str(), wxConvUTF8));
		}
	}
	info->SetTranslators(strarray);
}

DLIB_FUNC_START(aboutdialoginfo_setversion, 2, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETSTRING(version)
	info->SetVersion(version);
}

WX_FUNC_ARGRANGE_START(aboutdialoginfo_setwebsite, 2, 3, Nil)
	DLIB_WXGET_BASE(aboutdialoginfo, AboutDialogInfo, info)
	WX_GETSTRING(url)
	wxString desc = wxEmptyString;
	if (n >= 3) { WX_GETSTRING_DEFINED(desc) }
	info->SetWebSite(url, desc);
}
