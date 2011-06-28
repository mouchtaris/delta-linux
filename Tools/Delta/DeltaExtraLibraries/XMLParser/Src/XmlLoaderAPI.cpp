#include "XmlLoaderAPI.h"
#include "ReferenceTranslation.h"
#include <iostream>
#include "DDebug.h"
#include "ustrings.h"
#include "XmlLoaderErrorMsg.h"

XmlLoaderAPI* XmlLoaderAPI::singletonPtr = (XmlLoaderAPI*) 0;
std::string* XmlLoaderAPI::currFile = (std::string*) 0;
util_ui32 XmlLoaderAPI::currLine = (util_ui32) 0;
bool XmlLoaderAPI::outertag = false;
std::map<util_ui32, std::list<util_ui32> > XmlLoaderAPI::serialNums;

extern FILE*	XmlLex_yyin;
extern void		XmlLex_yyrestart( FILE * );
extern int		XmlLex_yylineno;
extern int		XmlLex_yyparse ( DeltaObject** );

DeltaObject* table = (DeltaObject *)0;


XmlLoaderAPI::XmlLoaderAPI(void)
{
	currFile = DNEW(std::string);
}

XmlLoaderAPI::~XmlLoaderAPI(void)
{
	DDELETE(currFile);
}

static bool InitialiseScannerFromFile (const std::string& file)
{
	XmlLex_yyin = fopen(file.c_str(), "r");
	if (!XmlLex_yyin) {
		XmlLoaderError("Could not open file '%s' for reading", file.c_str());
		return false;
	} else {
		XmlLoaderMsg("Start reading XML file '%s'.\n", file.c_str());
		//Reset Lexical Analizer (yyrestart(yyin))
		XmlLex_yyrestart( XmlLex_yyin );
		return true;
	}
}

void XmlLoaderAPI::CleanUpScannerFromFile (void)
{
	currFile->clear();
	currLine = (util_ui32) 0;
	fclose(XmlLex_yyin);
	XmlLex_yyin = (FILE *) 0;
}

extern void		XmlLoaderInitialiseScannerFromString (const char* str);
extern void		XmlLoaderCleanUpScannerFromString (void);

static bool InitialiseScannerFromString (const std::string& str) {
	XmlLoaderMsg("Start reading XML from string.\n");
	XmlLoaderInitialiseScannerFromString(str.c_str()); 
	return true;
}

void XmlLoaderAPI::CleanUpScannerFromString (void) {
	currFile->clear();
	currLine = (util_ui32) 0;
	XmlLoaderCleanUpScannerFromString();
}

DeltaObject* XmlLoaderAPI::Load (const std::string& path)
{
	DASSERT(!table);
	if (!InitialiseScannerFromFile(path)) {
		currFile->clear();
		return (DeltaObject*) 0;
	} else {
		currFile->assign(path);
		return Parse(&CleanUpScannerFromFile);
	}
}

DeltaObject*	XmlLoaderAPI::LoadFromString (const std::string& buffer)
{
	if (!InitialiseScannerFromString(buffer)) {
		currFile->clear();
		return (DeltaObject*) 0;
	} else {
		currFile->assign("<text buffer>");
		return Parse(&CleanUpScannerFromString);
	}
}

DeltaObject* XmlLoaderAPI::Parse (void (*cleanUpScanner)(void))
{
	XmlLex_yylineno = 1;

	XmlLoaderResetErrors();
	ReferenceTranslation::Initialize();

	XmlLex_yyparse(&table);

	ReferenceTranslation::CleanUp();
	(*cleanUpScanner)();

	util_ui32 numErrors = XmlLoaderNumErrors();
	if (!numErrors)
		XmlLoaderMsg("Finished compilation, no errors.\n");
	else
		XmlLoaderMsg("Finished compilation, %d errors.\n", numErrors);


	if (!numErrors) {
		DeltaObject* ret = table;
		table = (DeltaObject*) 0;
		return ret;
	} else {
		return (DeltaObject*) 0; 
	}
}

#define	NAME_KEY		"$Name"
#define	CHARDATA_KEY	"$CharData"
#define ATTRIBUTES_KEY	"$Attributes"

bool XmlLoaderAPI::Write (const char* file, DeltaTable* table)
{
	FILE* fp = fopen(file, "wt");
	if (!fp)
		return false;
	else {		//Index "$Name" used to take table's name.
		DeltaValue index, value;
		bool result;
		std::list<util_ui32> emptylist;

		outertag = true;
		index.FromString(NAME_KEY);
		DPTR(table)->Get(index, &value);

		ReferenceTranslation::Initialize();
		serialNums.insert(
			std::pair<util_ui32, std::list<util_ui32> > 
				(DPTR(table)->GetSerialNo(), emptylist));

		if (value.Type() == DeltaValue_String) {
			result = Write(fp, table, 0, value.ToString());
		} else {
			result = Write(fp, table, 0, "unnamed");
		}

		serialNums.clear();
		ReferenceTranslation::CleanUp();
		fclose(fp);

		return result;
	}
}



#define	TAB_STOPS		utabstops(tabs).c_str()

