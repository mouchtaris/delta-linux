#ifndef TXT_EXPORTER_H
#define TXT_EXPORTER_H

#include "Exporter.h"

//size definitions for memory allocation
#define EXPORT_SIZE_TXT_STATIC		0
#define EXPORT_SIZE_TXT_STYLE		0
#define EXPORT_SIZE_TXT_SWITCH		0

class TXTExporter :
	public Exporter
{
public:
	TXTExporter(void);
	~TXTExporter(void);
	bool exportData(ExportData * ed);
	TCHAR * getClipboardType();
};

#endif