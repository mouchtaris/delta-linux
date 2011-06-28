// Watch values test.
// A. Savidis, August 2008.
//
#if 0
#include "ddebug.h"
#include "DebugWatchValueInfo.h"
#include "DebugWatchValueTextEncoders.h"
#include "rcloaderapi.h"
#include "DeltaObjectToDebugWatchValueInfo.h"
#include "DeltaTable.h"
#include "DebugExprParserAPI.h"

///////////////////////////////////////////

void test (void) {

	DebugExprEvaluator::SingletonCreate();

	// 1. Make a table from rc
	DeltaTable* t = ResourceLoader::Load("rctest.txt");
	DASSERT(t);
	DeltaObject obj(t);
	
	// 2. convert to watch value.
	DebugWatchValueInfo watchValue;
	DeltaObjectToDebugWatchValueInfo::Convert(obj, &watchValue);

	// 3. encode to rc string, write it to file.
	//
	DebugWatchValueTextEncoders::Encoder f = DebugWatchValueTextEncoders::Get("rc");
	DASSERT(f);
	std::string rc = (*f)(watchValue);
	FILE* fp = fopen("rctestcopy.txt", "w");  fprintf(fp, "%s", rc.c_str()); fclose(fp);

	f = DebugWatchValueTextEncoders::Get("xml");
	DASSERT(f);
	std::string xml = (*f)(watchValue);
	fp = fopen("xmltestcopy.xml", "w");  fprintf(fp, "%s", xml.c_str()); fclose(fp);

	// 4. Reparse it.
	//
	t->DecRefCounter((DeltaObject*) 0);
	t = ResourceLoader::LoadFromString(rc);
	DASSERT(t);

	// 5. Write it again.
	//
	ResourceLoader::Write("rctestcopy2.txt", t);
	t->DecRefCounter((DeltaObject*) 0);

	DebugExprEvaluator::SingletonDestroy();
}

#endif