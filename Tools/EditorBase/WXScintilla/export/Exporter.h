#ifndef EXPORTER_H
#define EXPORTER_H

#include "ExportStructs.h"

#define CF_RECT_TEXT	TEXT("application/x-cbrectdata")

class SelectionText;

class Exporter
{
public:
	void doClipboardAll(const SelectionText& st);

	Exporter(wxScintilla* scintilla = 0);
	~Exporter(void);

	virtual bool exportData(ExportData * ed) const { return false; }
	virtual TCHAR * getClipboardType() const { return NULL; }

	int getClipboardID() const { return _clipboardId; }
protected:
	UINT _clipboardId;
	void setClipboardID(UINT id) { _clipboardId = id; }
private:
	void fillScintillaData(CurrentScintillaData * csd, int start, int end);
	void exportCustom(const Exporter& exporter);
	void exportTXT(const SelectionText& st);
	void exportRectTXT(const SelectionText& st);

	CurrentScintillaData mainCSD;
};

#endif