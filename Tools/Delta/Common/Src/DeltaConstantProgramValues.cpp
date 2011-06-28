// DeltaConstantProgramValues.cpp
// Constant array values, implementation as a separate class.
// ScriptFighter Project.
// A. Savidis, June 2005 (re-factoring).
//

#include "DeltaConstantProgramValues.h"
#include "ustrings.h"
#include "uerrorclass.h"
#include "ulexutil.h"

////////////////////////////////////////////////////////////////

DeltaConstantArraysWriter::DeltaConstantArraysWriter (void) {}
DeltaConstantArraysWriter::~DeltaConstantArraysWriter() {}

const char*	DeltaConstantArraysWriter::GetStringConst (util_ui16 index) const 
	{ return stringConsts.Get(index).c_str(); }

const char*	DeltaConstantArraysWriter::GetExternFuncUsed (util_ui16 index) const 
	{ return libFuncs.Get(index).c_str(); }

DeltaNumberValueType DeltaConstantArraysWriter::GetNumericConst (util_ui16 index) const
	{ return numericConsts.Get(index); }

//***************************

util_ui16 DeltaConstantArraysWriter::AddStringConst (const char* s) 
	{ return stringConsts.Add(std::string(s)); }

util_ui16 DeltaConstantArraysWriter::AddExternFuncUsed (const char* s)
	{ return libFuncs.Add(std::string(s)); }

util_ui16 DeltaConstantArraysWriter::AddNumericConst (DeltaNumberValueType val) 
	{ return numericConsts.Add(val); }

//***************************

void DeltaConstantArraysWriter::swritebin (GenericWriter& writer, util_ui16 total, const std::string* values) {
	writer.write(total);
	while (total--)
		writer.write(*values++);
}

void DeltaConstantArraysWriter::nwritebin (GenericWriter& writer, util_ui16 total, const DeltaNumberValueType* values) {
	writer.write(total);
	while (total--)
		writer.write(*values++);
}

//***************************

void DeltaConstantArraysWriter::swritetext (FILE* fp, util_ui16 total, const std::string* values) {
	fprintf(fp, "TOTAL = %d.\n", total);
	while (total--)
		fprintf(fp, "STRING \"%s\".\n", uunparseescapesequences((*values++).c_str()).c_str());
}

void DeltaConstantArraysWriter::nwritetext(FILE* fp, util_ui16 total, const DeltaNumberValueType* values) {
	fprintf(fp, "TOTAL = %d.\n", total);
	while (total--)
		fprintf(fp, "NUMERIC \"%6.6f\".\n", *values++);
}

//***************************

void DeltaConstantArraysWriter::Write (GenericWriter& writer) const {
	stringConsts.Write(writer, swritebin);
	libFuncs.Write(writer, swritebin);
	numericConsts.Write(writer, nwritebin);
}

void DeltaConstantArraysWriter::WriteText (FILE* fp) const {
	fprintf(fp,"STRING CONSTANTS:\n");
	stringConsts.Write(fp, swritetext);

	fprintf(fp,"USED LIBRARY FUNCTIONS:\n");
	libFuncs.Write(fp, swritetext);

	fprintf(fp,"NUMERIC CONSTANTS:\n");
	numericConsts.Write(fp, nwritetext);
}

////////////////////////////////////////////////////////////////

const char*	DeltaConstantArraysReader::GetStringConst (util_ui16 index) const 
	{ return stringConsts.Get(index).c_str(); }

const char*	DeltaConstantArraysReader::GetExternFuncUsed (util_ui16 index) const 
	{ return libFuncs.Get(index).c_str(); }

DeltaNumberValueType DeltaConstantArraysReader::GetNumericConst (util_ui16 index) const
	{ return numericConsts.Get(index); }

const DeltaNumberValueType* DeltaConstantArraysReader::GetNumericConstPtr (util_ui16 index) const
	{ return &numericConsts.Get(index); }

//***************************

bool DeltaConstantArraysReader::sreadbin (GenericReader& reader, util_ui16* total, std::string* arr) {
	if (!reader.read(total))
		return false;
	for (util_ui16 i = 0; i< *total; ++i)
		if (!reader.read(arr[i], false))
			return false;
	return true;
}

bool DeltaConstantArraysReader::nreadbin (GenericReader& reader, util_ui16* total, DeltaNumberValueType* arr) {
	if (!reader.read(total))
		return false;
	for (util_ui16 i = 0; i< *total; ++i)
		if (!reader.read(arr + i))
			return false;
	return true;
}

//***************************

#define	ERROR_HANDLER(what, errclass) \
	uerror::GetSingleton().post##errclass( \
		"Loading arrays of constants: error in reading '%s'!", what \
	); return false; \

bool DeltaConstantArraysReader::Read (GenericReader& reader) {

	UCHECK_PRIMARY_ERROR(stringConsts.Read(reader, sreadbin),	"strings");
	UCHECK_PRIMARY_ERROR(libFuncs.Read(reader, sreadbin),		"used libray functions");
	UCHECK_PRIMARY_ERROR(numericConsts.Read(reader, nreadbin), "numbers");
	return true;
}

//***************************

DeltaConstantArraysReader::DeltaConstantArraysReader (void) {}
DeltaConstantArraysReader::~DeltaConstantArraysReader() {}

////////////////////////////////////////////////////////////////
