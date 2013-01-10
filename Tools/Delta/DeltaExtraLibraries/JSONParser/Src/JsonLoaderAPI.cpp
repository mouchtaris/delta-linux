
#include <iostream>
#include "JsonLoaderAPI.h"
#include "DDebug.h"
#include "ustrings.h"
#include "JsonLoaderErrorMsg.h"
#include "JsonParseActions.h"
#include "uinit.h"

JsonLoaderAPI * JsonLoaderAPI::singletonPtr = (JsonLoaderAPI*) 0;
std::string*	JsonLoaderAPI::currFile		= (std::string*) 0;
util_ui32		JsonLoaderAPI::currLine		= (util_ui32) 0;

extern FILE*	JsonLex_yyin;
extern void		JsonLex_yyrestart(FILE *);
extern int		JsonLex_yylineno;
extern int		JsonLex_yyparse (DeltaObject**, bool);

JsonLoaderAPI::JsonLoaderAPI(void)
{
	currFile = DNEW(std::string);
}

JsonLoaderAPI::~JsonLoaderAPI(void)
{
	DDELETE(currFile);
}

static bool InitialiseScannerFromFile (const std::string& file)
{
	JsonLex_yyin = fopen(file.c_str(), "r");
	if (!JsonLex_yyin) {
		JsonParserLoaderErrorMsg::JsonLoaderError("Could not open file '%s' for reading", file.c_str());
		return false;
	}
	else {
		JsonParserLoaderErrorMsg::JsonLoaderMsg("Start reading JSON file '%s'.\n", file.c_str());
		//Reset Lexical Analizer (yyrestart(yyin))
		JsonLex_yyrestart( JsonLex_yyin );
		return true;
	}
}

void JsonLoaderAPI::CleanUpScannerFromFile (void)
{
	currFile->clear();
	currLine = (util_ui32) 0;
	fclose(JsonLex_yyin);
	JsonLex_yyin = (FILE *) 0;
}

extern void		JsonLoaderInitialiseScannerFromString (const char* str);
extern void		JsonLoaderCleanUpScannerFromString (void);

static bool InitialiseScannerFromString (const std::string& str) {
	JsonParserLoaderErrorMsg::JsonLoaderMsg("Start reading JSON from string.\n");
	JsonLoaderInitialiseScannerFromString(str.c_str()); 
	return true;
}

void JsonLoaderAPI::CleanUpScannerFromString (void) {
	currFile->clear();
	currLine = (util_ui32) 0;
	JsonLoaderCleanUpScannerFromString();
}

void JsonLoaderAPI::SetLine (util_ui32 line)
	{ currLine = line; }

util_ui32 JsonLoaderAPI::GetLine (void)
	{ return currLine; }

const std::string&	JsonLoaderAPI::GetFile (void)
	{ return *currFile; }

void JsonLoaderAPI::SetFile (const std::string& file)
	{ currFile->assign(file); }


DeltaObject* JsonLoaderAPI::Load (const std::string& path, bool retainNull ){

	if (!InitialiseScannerFromFile(path)) {
		currFile->clear();
		return (DeltaObject*) 0;
	}
	else {
		currFile->assign(path);
		return Parse(&CleanUpScannerFromFile, retainNull);
	}
}

DeltaObject* JsonLoaderAPI::LoadFromString (const std::string& buffer, bool retainNull ){
	
	if (!InitialiseScannerFromString(buffer)) {
		currFile->clear();
		return (DeltaObject*) 0;
	}
	else {
		currFile->assign("<text buffer>");
		return Parse(&CleanUpScannerFromString, retainNull);
	}
}

DeltaObject* JsonLoaderAPI::Parse (void (*cleanUpScanner)(void), bool retainNull)
{
	JsonLex_yylineno = 1;

	JsonParserLoaderErrorMsg::JsonLoaderResetErrors();

	DeltaObject* result = (DeltaObject *)0;
	JsonLex_yyparse(&result, retainNull);

	(*cleanUpScanner)();

	util_ui32 numErrors = JsonParserLoaderErrorMsg::JsonLoaderNumErrors();
	if (!numErrors)
		JsonParserLoaderErrorMsg::JsonLoaderMsg("Finished compilation, no errors.\n");
	else
		JsonParserLoaderErrorMsg::JsonLoaderMsg("Finished compilation, %d errors.\n", numErrors);

	return result;
}

