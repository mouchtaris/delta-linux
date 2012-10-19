#include "Exporter.h"
#include "HTMLExporter.h"
#include "RTFExporter.h"

class RangeToFormat;
#include "../Scintilla/Include/Platform.h"
#include "../Scintilla/Include/Scintilla.h"
#include "../Scintilla/Src/XPM.h"
#include "../scintilla/src/ContractionState.h"
#include "../scintilla/src/SVector.h"
#include "../scintilla/src/SplitVector.h"
#include "../scintilla/src/Partitioning.h"
#include "../scintilla/src/CellBuffer.h"
#include "../scintilla/src/CallTip.h"
#include "../scintilla/src/KeyMap.h"
#include "../scintilla/src/Indicator.h"
#include "../scintilla/src/LineMarker.h"
#include "../scintilla/src/Style.h"
#include "../scintilla/src/ViewStyle.h"
#include "../scintilla/src/AutoComplete.h"
#include "../scintilla/src/CharClassify.h"
#include "../scintilla/src/RESearch.h"
#include "../scintilla/src/Document.h"
#include "../scintilla/src/Editor.h"

#include <wx/textbuf.h>

static void err(const TCHAR * msg) { MessageBox(GetActiveWindow(), msg, TEXT("Error"), MB_OK); }

static void initScintillaData(CurrentScintillaData * csd) {
	csd->styles = new StyleData[NRSTYLES];
	csd->dataBuffer = NULL;
}

static void deinitScintillaData(CurrentScintillaData * csd) {
	delete [] csd->styles;
	if (csd->dataBuffer)
		delete [] csd->dataBuffer;
}

Exporter::Exporter(wxScintilla* scintilla) : _clipboardId(0) {
	initScintillaData(&mainCSD);
	mainCSD.hScintilla = scintilla;
}

Exporter::~Exporter(void) { deinitScintillaData(&mainCSD); }

void Exporter::doClipboardAll(const SelectionText& st) {
	fillScintillaData(&mainCSD, 0, 0);
	
	BOOL result = OpenClipboard(GetActiveWindow());
	if (result == FALSE) {
		err(TEXT("Unable to open clipboard"));
		return;
	}

	result = EmptyClipboard();
	if (result == FALSE) {
		err(TEXT("Unable to empty clipboard"));
		return;
	}

	exportCustom(RTFExporter());
	exportCustom(HTMLExporter());
	exportTXT(st);
	exportRectTXT(st);

	result = CloseClipboard();
	if (result == FALSE) {
		err(TEXT("Unable to close clipboard"));
		return;
	}
}

//Export handlers
void Exporter::exportCustom(const Exporter& exporter) {
	ExportData ed;
	ed.csd = &mainCSD;
	exporter.exportData(&ed);
	if (ed.hBuffer) {
		HANDLE clipHandle = SetClipboardData(exporter.getClipboardID(), ed.hBuffer);
		if (!clipHandle) {
			GlobalFree(ed.hBuffer);
			err(TEXT("Failed setting clipboard data"));
		}
	}
}

void Exporter::exportTXT(const SelectionText& st) {
	wxString text = wxTextBuffer::Translate (sci2wx(st.s, st.len-1));
	HGLOBAL hTXTBuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (text.size() + 1) * sizeof(wxChar));
	wxChar* buffer = (wxChar *)GlobalLock(hTXTBuffer);
	wxStrcpy(buffer, text.c_str());
	GlobalUnlock(hTXTBuffer);
#ifdef UNICODE
#define CF_MY_TEXT	CF_UNICODETEXT
#else
#define CF_MY_TEXT	CF_UNICODETEXT
#endif
	if (!SetClipboardData(CF_MY_TEXT, buffer)) {
		GlobalFree(buffer);
		err(TEXT("Failed setting clipboard data"));
	}
}