bool XmlLoaderAPI::Write (FILE* fp,
						  DeltaTable* table,
						  util_ui32 tabs,
						  const std::string& indexing)
{
	DeltaTable* indices = table->GetIndices();

	XmlWriteIndexType type = CheckTable(fp, table, indices);
	bool result = (type != ErrorIndex);

	if (result) {
		if (type != NumericIndex)
			fprintf(fp, "%s<%s", TAB_STOPS, indexing.c_str());

		WriteAttributes(fp, table, indexing, indices);

		if (type != NumericIndex)
			fprintf(fp, ">\n");

		WriteTabs(fp, table, tabs, indexing, indices);

		if (type != NumericIndex)
			fprintf(fp, "%s</%s>\n", TAB_STOPS, indexing.c_str());
	}
	return result;
}

bool XmlLoaderAPI::CheckStringIndex(std::string str)
{
	if (str.length() == 0)
		return false;
	else if (!isalpha(str[0]) &&
			 str[0] != ':' &&
			 str[0] != '_')
		return false;
	else {
		for (std::string::iterator it = str.begin(); it != str.end(); it++) {
			if (!isalnum(*it) &&
				*it != ':' &&
				*it != '_' &&
				*it != '-' &&
				*it != '.')
				return false;
		}
	}
	return true;
}

bool XmlLoaderAPI::FindCircles(util_ui32 contentId, util_ui32 tableId)
{
	std::map<util_ui32, std::list<util_ui32> >::iterator map_it = serialNums.find(contentId);
	if (map_it != serialNums.end()) {
		for (std::list<util_ui32>::iterator it = (*map_it).second.begin(); it != (*map_it).second.end(); it++) {
			if (*it == tableId) {
				return false;
			} else if (!FindCircles(*it, tableId)) {
				return false;
			}
		}
	}
	return true;
}

XmlWriteIndexType XmlLoaderAPI::CheckTable(FILE *fp,
										   DeltaTable *table,
										   DeltaTable *indices)
{
	DeltaValue index, content;
	XmlWriteIndexType type = UndefinedIndex;

	bool hastablevalues = false;

	for (util_ui32 n = table->Total(), i = 0; i < n; ++i) {
		DPTR(indices)->Get(DeltaValue((DeltaNumberValueType) i), &index);
		DPTR(table)->Get(index, &content);

		switch( index.Type() ) {
			case (DeltaValue_Number):

				if (outertag) {
					type = ErrorIndex;
					fprintf(
						fp,
						"\"In Xml write: illegal index type '%s'! Cannot have numeric indices in outer tag.\"\n",
						index.TypeStr()
					);
					break;
				}
				switch( type ) {
					case (UndefinedIndex):
					case (NumericIndex):
						type = NumericIndex;
						break;
					default:
						type = ErrorIndex;
						fprintf(
							fp,
							"\"In Xml write: illegal index type '%s'! Cannot have both numeric and string indices.\"\n",
							index.TypeStr()
						);
						break;
				}
				if (type == ErrorIndex)
					break;
				if (content.Type() == DeltaValue_Table) {
					hastablevalues = true;
					DeltaTable* tab_indices = content.ToTable()->GetIndices();
					XmlWriteIndexType tab_type = CheckTable(fp, content.ToTable(), tab_indices);

					if (tab_type == NumericIndex) {
						type = ErrorIndex;
						fprintf (
							fp,
							"\"In Xml write: Cannot have numeric indices into a numerically indexed table.\"\n"
						);
					}
				} else if ((content.Type() == DeltaValue_String ||
							content.Type() == DeltaValue_Number) && hastablevalues) {
					type = ErrorIndex;
					fprintf(
						fp,
						"\"In Xml write: illegal content type '%s'! Expected table value instead.\"\n",
						content.TypeStr()
					);
				}
				break;
			case (DeltaValue_String):

				if (index.ToString() == NAME_KEY ||
					index.ToString() == CHARDATA_KEY ||
					index.ToString() == ATTRIBUTES_KEY)
					continue;
				switch( type ) {
					case (UndefinedIndex):
					case (StringIndex):
						type = StringIndex;
						break;
					default:
						type = ErrorIndex;
						fprintf(
							fp,
							"\"In Xml write: illegal index type '%s'! Cannot have both numeric and string indices.\"\n",
							index.TypeStr()
						);
						break;
				}
				if (type != ErrorIndex)
					if (!CheckStringIndex(index.ToString())) {
						type = ErrorIndex;
						fprintf(
							fp,
							"\"In Xml write: illegal index value '%s'!\"\n",
							index.ToString().c_str()
						);
					}
				break;
			default:
				type = ErrorIndex;
				fprintf(
					fp,
					"\"In Xml write: illegal index type '%s'!\"\n",
					index.TypeStr()
				);
				break;
		}
		if (content.Type() == DeltaValue_Table) {
			util_ui32 contentSerialNo = content.ToTable()->GetSerialNo();
			util_ui32 tableSerialNo = DPTR(table)->GetSerialNo();
			if (!FindCircles(contentSerialNo, tableSerialNo)) {
				type = ErrorIndex;
				fprintf(
					fp,
					"\"In Xml write: Cannot have circle table references.\"\n"
				);
			}
		}
		if (type == ErrorIndex)
			break;
	}
	outertag = false;
	return type;
}

