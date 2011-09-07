/**
 *	CopyAbsoluteReferenceOption.h
 *
 *	Copy object's absolute referenfe to clibolrd
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	July 2010
 */

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