#ifndef	XMLLOADERAPI_H
#define	XMLLOADERAPI_H
#include <string>
#include <map>
#include <list>
#include "DeltaObject.h"

enum XmlWriteIndexType
{
	ErrorIndex			= 0,
	UndefinedIndex		= 1,
	NumericIndex		= 2,
	StringIndex			= 3
};

class XmlLoaderAPI
{
private:
	XmlLoaderAPI(void);
	~XmlLoaderAPI(void);

	static std::string*		currFile;
	static util_ui32		currLine;
	static bool				outertag;
	static std::map<
		util_ui32,
		std::list<util_ui32>
	>						serialNums;
	static					XmlLoaderAPI* singletonPtr;
	static DeltaObject*	Parse (void (*cleanUpScanner)(void));
	static void				CleanUpScannerFromFile (void);
	static void				CleanUpScannerFromString (void);
	static bool				Write (
									FILE*				fp,
									DeltaTable*			table,
									util_ui32			tabs,
									const std::string&	indexing
							);
	static XmlWriteIndexType CheckTable (
		FILE* fp,
		DeltaTable* table,
		DeltaTable* indices
		);
	static void				WriteAttributes (
		FILE* fp,
		DeltaTable* table,
		const std::string& indexing,
		DeltaTable* indices
	);
	static void				WriteTabs (
		FILE* fp,
		DeltaTable* table,
		util_ui32 tabs,
		const std::string& indexing,
		DeltaTable* indices
	);
	static bool				CheckStringIndex(std::string);
	static bool				FindCircles(util_ui32, util_ui32);
public:
	static void					SetLine (util_ui32 line);
	static util_ui32			GetLine (void);
	static const std::string&	GetFile (void);
	static void					SetFile (const std::string& file);

	static void				SingletonCreate (void) { singletonPtr = new XmlLoaderAPI; }
	static void				SingletonDestroy (void) { delete singletonPtr; singletonPtr = 0; }
	static DeltaObject*	Load (const std::string& path);
	static DeltaObject*	LoadFromString (const std::string& buffer);
	static bool				Write (const char* file, DeltaTable* table);
};


#endif