void Exporter::exportRectTXT(const SelectionText& st) {
	HGLOBAL hTXTBuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, st.len+1);
	char * buffer = (char *)GlobalLock(hTXTBuffer);
	buffer[0] = (st.rectangular)? (char)1 : (char)0;
    memcpy (buffer+1, st.s, st.len);
	GlobalUnlock(hTXTBuffer);

	if (!SetClipboardData(RegisterClipboardFormat(CF_RECT_TEXT), buffer)) {
		GlobalFree(buffer);
		err(TEXT("Failed setting clipboard data"));
	}
}

// Private funcs

void Exporter::fillScintillaData(CurrentScintillaData * csd, int start, int end) {
	bool doColourise = true;

	if (csd->dataBuffer) {
		delete [] csd->dataBuffer;
		csd->dataBuffer = 0;
	}
	
	wxScintilla* sci = csd->hScintilla;
	
	if (end == 0 && start == 0) {
		int selStart = 0, selEnd = 0;
		selStart = (int)sci->SendMsg(SCI_GETSELECTIONSTART, 0, 0);
		selEnd = (int)sci->SendMsg(SCI_GETSELECTIONEND, 0, 0);
		if (selStart != selEnd) {
			start = selStart;
			end = selEnd;
			doColourise = false;	//do not colourise on selection, scintilla should have done this by now. Colourise on selection and the state of the lexer doesnt always match
		} else {
			end = -1;
		}
	}

	if (end == -1) {
		end = (int) sci->SendMsg(SCI_GETTEXTLENGTH, 0, 0);
	}

	int len = end - start;
	int tabSize = (int) sci->SendMsg(SCI_GETTABWIDTH, 0, 0);
	int codePage = (int) sci->SendMsg(SCI_GETCODEPAGE, 0, 0);

	//csd->hScintilla = hScintilla;
	csd->isRectText = sci->SendMsg(SCI_GETSELECTIONMODE) == SC_SEL_RECTANGLE;
	csd->nrChars = len;
	csd->tabSize = tabSize;
	csd->currentCodePage = codePage;

	csd->dataBuffer = new char[csd->nrChars * 2 + 2];

	TextRange tr;
	tr.lpstrText = csd->dataBuffer;
	tr.chrg.cpMin = start;
	tr.chrg.cpMax = end;

	if (doColourise)
		sci->SendMsg(SCI_COLOURISE, start, (LPARAM)end);	//colourise doc so stylers are set
	sci->SendMsg(SCI_GETSTYLEDTEXT, 0, (LPARAM)&tr);

	csd->nrStyleSwitches = 0, csd->nrUsedStyles = 1;	//Default always
	csd->totalFontStringLength = 0;
	int prevStyle = -1, currentStyle;

	//Mask the styles so any indicators get ignored, else overflow possible
	int bits = (int)sci->SendMsg(SCI_GETSTYLEBITS, 0, 0);
	unsigned char mask = 0xFF >> (8-bits);
	for(int i = 0; i < len; i++) {
		csd->dataBuffer[i*2+1] &= mask;
	}

	for(int i = 0; i < NRSTYLES; i++) {
		csd->usedStyles[i] = false;
	}

	csd->usedStyles[STYLE_DEFAULT] = true;
	sci->SendMsg(SCI_STYLEGETFONT, STYLE_DEFAULT, (LPARAM) (csd->styles[STYLE_DEFAULT].fontString));
	csd->totalFontStringLength += (int)strlen((csd->styles[STYLE_DEFAULT].fontString));
	csd->styles[STYLE_DEFAULT].fontIndex = 0;
	csd->styles[STYLE_DEFAULT].size =		(int)sci->SendMsg(SCI_STYLEGETSIZE,		STYLE_DEFAULT, 0);
	csd->styles[STYLE_DEFAULT].bold =		(int)sci->SendMsg(SCI_STYLEGETBOLD,		STYLE_DEFAULT, 0);
	csd->styles[STYLE_DEFAULT].italic =		(int)sci->SendMsg(SCI_STYLEGETITALIC,	STYLE_DEFAULT, 0);
	csd->styles[STYLE_DEFAULT].underlined =	(int)sci->SendMsg(SCI_STYLEGETUNDERLINE, STYLE_DEFAULT, 0);
	csd->styles[STYLE_DEFAULT].fgColor =	(int)sci->SendMsg(SCI_STYLEGETFORE,		STYLE_DEFAULT, 0);
	csd->styles[STYLE_DEFAULT].bgColor =	(int)sci->SendMsg(SCI_STYLEGETBACK,		STYLE_DEFAULT, 0);
	csd->styles[STYLE_DEFAULT].eolExtend =(bool)(sci->SendMsg(SCI_STYLEGETEOLFILLED,	STYLE_DEFAULT, 0) != 0);
	for(int i = 0; i < len; i++) {
		currentStyle = csd->dataBuffer[i*2+1];
		if (currentStyle != prevStyle) {
			prevStyle = currentStyle;
			csd->nrStyleSwitches++;
		}
		if (csd->usedStyles[currentStyle] == false) {
			csd->nrUsedStyles++;
			sci->SendMsg(SCI_STYLEGETFONT, currentStyle, (LPARAM) (csd->styles[currentStyle].fontString));
			csd->totalFontStringLength += (int)strlen((csd->styles[currentStyle].fontString));
			csd->styles[currentStyle].fontIndex = 0;
			csd->styles[currentStyle].size =		(int)sci->SendMsg(SCI_STYLEGETSIZE,		currentStyle, 0);
			csd->styles[currentStyle].bold =		(int)sci->SendMsg(SCI_STYLEGETBOLD,		currentStyle, 0);
			csd->styles[currentStyle].italic =		(int)sci->SendMsg(SCI_STYLEGETITALIC,	currentStyle, 0);
			csd->styles[currentStyle].underlined =	(int)sci->SendMsg(SCI_STYLEGETUNDERLINE, currentStyle, 0);
			csd->styles[currentStyle].fgColor =		(int)sci->SendMsg(SCI_STYLEGETFORE,		currentStyle, 0);
			csd->styles[currentStyle].bgColor =		(int)sci->SendMsg(SCI_STYLEGETBACK,		currentStyle, 0);
			csd->styles[currentStyle].eolExtend = (bool)(sci->SendMsg(SCI_STYLEGETEOLFILLED,	currentStyle, 0) != 0);
			csd->usedStyles[currentStyle] = true;
		}
	}

	//For retrieving fontsize for tabs
	HWND hScintilla = (HWND) sci->GetHWND();
	HDC scintDC = GetDC(hScintilla);
	
	//font magic
	int pplix = GetDeviceCaps(scintDC, LOGPIXELSX);
	int ppliy = GetDeviceCaps(scintDC, LOGPIXELSY);
	int nHeight = -MulDiv(csd->styles[STYLE_DEFAULT].size, ppliy, 72);

#ifdef UNICODE

	TCHAR uniFont[32];
	MultiByteToWideChar(CP_ACP, 0, csd->styles[STYLE_DEFAULT].fontString, -1, uniFont, 32);

	HFONT font = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
							OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, 
							uniFont);

#else

	HFONT font = CreateFont(nHeight, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
							OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, 
							csd->styles[STYLE_DEFAULT].fontString);

#endif


	HGDIOBJ old = SelectObject(scintDC, font);

	SIZE size;
	size.cx = 8;	//fallback, 8 pix default
	GetTextExtentPoint32(scintDC, TEXT(" "), 1, &size);
	int twips = size.cx * (1440 / pplix);

	SelectObject(scintDC, old);
	DeleteObject(font);
	ReleaseDC(hScintilla, scintDC);

	//We now have the amount of twips per space in the default font
	csd->twipsPerSpace = twips;
}