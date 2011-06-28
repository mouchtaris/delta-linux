//july 2010
//copy object's absolute referenfe to clibolrd
#ifndef COPY_ABSOLUTE_REFRENCE_OPTION_H
#define	COPY_ABSOLUTE_REFRENCE_OPTION_H

#include "VertexRenderingInfo.h"
#include "VertexToDebugInfoMapping.h"

namespace iviews {

class CopyAbsoluteReferenceOption {
public:
	typedef graphs::VertexToDebugInfoMapping VertexToDebugInfoMapping;
	
	~CopyAbsoluteReferenceOption();
	CopyAbsoluteReferenceOption();
	void operator()(VertexRenderingInfo * v, VertexToDebugInfoMapping * mapping);
};

}

#endif	COPY_ABSOLUTE_REFRENCE_OPTION_H