// RcLoaderAPI.cpp
// API for ResourceLoader.
// ScriptFigher Project.
// A. Savidis, January 2000.
// Updated July 2009, supported ordered numerically indexed items and
// error checking.
//

#include <stdio.h>
#include <stdlib.h>
#include "DDebug.h"
#include "RcLoaderAPI.h"
#include "RcParseActions.h"
#include "RcLoaderErrorMsg.h"
#include "DeltaTable.h"
#include "ustrings.h"
#include "ulexutil.h"
#include "uerrorclass.h"
#include "ugeometry.h"
#include "ustrings.h"

#ifdef _UNIX_
#include <unistd.h>
#endif

//////////////////////////////////////////////////////////////////

USTRINGDESTRUCTOR_DECLARE(UCLASSQUALIFIER_EMPTY, RcLoaderStringDestructor)
USTRINGDESTRUCTOR_IMPL(RcLoaderStringDestructor)

char* RcLoader::StringLateDestroy (char* s) {
	RcLoaderStringDestructor::GetSingleton().add(DNEWCLASS(udestroyablestring, (s))); 
	return s;
}

ResourceLoader::Data* ResourceLoader::data = (Data*) 0;

//////////////////////////////////////////////////////////////////

extern FILE*	ResourceLoader_yyin;
extern int		ResourceLoader_yyparse (void);
extern void		ResourceLoader_ResetLex (FILE* fp);

//////////////////////////////////////////////////////////////////

static ubag<DeltaTable*>* visited = (ubag<DeltaTable*>*) 0;
void ResourceLoader::SingletonCreate (void) 
	{ RcLoaderStringDestructor::SingletonCreate(); unew(data); unew(visited); }

void ResourceLoader::SingletonDestroy (void) 
	{ RcLoaderStringDestructor::SingletonDestroy(); udelete(data); udelete(visited); }

//////////////////////////////////////////////////////////////////

bool ResourceLoader::InitialiseScannerFromFile (const std::string& file) {
	
	DPTR(data)->currFile = file;
	DASSERT(!ResourceLoader_yyin);
	ResourceLoader_yyin = fopen(Preprocess(file).c_str(), "r");

	if (!ResourceLoader_yyin) {
		ResourceLoaderMsg("Could not open file '%s' for compiling.\n", file.c_str());
		DPTR(data)->currFile.clear();
		return false;
	}
	else {
		ResourceLoaderMsg("Start compiling resource file '%s'.\n", file.c_str());
		ResourceLoader_ResetLex(ResourceLoader_yyin);
		return true;
	}
}

//************************

void ResourceLoader::CleanUpScannerFromFile (void) {
	DASSERT(ResourceLoader_yyin);
	fclose(ResourceLoader_yyin);
	unullify(ResourceLoader_yyin);
}

//////////////////////////////////////////////////////////////////

extern void RcLoaderInitialiseScannerFromString (const char* str);
extern void RcLoaderCleanUpScannerFromString (void);

bool ResourceLoader::InitialiseScannerFromString (const std::string& str) {
	RcLoaderInitialiseScannerFromString(str.c_str()); 
	DPTR(data)->currFile = "<text buffer>";
	if (HasPreprocessor()) 
		ResourceLoaderMsg("Warning: cpp ignored since compiling from text buffer!");
	return true;
}

void ResourceLoader::CleanUpScannerFromString (void) 
	{ RcLoaderCleanUpScannerFromString(); }

//////////////////////////////////////////////////////////////////

DeltaTable* ResourceLoader::Load (const std::string& file) {
	if (!InitialiseScannerFromFile(file))
		return (DeltaTable*) 0;
	else
		return Parse(&CleanUpScannerFromFile);
}

//////////////////////////////////////////////////////////////////

DeltaTable*	ResourceLoader::LoadFromString (const std::string& buffer) {
	DPTR(data)->currFile.clear();
	if (!InitialiseScannerFromString(buffer))
		return (DeltaTable*) 0;
	else
		return Parse(&CleanUpScannerFromString);
}

//////////////////////////////////////////////////////////////////

void ResourceLoader::SetLine (util_ui32 line)
	{ DPTR(data)->currLine = line; }

util_ui32 ResourceLoader::GetLine (void)
	{ return DPTR(data)->currLine; }

void ResourceLoader::NextLine (void)
	{ ++DPTR(data)->currLine; }

const std::string&	ResourceLoader::GetFile (void)
	{ return DPTR(data)->currFile; }

