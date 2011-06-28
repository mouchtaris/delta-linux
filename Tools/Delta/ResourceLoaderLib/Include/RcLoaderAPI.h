// RcLoaderAPI.h
// API for ResourceLoader.
// ScriptFigher Project.
// A. Savidis, January 2000.
//

#ifndef	RCLOADERAPI_H
#define	RCLOADERAPI_H

#include <stdio.h>
#include "utypes.h"
#include "usingleton.h"
#include "RcLoaderDefs.h"
#include "DeltaValue.h"
#include <string>

class DeltaTable;

class RCLOAD_CLASS ResourceLoader {
	private:
	struct Data {
		util_ui32	currLine;
		std::string	currFile, temp;
		std::string cpp, include;
	};
	static Data* data;

	static bool					WriteValue (
									FILE*				fp, 
									DeltaValue&		content, 
									bool				comma, 
									util_ui32			tabs, 
									const std::string&	indexing, 
									bool				hasId
								);
	static bool					Write (
									FILE*				fp, 
									DeltaTable*			table, 
									bool				comma, 
									util_ui32			tabs, 
									const std::string&	indexing
								);

	static bool					IsSubarrayFollowing (
									DeltaTable*			indices, 
									util_ui32			total,
									util_ui32			i, 
									util_ui32			num, 
									util_ui32			groupSize
								);

	static DeltaTable*			Parse (void (*cleanUpScanner)(void));
	static bool					InitialiseScannerFromFile (const std::string& file);
	static void					CleanUpScannerFromFile (void);
	static bool					InitialiseScannerFromString (const std::string& str);
	static void					CleanUpScannerFromString (void);
	static bool					HasPreprocessor (void);
	static const std::string	Preprocess (const std::string& file);

	public:
	static void					SetLine (util_ui32 line);
	static util_ui32			GetLine (void);
	static void					NextLine (void);
	static const std::string&	GetFile (void);
	static void					SetFile (const std::string& file);
	
	static void					ParseLineDirective (const char* yytext);
	static void					SetPreprocessor (const std::string& fullPath, const std::string& includeFlags); 
	static DeltaTable*			Load (const std::string& path);
	static DeltaTable*			LoadFromString (const std::string& buffer);
	static bool					Write (const char* file, DeltaTable* table);
	static void					SetErrorCallback (void (*callback)(const char*));
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
};

#endif	// Do not add stuff beyond this point.
