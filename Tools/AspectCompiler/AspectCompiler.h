// AspectCompiler.h
// Aspects for Delta scripts based on AST Transformations
// Y.Lilis, July 2012.
//

#ifndef	ASPECTCOMPILER_H
#define	ASPECTCOMPILER_H

#include <string>
#include <list>

#include "ucallbacks.h"
#include "DeltaMetaCompiler.h"

#define ASPECT_TRANSFORMATION_FUNCNAME	"transform"

///////////////////////////////////////////////////////////////////

class AspectCompiler {
public:
	typedef DeltaMetaCompiler::LineMappings	LineMappings;

	typedef std::list<std::string> StringList;
	typedef ucallbackwithclosure<void (*)(const char*, void*)> ErrorCallback;
	typedef ucallbackwithclosure<void (*)(const std::string&, const std::string&, const LineMappings&, const AST::NodeToChainOfSourceLineOriginInfo&, bool, void*)> TransformationCallback;
	typedef ucallbackwithclosure<void (*)(AST::Node*, void*)> ParseCallback;

private:
	TreeNode*				tree;
	std::string				name;
	ErrorCallback			onError;
	TransformationCallback	onTransformation;
	ParseCallback			onParse;

	void Error (const std::string& msg);
public:
	void SetErrorCallback (ErrorCallback callback) { onError = callback; }
	ErrorCallback GetErrorCallback (void) const { return onError; }

	void SetTransformationCallback (TransformationCallback callback) { onTransformation = callback; }
	TransformationCallback GetTransformationCallback (void) const { return onTransformation; }

	void SetParseCallback (ParseCallback callback) { onParse = callback; }
	ParseCallback GetParseCallback (void) const { return onParse; }

	bool ParseFile (const std::string& path, const std::string& symbolic);
	bool ApplyTransformations (const StringList& aspects);

	AspectCompiler(void);
	~AspectCompiler();
};

#endif	// Do not add stuff beyond this point.