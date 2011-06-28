// DeltaCompilerInit.cpp
// Global program start-up  and ending actions.
// ScriptFigher Project.
// A. Savidis, May 2003.
//

#ifndef	DELTACOMPILERINIT_H
#define	DELTACOMPILERINIT_H

#include "CompilerAPI.h"
#include "InterCode.h"
#include "Symbol.h"
#include "TargetCode.h"
#include "LocalDataHandling.h"
#include "Optimizer.h"
#include "DebugNamingForStaticVars.h"
#include "AutoCollection.h"
#include "DeltaCompilerInit.h"
#include "LibraryNamespaceHolder.h"
#include "FunctionReturnTypesManager.h"

static util_ui32 initCounter = 0;

namespace DeltaCompilerInit {

	DCOMPLIB_FUNC void Initialise (void) {
		if (!initCounter++) {
			DeltaCompiler::SingletonCreate();
			DeltaSymbolTable::SingletonCreate();
			Optimizer::SingletonCreate();
			DeltaQuadManager::SingletonCreate();
			AutoCollector::SingletonCreate();
			DeltaCodeGenerator::SingletonCreate();
			LocalDataHandler::SingletonCreate();
			DebugNamingForStaticVars::SingletonCreate();
			DeltaLibraryNamespaceHolder::SingletonCreate();
			DeltaFunctionReturnTypesManager::SingletonCreate();
		}
	}

	DCOMPLIB_FUNC void CleanUp (void) {
		DASSERT(initCounter);
		if (!--initCounter) {
			DeltaCompiler::SingletonDestroy();
			DeltaSymbolTable::SingletonDestroy();
			DeltaCodeGenerator::SingletonDestroy();
			AutoCollector::SingletonDestroy();
			Optimizer::SingletonDestroy();
			DeltaQuadManager::SingletonDestroy();
			LocalDataHandler::SingletonDestroy();
			DebugNamingForStaticVars::SingletonDestroy();
			DeltaLibraryNamespaceHolder::SingletonDestroy();
			DeltaFunctionReturnTypesManager::SingletonDestroy();
		}
	}
}

#endif	// Do not add stuff beyond this point.