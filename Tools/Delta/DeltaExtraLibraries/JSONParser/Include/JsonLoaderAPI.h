// JsonLoaderAPI.h
// The API of a basic json parser. 
// Using singleton pattern.
// Giannhs Apostolidhs, january 2013.
//

#ifndef	JSONLOADERAPI_H
#define	JSONLOADERAPI_H

#include <string>
#include <list>
#include "DeltaObject.h"
#include "DeltaValue.h"
#include "DeltaJsonParserLib.h"

#define JSON_NULL_TYPE_STR	"json::null"
#define JSON_NULL			(void*) 0     

class DJSONPARSER_CLASS JsonLoaderAPI{
	public:
		static void					SingletonCreate (void) 
										{ singletonPtr = new JsonLoaderAPI; }
		static void					SingletonDestroy (void) 
										{ delete singletonPtr; singletonPtr = 0; }

		static DeltaObject*			Load (const std::string& path, bool retainNull );
		static DeltaObject*			LoadFromString (const std::string& buffer, bool retainNull );
		static bool					Write (const char* file, DeltaTable* table);

		static const DeltaValue		Null (void);

		static void					SetLine (util_ui32 line);
		static util_ui32			GetLine (void);
		static const std::string&	GetFile (void);
		static void					SetFile (const std::string& file);

	private:  
		static std::string*			currFile;
		static util_ui32			currLine;
		static JsonLoaderAPI*		singletonPtr;

		static DeltaObject*			Parse (void (*cleanUpScanner)(void), bool retainNull);
		static void					CleanUpScannerFromFile (void);
		static void					CleanUpScannerFromString (void);

		static bool					WriteObject(FILE* fp, DeltaTable* table, util_ui32 tabs);
		static bool					WriteArray(FILE* fp, DeltaTable* table, util_ui32 tabs);
		static bool					WriteValue(FILE* fp, DeltaValue& table, util_ui32 tabs);

		JsonLoaderAPI (void);
		~JsonLoaderAPI();
};

#endif