void XmlLoaderAPI::WriteAttributes (FILE* fp,
									DeltaTable* table,
									const std::string& indexing,
									DeltaTable* indices)
{
	DeltaValue index, content;
	for (util_ui32 n = table->Total(), i = 0; i < n; ++i) {
		DPTR(indices)->Get(DeltaValue((DeltaNumberValueType) i), &index);
		DPTR(table)->Get(index, &content);
		if (index.Type() == DeltaValue_Table ||
			index.Type() == DeltaValue_Number) {
		} else if (index.Type() == DeltaValue_String) {
			if (index.ToString() == CHARDATA_KEY ||
				index.ToString() == NAME_KEY)
			{
				continue;
			} else if (index.ToString() == ATTRIBUTES_KEY && content.Type() == DeltaValue_Table &&
					   CheckTable(fp, content.ToTable(), content.ToTable()->GetIndices()) == StringIndex) {
				WriteAttributes(fp, content.ToTable(), indexing, content.ToTable()->GetIndices());
			} else if (content.Type() == DeltaValue_String) {
				fprintf(
					fp,
					" %s=\"%s\"",
					index.ToString().c_str(),
					ReferenceTranslation::Singleton()->RemoveIllegalXmlCharacters(&content.ToString())->c_str()
				);
			} else if (content.Type() == DeltaValue_Number) {
				fprintf(
					fp,
					" %s=\"%s\"",
					index.ToString().c_str(),
					content.ConvertToString().c_str()
				);
			}
		}
	}
}

void XmlLoaderAPI::WriteTabs (FILE* fp,
							  DeltaTable* table,
							  util_ui32 tabs,
							  const std::string& indexing,
							  DeltaTable* indices)
{
	DeltaValue index, content;
	for (util_ui32 n = table->Total(), i = 0; i < n; ++i) {
		DPTR(indices)->Get(DeltaValue((DeltaNumberValueType) i), &index);
		DPTR(table)->Get(index, &content);

		if ((index.Type() == DeltaValue_Number && indexing == CHARDATA_KEY) ||
			(index.Type() == DeltaValue_String && index.ToString() == CHARDATA_KEY)) {
			if (content.Type() == DeltaValue_String) {
				fprintf(
					fp,
					"%s\t<![CDATA[%s]]>\n",
					TAB_STOPS,
					ReferenceTranslation::Singleton()->RemoveIllegalXmlCharacters(&content.ToString())->c_str()
				);
			} else if (content.Type() == DeltaValue_Table &&
					   CheckTable(fp, content.ToTable(), content.ToTable()->GetIndices()) == NumericIndex) {
				WriteTabs(fp, content.ToTable(), tabs, CHARDATA_KEY, content.ToTable()->GetIndices());
			}
		} else if (content.Type() != DeltaValue_Table) {
			continue;
		} else if (index.Type() == DeltaValue_String &&
			(index.ToString() == NAME_KEY || index.ToString() == ATTRIBUTES_KEY)) {
			continue;
		} else {
			std::map<util_ui32, std::list<util_ui32> >::iterator map_it = serialNums.find(content.ToTable()->GetSerialNo());
			if (map_it == serialNums.end()) {
				std::list<util_ui32> emptylist;
				serialNums.insert(std::pair<util_ui32, std::list<util_ui32> >
					(content.ToTable()->GetSerialNo(), emptylist));
			}
			std::map<util_ui32, std::list<util_ui32> >::iterator it = serialNums.find(DPTR(table)->GetSerialNo());
			DASSERT((*it).first == DPTR(table)->GetSerialNo());
			(*it).second.push_back(content.ToTable()->GetSerialNo());
			serialNums.insert(std::pair<util_ui32, std::list<util_ui32> >((*it).first, (*it).second));

			DeltaTable* tab_indices = content.ToTable()->GetIndices();
			if (CheckTable(fp, content.ToTable(), tab_indices) == NumericIndex) {
				DASSERT(index.Type() == DeltaValue_String);
				WriteTabs(fp, content.ToTable(), tabs+1, index.ToString(), tab_indices);
			} else if (index.Type() == DeltaValue_Number) {
				Write(fp, content.ToTable(), tabs, indexing);
				fprintf(fp, "%s\t<!--index of table %s is %d.-->\n", TAB_STOPS, indexing.c_str(), (int)index.ToNumber());
			} else if (index.Type() == DeltaValue_String) {
				Write(fp, content.ToTable(), tabs+1, index.ToString());
			}
		}
	}
}

void XmlLoaderAPI::SetLine (util_ui32 line)
{
	currLine = line;
}

util_ui32 XmlLoaderAPI::GetLine (void)
{
	return currLine;
}

const std::string& XmlLoaderAPI::GetFile (void)
{
	return *currFile;
}

void XmlLoaderAPI::SetFile (const std::string& file)
{
	currFile->assign(file);
}

////////////////////////////////////////////////////////////////
