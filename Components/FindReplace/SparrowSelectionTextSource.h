#ifndef SPAROOW_SELECTION_TEXT_SOURCE_H
#define SPAROOW_SELECTION_TEXT_SOURCE_H

#include "common.h"
#include "Component.h"
#include "SparrowEditorTextSource.h"

//warning C4018: '>=' : signed/unsigned mismatch
#pragma warning(disable:4018)

namespace ide {

	class SparrowSelectionTextSource : public SparrowEditorTextSource {
	public:
		SparrowSelectionTextSource(Component *, std::string);
		virtual ~SparrowSelectionTextSource(void);
	
		virtual bool	IsEof(void) const;
		//virtual char	GetCurr (void);
		//virtual char	GetNext (void);
		virtual bool	Unput(void);

	private:
		int selectedTextStart, selectedTextEnd;
	};

}	//namespace ide

#endif	// SPAROOW_SELECTION_TEXT_SOURCE_H