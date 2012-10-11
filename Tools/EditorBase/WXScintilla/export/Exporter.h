#ifndef EXPORTER_H
#define EXPORTER_H

#include "ExportStructs.h"

#define CF_RECT_TEXT	TEXT("application/x-cbrectdata")

class SelectionText;

class Exporter
{
public:
	void doExportRTF(TCHAR* filename);
	void doExportHTML(TCHAR* filename);

	void doClipboardRTF();
	void doClipboardHTML();
	void doClipboardAll(const SelectionText& st);

	Exporter(wxScintilla* scintilla = 0);
	~Exporter(void);

	virtual bool exportData(ExportData * ed) { return false; }
	virtual TCHAR * getClipboardType() { return NULL; }

	int getClipboardID() { return _clipboardId; }
protected:
	UINT _clipboardId;

	void setClipboardID(UINT id) {
		_clipboardId = id;
	}
private:
	BOOL saveFile(TCHAR * filebuffer, int buffersize, const TCHAR * filters);
	void fillScintillaData(CurrentScintillaData * csd, int start, int end);
	void exportHTML(bool isClipboard, HANDLE exportFile);
	void exportRTF(bool isClipboard, HANDLE exportFile);
	void exportTXT(bool isClipboard, HANDLE exportFile);
	void exportRectTXT(const SelectionText& st);

	CurrentScintillaData mainCSD;
};

#endif