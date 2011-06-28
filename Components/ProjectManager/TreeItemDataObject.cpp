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
#include "TreeItemDataObject.h"
#include "Encoding.h"

namespace ide
{
	//-------------------------------------------------------//
	//---- class TreeItemDataObject -------------------------//

	TreeItemDataObject::TreeItemDataObject(void) :	handleObject(_T("Handle")), hasHandle(false)
	{
	}

	//-----------------------------------------------------------------------

	TreeItemDataObject::~TreeItemDataObject()
	{
	}

	//-----------------------------------------------------------------------

	bool TreeItemDataObject::HasHandle (void) const { return hasHandle; }

	//-----------------------------------------------------------------------

	const Handle TreeItemDataObject::GetHandle (void) const
	{
		assert(HasHandle());
		Handle handle;
		comm::decoder(handleObject.GetData()) >> handle;
		return handle;
	}

	//-----------------------------------------------------------------------

	const wxArrayString& TreeItemDataObject::GetFilenames(void) const
	{
		assert(!HasHandle());
		return fileObject.GetFilenames();
	}

	//-----------------------------------------------------------------------

	wxDataFormat TreeItemDataObject::GetPreferredFormat	(Direction dir) const
	{
		return fileObject.GetFormat();
	}

	//-----------------------------------------------------------------------

	size_t TreeItemDataObject::GetFormatCount(Direction dir) const
	{
		return fileObject.GetFormatCount(dir) + handleObject.GetFormatCount(dir);
	}

	//-----------------------------------------------------------------------

	void TreeItemDataObject::GetAllFormats(wxDataFormat *formats, Direction dir) const
	{
		fileObject.GetAllFormats(formats, dir);
		handleObject.GetAllFormats(formats + fileObject.GetFormatCount(dir), dir);
	}

	//-----------------------------------------------------------------------

	size_t TreeItemDataObject::GetDataSize(const wxDataFormat& format) const
	{
		return format == fileObject.GetFormat() ?
			fileObject.GetDataSize(format) : handleObject.GetDataSize();
	}

	//-----------------------------------------------------------------------

	bool TreeItemDataObject::GetDataHere(const wxDataFormat& format, void *buf) const
	{
		return format == fileObject.GetFormat() ?
			fileObject.GetDataHere(format, buf) : handleObject.GetDataHere(buf);
	}

	//-----------------------------------------------------------------------

	bool TreeItemDataObject::SetData(const wxDataFormat& format, size_t len, const void *buf)
	{
		bool ret;
		if (format == fileObject.GetFormat()) {
			hasHandle = false;
			ret = fileObject.SetData(format, len, buf);
		}
		else {
			hasHandle = true;
			ret = handleObject.SetData(len, buf);
		}
		return ret;
	}

	//-----------------------------------------------------------------------
}