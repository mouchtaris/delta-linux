#ifndef EXPORT_STRUCTS_H
#define EXPORT_STRUCTS_H

#ifdef _MSC_VER
#include <windows.h>
#else
#error "Exporting functionality implemented only on windows"
#endif

#include "scintilla.h"
#include "../wxscintilla.h"

#define SCINTILLA_FONTNAME_SIZE	(32+1)
#define NRSTYLES	(STYLE_MAX+1)

struct StyleData {
	char fontString[32];
	int fontIndex;
	int size;
	int bold;
	int italic;
	int underlined;
	int fgColor;
	int bgColor;
	int fgClrIndex;
	int bgClrIndex;
	bool eolExtend;
};

struct CurrentScintillaData {
	wxScintilla* hScintilla;
	long nrChars;
	int tabSize;
	bool usedStyles[NRSTYLES];
	StyleData * styles;
	char * dataBuffer;
	int nrUsedStyles;
	int nrStyleSwitches;
	int totalFontStringLength;
	int currentCodePage;
	int twipsPerSpace;
	bool isRectText;
};

struct ExportData {
	bool isClipboard;
	CurrentScintillaData * csd;
	HGLOBAL hBuffer;
	unsigned long bufferSize;
};

#endif