bool JsonLoaderAPI::Write (const char* file, DeltaTable* table){
	FILE* fp = fopen(file, "wt");
	bool done = true;
	
	if (!fp)
		done = false;
	else {
		if (table)
			done = WriteObject(fp, table, 1);
		fclose(fp);
	}

	return done;
}

#define	TAB_STOPS	utabstops(tabs).c_str()

bool JsonLoaderAPI::WriteArray(FILE* fp, DeltaTable* table, util_ui32 tabs){
	DeltaValue indexed, content;

	DeltaTable* indices = DPTR(table)->GetIndices();

	fprintf(fp, "[\n%s", TAB_STOPS);
	for (util_ui32 i = DPTR(table)->Total(); i; --i) {
		DPTR(indices)->Get(DeltaValue((DeltaNumberValueType) i - 1), &indexed);
		DPTR(table)->Get(indexed, &content);
		if(i != DPTR(table)->Total())
			fprintf(fp, ",\n%s", TAB_STOPS);		
		WriteValue(fp, content, tabs);
	}
	--tabs;
	fprintf(fp, "\n%s]", TAB_STOPS);

	return true;
}

bool JsonLoaderAPI::WriteObject(FILE* fp, DeltaTable* table, util_ui32 tabs){
	DeltaValue index, content;

	DeltaTable* indices = DPTR(table)->GetIndices();

	fprintf(fp, "%{\n", TAB_STOPS);
	for (util_ui32 i = DPTR(table)->Total(); i; --i) {
		DPTR(indices)->Get(DeltaValue((DeltaNumberValueType) i - 1), &index);
		DPTR(table)->Get(index, &content);
		if (i != DPTR(table)->Total())
			fprintf(fp, ",\n", TAB_STOPS);
		fprintf(fp, "%s\"%s\" : ", TAB_STOPS, index.ToString().c_str()); 
		WriteValue(fp, content, tabs);
	}
	--tabs;
	fprintf(fp, "\n%s}", TAB_STOPS);

	return true;
}

bool JsonLoaderAPI::WriteValue(FILE* fp, DeltaValue& value, util_ui32 tabs){
	DeltaTable* obj, *indices;
	DeltaValue val;

	DASSERT(
		value.Type() == DeltaValue_Table	|| value.Type() == DeltaValue_String || 
		value.Type() == DeltaValue_Bool		|| value.Type() == DeltaValue_Number || 
		value.Type() == DeltaValue_ExternId
	);

	if(value.Type() == DeltaValue_Table) {
		obj = value.ToTable();
		indices = obj->GetIndices();
		indices->Get(DeltaValue((DeltaNumberValueType) 0), &val);

		++tabs;
		if (val.Type() == DeltaValue_String)
			WriteObject(fp, obj, tabs);
		else
			WriteArray(fp, obj, tabs);
	}
	else{
		std::string typeStr;

		if(value.Type() == DeltaValue_ExternId){
			void * val = value.ToExternId(typeStr);
			DASSERT(typeStr == JSON_NULL_TYPE_STR);
			DASSERT(val ==  JSON_NULL);
			fprintf( fp, "null");
		}	
		else
		if(value.Type() == DeltaValue_String )
			fprintf( fp, "\"%s\"", value.ConvertToString().c_str());
		else
		if(value.IsBool())
			fprintf( fp, "%s", ( value.ToBool() ? "true" : "false" ) );
		else
		if(value.IsNumber())
			fprintf( fp, "%s", value.ConvertToString().c_str());
		else
			DASSERT(false);
	}

	return true;
}

static void ToString (DeltaString* at, void* val) {
    DASSERT(val == JSON_NULL);
    DPTR(at)->Add(JSON_NULL_TYPE_STR);
}

const DeltaValue JsonLoaderAPI::Null (void) {
	return	DeltaValue(
				JSON_NULL,                                         
				DeltaExternId_NonCollectable,
				ToString, 
				JSON_NULL_TYPE_STR,
				DELTA_EXTERNID_NO_FIELD_GETTER
			);
}