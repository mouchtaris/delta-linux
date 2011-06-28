// ResourceLoaderTest.cpp.
// Test  program for resource loader and attribute parser.
// A. Savidis, February 2007.
//

#include "RcLoaderAPI.h"
#include "RcAttrRDParser.h"
#include "DDebug.h"
#include "VMInit.h"
#include "uinit.h"

static void OnError (const char* msg) 
	{ printf("%s" , msg); }

int main(int argc, char** argv) {

	DASSERT(argc == 2);
	dinit(OnError);
	UtilPackage::Initialise();
	VirtualMachinePackage::Initialise();
	ResourceLoader::SingletonCreate();
	ResourceLoader::SetErrorCallback(OnError);
	ResourceLoader::SetPreprocessor("\\bin\\cpp.exe", "");
	
	DeltaTable* t = ResourceLoader::Load(argv[1]);

	if (t) {
		RcAttrParser parser;
		char expr[256];
		printf("Expression:"); scanf("%s", expr);

		if (parser.Parse(t, expr)) {
			RcAttrParser::Value result = parser.GetResult();
			printf("Result=%s\n", result.ToString().c_str());
		}
		else
			printf("%s\n", parser.GetErrorMsg().c_str());

		ResourceLoader::Write("TestCopy.txt", t);
		DPTR(t)->DecRefCounter((DeltaValue*) 0);
	}
	
	ResourceLoader::SingletonDestroy();
	VirtualMachinePackage::CleanUp();
	UtilPackage::CleanUp();
	dclose();
}

