/**
 *	TreeItemDataObject.h
 *
 *	-- IDE Data object type for the drag&drop of the Project Manager tree --
 *
 *	Support for dropping items in the Project Manager treeview.
 *	Items supported are external files and editor handles.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	April 2009
 */
#ifndef IDE_TREE_ITEM_DATA_OBJECT_H_
#define IDE_TREE_ITEM_DATA_OBJECT_H_

#include "Common.h"
#include "ComponentHandle.h"
#include "InternalDataObject.h"
#include <wx/wx.h>
#include <wx/dataobj.h>

namespace ide
{

	class TreeItemDataObject : public wxDataObject {
	public:
		///--- type definitions

		///--- constructors / destructor
		TreeItemDataObject(void);
		~TreeItemDataObject();

		///--- public API
		bool					HasHandle	(void) const;

		const Handle			GetHandle	(void) const;
		const wxArrayString&	GetFilenames(void) const;

		virtual wxDataFormat	GetPreferredFormat	(Direction dir)							const;
		virtual size_t			GetFormatCount		(Direction dir)							const;
		virtual void			GetAllFormats		(wxDataFormat *formats, Direction dir)	const;

		virtual size_t	GetDataSize	(const wxDataFormat& format)								const;
		virtual bool	GetDataHere	(const wxDataFormat& format, void *pBuf)					const;
		virtual bool	SetData		(const wxDataFormat& format, size_t len, const void *buf);

	private:
		///--- private API

		///--- private members
		wxFileDataObject		fileObject;
		gui::InternalDataObject	handleObject;
		bool					hasHandle;
	};
}

#endif	//IDE_TREE_ITEM_DATA_OBJECT_H_