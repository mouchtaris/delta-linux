#include "SparrowSelectionTextSource.h"

#include <assert.h>

#include "Call.h"
#include "Common.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "ComponentRegistry.h"


namespace ide {

	//-----------------------------------------------------------------------

	SparrowSelectionTextSource::SparrowSelectionTextSource(	
		Component * c, 
		std::string str)
	: SparrowEditorTextSource(c, str) {		
		String selectedText =	Call<const String (void)>
								(editor, editor, "GetSelectedText")
								();

		selectedTextEnd		=	Call<int (void)>	
								(editor, editor, "GetCurrentPos")
								();

		sizeOfText			=	selectedTextEnd;
		
		selectedTextStart	=	selectedTextEnd - selectedText.size();
		
		if(selectedTextStart > selectedTextEnd || selectedTextStart < 0) {
			int tmp				= selectedTextStart;
			selectedTextStart	= selectedTextEnd;
			selectedTextEnd		= tmp > 0 ? tmp : -tmp;
			sizeOfText			= selectedTextEnd;
		}

		indexPos			= 
		currPos.col			=	selectedTextStart;
		currPos.row			=	Call<int (int)>	
								(editor, editor, "GetLineFromPos")
								(selectedTextStart);
	}

	//-----------------------------------------------------------------------

	SparrowSelectionTextSource::~SparrowSelectionTextSource(void)
	{}


	//-----------------------------------------------------------------------

	bool SparrowSelectionTextSource::IsEof(void) const { 
		assert((indexPos >= selectedTextStart) && (indexPos <= selectedTextEnd));
		return (indexPos == selectedTextEnd); 
	}


	//-----------------------------------------------------------------------

	bool SparrowSelectionTextSource::Unput(void) { 
		if (indexPos > selectedTextStart){
			currPos.col = --indexPos - 1;		// Sto sparrow metrame xaraktires apo thn arxh tou koimenou mono
			if ( text[currPos.col] == '\n' )
				currPos.row--;
			return true;
		}
		return false; 
	}

}