void ResourceLoader::SetFile (const std::string& file)
	{ DPTR(data)->currFile = file; }

void ResourceLoader::SetPreprocessor (const std::string& fullPath, const std::string& includeFlags) {
	DPTR(data)->cpp		= fullPath;
	DPTR(data)->include = includeFlags;
}

//////////////////////////////////////////////////////////////////

bool ResourceLoader::HasPreprocessor (void)
	{ return !DPTR(data)->cpp.empty(); }

const std::string ResourceLoader::Preprocess (const std::string& file) {
	if (HasPreprocessor()) {
		const char* s = tmpnam(NULL);
		DPTR(data)->temp = *s == '\\' ? "." : "";
		(data)->temp.append(s);
		std::string call =	uconstructstr("%s %s %s > %s",
								DPTR(data)->cpp.c_str(),
								file.c_str(),
								DPTR(data)->include.c_str(),
								DPTR(data)->temp.c_str()
							);
		system(call.c_str());
		return DPTR(data)->temp;
	} 
	else
		return file;
}

//////////////////////////////////////////////////////////////////

void ResourceLoader::ParseLineDirective (const char* yytext) {
	util_ui32 line;
	sscanf(yytext + 1, "%u", &line);
	SetLine(line);
	SetFile(DNULLCHECK(strchr(yytext, '\"')) + 1);
}

//////////////////////////////////////////////////////////////////

DeltaTable* ResourceLoader::Parse (void (*cleanUpScanner)(void)) {

	DPTR(data)->currLine = 1;
	AllocateResources();

	ResourceLoaderResetErrors(); 
	ResourceLoader_yyparse();
	(*cleanUpScanner)();
	
	if (HasPreprocessor())	// Temp outout file must be removed.
		{ unlink(DPTR(data)->temp.c_str());  DPTR(data)->currFile.clear(); }

	util_ui32 numErrors = ResourceLoaderErrorsExist();
	if (!numErrors)
		ResourceLoaderMsg("Finished compilation, no errors.\n");
	else
		ResourceLoaderMsg("Finished compilation, %d errors.\n", numErrors);

	UnreferenceTables();
	RcLoaderStringDestructor::GetSingleton().commit();

	if (!numErrors)
		return GetResources();
	else {
		ReleaseResources(); 
		return (DeltaTable*) 0; 
	}
}

//////////////////////////////////////////////////////////////////

void ResourceLoader::SetErrorCallback (void (*callback)(const char*))  
	{ ResourceLoaderSetErrorCallback(callback); }

//////////////////////////////////////////////////////////////////

#define	ID_TAB_STOPS	hasId ? "" : utabstops(tabs).c_str()
#define	TAB_STOPS		utabstops(tabs).c_str()
#define	COMMA			comma ? "," : ""

bool ResourceLoader::WriteValue (
		FILE*				fp, 
		DeltaValue&			content, 
		bool				comma, 
		util_ui32			tabs, 
		const std::string&	indexing,
		bool				hasId
	) {

	switch (content.Type()) {

		case DeltaValue_Table: {
			DeltaTable*		t	= content.ToTable();
			bool			singleton = t->Total() <= 1;

			if (singleton)	fprintf(fp, "%s[", ID_TAB_STOPS);
			else			fprintf(fp, "%s[\n", ID_TAB_STOPS);

			Write(fp, t, true, tabs + 1, indexing);

			if (singleton)	fputs("]", fp);
			else			fprintf(fp,"\n%s]", TAB_STOPS);
			return true;
		}

		case DeltaValue_Number: {
			fprintf(fp, "%s%s", ID_TAB_STOPS, unum2string(content.ToNumber()).c_str());
			return true;
		}

		case DeltaValue_String: {
			fprintf(fp, "%s\"%s\"", ID_TAB_STOPS, uunparseescapesequences(content.ToString().c_str()).c_str());
			return true;
		}

		case DeltaValue_Bool: {
			fprintf(fp, "%s%s", ID_TAB_STOPS, ubool2conststr(content.ToBool()));
			return true;
		}

		default: {
			fprintf(
				fp, 
				"%s\"In RC write: element '%s' is of type '%s' not allowed in RC files!\"",
				ID_TAB_STOPS, 
				uunparseescapesequences(indexing.c_str()).c_str(),
				content.TypeStr()
			); 
			return false;
		}
	}
}

#undef	ID_TAB_STOPS

