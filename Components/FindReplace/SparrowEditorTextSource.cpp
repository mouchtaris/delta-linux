/*
 * SparrowEditorTextSource.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#include <assert.h>

#include "Call.h"
#include "Common.h"
#include "Streams.h"
#include "Component.h"
#include "ComponentHandle.h"
#include "ComponentRegistry.h"
#include "SparrowEditorTextSource.h"


namespace ide {

	//-----------------------------------------------------------------------

	SparrowEditorTextSource::SparrowEditorTextSource (	
		Component * editor_, 
		std::string path_
	) : frep::TextSource(),
		editor(editor_) 
	{
		path = path_;
		assert(editor && !editor->GetName().compare( wxT("Editor") ));
		indexPos	= 0;
		text		= util::str2std(Call<const String (void)>(editor, editor, "GetText")());
		currPos.row = 1;
		currPos.col = 0;
		sizeOfText	= text.size();
		
	}

	//-----------------------------------------------------------------------

	SparrowEditorTextSource::~SparrowEditorTextSource(void) 
		{	markPos.clear();	}

	//-----------------------------------------------------------------------
	// Auth 8a epistrefei panta true kai tipota alo.
	// Logos einai oti ta arxeia mou 8a einai idi anixta sto ide
	bool SparrowEditorTextSource::Open (void) 
		{ return true; }

	//-----------------------------------------------------------------------
	// Auth 8a epistrefei panta true kai tipota alo.
	// Logos einai oti ta arxeia mou 8a einai idi anixta sto ide
	bool SparrowEditorTextSource::Close (void) 
		{ return true; }

	//-----------------------------------------------------------------------
	bool SparrowEditorTextSource::IsEof(void) const { 
		assert(indexPos <= sizeOfText);
		return (indexPos == sizeOfText);
	}

	//-----------------------------------------------------------------------

	char SparrowEditorTextSource::GetCurr() {
		if ( indexPos < sizeOfText ){
			char c = text[indexPos];

			if (c == '\n')
				currPos.row++;
			currPos.col = indexPos++;		// Sto sparrow metrame xaraktires apo thn arxh tou koimenou mono
			return c;
		}
		return EOF; 
	}

	//-----------------------------------------------------------------------

	char SparrowEditorTextSource::GetNext() {
		if ( indexPos < sizeOfText -1  ){		//-1 ari8mish apo to 0-sizeVec
			char c = text[++indexPos];
			if ( text[indexPos-1] == '\n' )		//an autos pou umatsan \n tote au3anoume thn shra
				currPos.row++;
			currPos.col = indexPos;				// Sto sparrow metrame xaraktires apo thn arxh tou koimenou mono
			return c;
		}
		return EOF; 
	}

	//-----------------------------------------------------------------------

	bool SparrowEditorTextSource::Unput(void) { 
		if (indexPos > 0){
			currPos.col = --indexPos - 1;		// Sto sparrow metrame xaraktires apo thn arxh tou koimenou mono
			if ( text[currPos.col] == '\n' )
				currPos.row--;
			return true;
		}
		return false; 
	}

	//-----------------------------------------------------------------------

	unsigned SparrowEditorTextSource::Mark (void) { 
		markPos[indexPos].row = currPos.row;
		markPos[indexPos].col = currPos.col;
		return indexPos; 
	}

	//-----------------------------------------------------------------------

	bool SparrowEditorTextSource::Retract(unsigned marker) {
		if( markPos.find(marker) != markPos.end() ){
			indexPos	= marker;
			currPos.row	= markPos[marker].row;
			currPos.col	= markPos[marker].col;
			Unmark(marker);
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------
	
	bool SparrowEditorTextSource::Unmark (unsigned marked) 
		{  return markPos.erase(marked) == 1; }

	//-----------------------------------------------------------------------
	
	void SparrowEditorTextSource::UnmarkAll (void) 
		{ markPos.clear(); }

	//-----------------------------------------------------------------------
	void SparrowEditorTextSource::SetPath(const std::string & path_) 
		{ path = path_;	}

	//-----------------------------------------------------------------------

	bool SparrowEditorTextSource::IsFocused (void) {
		const Handle& focused = Call<const Handle& (void)>(editor, "EditorManager", "GetFocusedEditor")();
		const String& focusedEditorURI = Call<const String& (void)>(editor, focused, "GetURI")();
		if ( !util::str2std(focusedEditorURI).compare(path))
			return true;
		return false;
	}

	//-----------------------------------------------------------------------
	
	void SparrowEditorTextSource::Focus (void) {
		Call<void (Handle)>
		(editor, "EditorManager", "FocusEditor")
		(editor);
	}

	//-----------------------------------------------------------------------

	const frep::TextSource::Pos	SparrowEditorTextSource::GetPos (void) const 
		{ return currPos; }

	//-----------------------------------------------------------------------

	const frep::TextSource::Pos	SparrowEditorTextSource::GetCurrentFilePos (void) { 
		currFilePos.col =	Call<int (void)>
							(editor, editor, "GetCurrentPos")
							();
		
		currFilePos.row =	Call<int (void)>
							(editor, editor, "GetCurrentLine")
							();
		return currFilePos; 
	}

	//-----------------------------------------------------------------------

	const std::string SparrowEditorTextSource::GetPath(void) const 
		{ return path; }
		
	//-----------------------------------------------------------------------
	
	const std::string SparrowEditorTextSource::GetLineText (const int line) const {
		const String lineText =	Call<const String (int line)>
								(editor, editor, "GetLineText")
								(!line?0:line-1);	//8elei thn apolith 8esh mesa sto koimeno
		return util::str2std(lineText);
	}

	//-----------------------------------------------------------------------

	const char SparrowEditorTextSource::GetSpecificChar(unsigned c) const 
		{ return (c >= sizeOfText)? EOF: text[c]; }

	//-----------------------------------------------------------------------
	
	void SparrowEditorTextSource::Bookmark(const unsigned row, const unsigned col) {
		if (!Call<bool (int)>(editor, editor, "HasBookmark")(row))
			Call<void (int)>(editor, editor, "InsertBookmark")(row);
	}

	//-----------------------------------------------------------------------
	
	void SparrowEditorTextSource::Select( const int row1, const int col1, const int row2, const int col2) {
		Focus();
		Call<void  (int, int)>(editor, editor, "SelectRange")(col1, col2+1); //+1 magic number. Is like the 
																			//end() of interator.
	}

	//-----------------------------------------------------------------------
	
	std::string	SparrowEditorTextSource::Replace (	const int row1, 
													const int col1, 
													const int row2, 
													const int col2, 
													const std::string str) 
	{ 
		Focus();
		Call<void  (int, int)>(editor, editor, "SelectRange")(col1, col2+1);//+1 magic number is like the 
																			//.end() of interator.
		
		//to string pou 8a alax8ei
		std::string toChange = util::str2std( Call<const String& (void)>(editor, editor, "GetSelectedText")() );
		Call<void (const String&)>(editor, editor, "ReplaceSelection")(util::std2str(str));
		
		text		= util::str2std(Call<const String (void)>(editor, editor, "GetText")());
		sizeOfText	= text.size();
		indexPos	= 0;
		currPos.col = 0;
		currPos.row = 1;
		return toChange;
	}

} //namespace ide