//////////////////////////////////////////////////////////////////

bool ResourceLoader::IsSubarrayFollowing (
		DeltaTable*	indices, 
		util_ui32	total,
		util_ui32	i, 
		util_ui32	num, 
		util_ui32	groupSize
	) {

	if (i + groupSize >= total)	// Not enough elements to find the required group.
		return false;
	++num;

	while (groupSize--) {
		DeltaValue index;
		DPTR(indices)->Get(DeltaValue((DeltaNumberValueType) ++i), &index);
		if (index.Type() != DeltaValue_Number)	
			return false;
		double x = index.ToNumber();
		if (ufractionalpart(x) != 0)
			return false;
		if (((unsigned) x) != num)
			return false;
		++num;
	}

	return true;
}

//////////////////////////////////////////////////////////////////

#define	ARRAY_GROUP_MIN_SIZE 2

bool ResourceLoader::Write (
		FILE*				fp, 
		DeltaTable*			table, 
		bool				comma, 
		util_ui32			tabs, 
		const std::string&	indexing
	) {

	if (DPTR(visited)->find(table) != DPTR(visited)->end()) {
		fprintf(fp, "%s\"Circural reference on 0x%x\"\n", TAB_STOPS, table->GetSerialNo());
		return true;
	}
	else
		DPTR(visited)->insert(table);

	DeltaValue index, content;	
	bool first	= true;
	bool result = true;	

	DeltaTable* indices = table->GetIndices();
	DPTR(indices)->IncRefCounter((DeltaValue*) 0);
	util_ui32 autoNumIndex = 0;

	for (util_ui32 n = table->Total(), i = 0; i < n; ++i) {
	
		DPTR(indices)->Get(DeltaValue((DeltaNumberValueType) i), &index);
		table->GetBySkippingUserCode(index, &content);

		bool hasId = false;
		if (!first)
			fprintf(fp, "%s\n", COMMA);
		std::string subIndex;

		// Write resource index.
		if (index.Type() == DeltaValue_String) {

			std::string indexStr = uunparseescapesequences(index.ToString().c_str());

			if (DeltaString::IsIdentifier(indexStr)) {
				fprintf(fp, "%s%s ", TAB_STOPS, indexStr.c_str());
				subIndex = uconstructstr(".%s", indexStr.c_str());
			}
			else {
				fprintf(fp, "%s\"%s\" : ", TAB_STOPS, indexStr.c_str());
				subIndex = uconstructstr("[\"%s\"]", indexStr.c_str());
			}
			hasId = true;			
		}
		else
		if (index.Type() == DeltaValue_Number) {

			double num = index.ToNumber();
			subIndex = std::string("[") + unum2string(num) + "]";

			if (num < 0 || ufractionalpart(num) != 0) {
				fprintf(
					fp,
					"\"In RC write: illegal numeric index '%s' (unsigned allowed only)!\"",
					(indexing + subIndex).c_str()
				); 
				result = false;
				continue;
			}

			if (((unsigned) num) == autoNumIndex)	// Implicit indexing since we have successive indices.
				++autoNumIndex;
			else
			if (IsSubarrayFollowing(indices, n, i, (unsigned) num, ARRAY_GROUP_MIN_SIZE - 1)) {
				autoNumIndex = (unsigned) num + 1;
				fprintf(fp, "%s@%u,\n", TAB_STOPS, (unsigned) num);
			}
			else {	// Explicit indexing.
				fprintf(fp, "%s%u : ", TAB_STOPS, (unsigned) num);
				subIndex = uconstructstr("[%u]", (unsigned) num);
				hasId = true;
			}
		}
		else {
			fprintf(
				fp,
				"\"In RC write: illegal index type '%s' at elementt '%s'!\"",
				index.TypeStr(), 
				indexing.c_str()
			); 
			result = false;
			continue;
		}

		// Write resource value.
		WriteValue(fp, content, comma, tabs, indexing + subIndex, hasId);
		first = false;
	}

	DPTR(indices)->DecRefCounter((DeltaValue*) 0);
	return result;
}

//////////////////////////////////////////////////////////////////

bool ResourceLoader::Write (const char* file, DeltaTable* table) {

	FILE* fp = fopen(file, "wt");
	if (!fp)
		return false;
	else {
		bool result = Write(fp, table, false, 0, "<rc>");
		fclose(fp);
		DPTR(visited)->clear();
		return result;
	}
}

//////////////////////////////////////////////////////////////////
