/*
 * SparrowFileTextSource.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */

#include <stdio.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <algorithm>


#include "Call.h"
#include "Common.h"
#include "Streams.h"
#include "Component.h"
#include "SparrowFileTextSource.h"
#include "ComponentHandle.h"
#include "ComponentRegistry.h"
#include "DockableComponent.h"

using namespace frep;

namespace ide {
	//-----------------------------------------------------------------------
	
	SparrowFileTextSource::SparrowFileTextSource(void) 
	:	TextSource(),
		editor(static_cast<Component *>(0)),
		indexVec(0)
	{
		currPos.row = 1;
		currPos.col = 1;
	}
		
	//-----------------------------------------------------------------------
	
	SparrowFileTextSource::SparrowFileTextSource(const std::string & path_) 
	:	TextSource(),
		editor(static_cast<Component *>(0)),
		indexVec(0)
	{ 
		path = path_; 
		currPos.row = 1;
		currPos.col = 1;
	}

	//-----------------------------------------------------------------------
	
	SparrowFileTextSource::~SparrowFileTextSource(void)
		{ ClearAllContainers(); }

	//-----------------------------------------------------------------------
	
	static bool HasEditor(const std::string& uri){
		DockableComponent::EnsureVisibility("EditorManager");
		return Call<const Handle& (const String& uri)>
					("FindAndReplace", "EditorManager", "GetEditor")
					(util::std2str(uri));
	}

	//-----------------------------------------------------------------------

	static Component *	NewEditor(const std::string& uri) {
		DockableComponent::EnsureVisibility("EditorManager");
		const Handle& editor2 = Call<const Handle& (void)>("FindAndReplace", "EditorManager", "NewEditor")();
		Call<void (const String& uri)>("FindAndReplace", editor2, "Open")( util::std2str(uri));
		return editor2.Resolve();
	}

	//-----------------------------------------------------------------------

	void SparrowFileTextSource::ClearAllContainers(void) {
		vec.clear();
		markPos.clear();
		lineHolder.clear();
		return;
	}

	//-----------------------------------------------------------------------
	bool SparrowFileTextSource::Open (void){
		std::ifstream file(path.c_str());

		if (file.is_open()) {
			indexVec	= 0;
			currPos.row	= 1;
			currPos.col	= 0;
				

			std::string lineText("");
			unsigned lineCnt = 1;
			for (unsigned charCnt = 0; file.good(); charCnt++) {
				char c = file.get();
				
				if ( c != EOF ) {
					if (c == '\n') {
						lineText.append(1, (char)13);	//We must put an extra character 13,
						vec.push_back((char)13);		//for compatibility with editorm if we find \n.
					}
					lineText.append(1, c);
				}
				vec.push_back(c);

				if ( c == '\n' || c == EOF){
					lineHolder[lineCnt++] = lineText;
					lineText.clear();
				}
			}
			
			//
			//file.unsetf(std::ios::skipws);  //Turn of white space skipping
			//std::copy(
			//	std::istream_iterator<char>(file),
			//	std::istream_iterator<char>(),
			//	std::back_inserter(vec)
			//);
			//
			
			file.close();
			sizeOfVec = (unsigned)vec.size();
			return true;
		}
		return false;
	}
	
	//-----------------------------------------------------------------------
	
	bool SparrowFileTextSource::Close (void){
		indexVec	= 0;
		currPos.row	= 1;
		currPos.col	= 0;
		
		vec.clear();
		markPos.clear();
		return true;
	}

	//-----------------------------------------------------------------------
	
	bool SparrowFileTextSource::IsEof(void) const{
		assert(indexVec <= sizeOfVec);
		return (indexVec == sizeOfVec);
	}

	//-----------------------------------------------------------------------
	
	char SparrowFileTextSource::GetCurr (void) {
		if ( indexVec < vec.size() ){
			char c = vec[indexVec];

			if ( c == '\n' ){
				currPos.row++;
			}
			currPos.col = indexVec++;		// Sto sparrow metrame xaraktires apo thn arxh tou koimenou mono
			return c;
		}
		return EOF; 
	}	

	//-----------------------------------------------------------------------
	
	char SparrowFileTextSource::GetNext() {
		if ( indexVec < sizeOfVec -1 ){	//-1 ari8mish apo to 0-sizeVec
			char c = vec[++indexVec];
			if ( vec[indexVec-1] == '\n' ){	//an autos pou umatsan \n tote au3anoume thn shra
				currPos.row++;
			}
			currPos.col = indexVec;		// Sto sparrow metrame xaraktires apo thn arxh tou koimenou mono
			return c;
		}
		return EOF; 
	}

	//-----------------------------------------------------------------------

	bool SparrowFileTextSource::Unput(void){
		if (indexVec > 0){
			currPos.col = --indexVec - 1;		// Sto sparrow metrame xaraktires apo thn arxh tou koimenou mono
			if ( vec[currPos.col] == '\n' ){
				currPos.row--;
			}
			return true;
		}
		return false; 
	}

	//-----------------------------------------------------------------------

	bool SparrowFileTextSource::Retract(unsigned marker){
		if( markPos.find(marker) != markPos.end() ){
			indexVec	= marker;
			currPos.row	= markPos[marker].row;
			currPos.col	= markPos[marker].col;
			Unmark(marker);
			return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------

	unsigned SparrowFileTextSource::Mark (void) {
		markPos[indexVec].row = currPos.row;
		markPos[indexVec].col = currPos.col;
		return indexVec;
	}

	//-----------------------------------------------------------------------
	bool SparrowFileTextSource::Unmark (unsigned marked)
		{ return markPos.erase(marked) == 1; }

	//-----------------------------------------------------------------------
	
	void SparrowFileTextSource::UnmarkAll (void) 
		{ markPos.clear(); }

	//-----------------------------------------------------------------------
	const char SparrowFileTextSource::GetSpecificChar (unsigned c) const 
		{ return ((c >= sizeOfVec))? EOF: vec[c]; }

	//-----------------------------------------------------------------------
	
	const std::string SparrowFileTextSource::GetLineText (const int line) const {
		std::map<unsigned, std::string>::const_iterator i;
		if ( (i = lineHolder.find(line)) != lineHolder.end() )
			return i->second;
		return "";
	}

	//-----------------------------------------------------------------------
	
	const frep::TextSource::Pos SparrowFileTextSource::GetCurrentFilePos (void) { 
		if (editor) {
			currPos.col = Call<int (void)>("FindAndReplace", editor, "GetCurrentPos")();
			currPos.row = Call<int (void)>("FindAndReplace", editor, "GetCurrentLine")();
		}
		return currPos; 
	}

	//-----------------------------------------------------------------------
	
	bool SparrowFileTextSource::IsFocused (void) { 
		if (editor) {
		//if ( !HasEditor(path) ) editor = NewEditor(path);
			const Handle& focused = Call<const Handle& (void)>("FindAndReplace", "EditorManager", "GetFocusedEditor")();
			const String& focusedEditorURI = Call<const String& (void)>("FindAndReplace", focused, "GetURI")();
			if ( !util::str2std(focusedEditorURI).compare(path))
				return true;
		}
		return false;
	}

	//-----------------------------------------------------------------------
	
	void SparrowFileTextSource::Focus (void) { 
		if ( !HasEditor(path) ) 
			editor = NewEditor(path);
		Call<void (Handle)>("FindAndReplace", "EditorManager", "FocusEditor")(editor);
		editor->Focus();
	}

	//-----------------------------------------------------------------------
	
	void SparrowFileTextSource::Bookmark(const unsigned row, const unsigned col) {
		if ( !HasEditor(path) ) 
			editor = NewEditor(path);
		if (!Call<bool (int)>("FindAndReplace", editor, "HasBookmark")(row))
			Call<void (int)>("FindAndReplace", editor, "InsertBookmark")(row);
	}

	//-----------------------------------------------------------------------
	void SparrowFileTextSource::Select( const int row1, const int col1, const int row2, const int col2) {
		if ( !HasEditor(path) ) 
			editor = NewEditor(path);
		Call<void (Handle)>("FindAndReplace", "EditorManager", "FocusEditor")(editor);
		Call<void  (int, int)>("FindAndReplace", editor, "SelectRange")(col1, col2+1);	//+1 magic number is like the 
																						//.end() of iterator.
	}



	//-----------------------------------------------------------------------
	std::string	SparrowFileTextSource::Replace (	
		const int row1, 
		const int col1, 
		const int row2, 
		const int col2, 
		const std::string str
	) { 
		if ( !HasEditor(path) ) 
			editor = NewEditor(path);

		Call<void (Handle)>("FindAndReplace", "EditorManager", "FocusEditor")(editor);
		Call<void  (int, int)>("FindAndReplace", editor, "SelectRange")(col1, col2+1);	//+1 magic number is like the 
																						//.end() of iterator.
		
		//to string pou 8a alax8ei
		std::string toChange = util::str2std( Call<const String& (void)>("FindAndReplace", editor, "GetSelectedText")() );
		Call<void (const String&)>("FindAndReplace", editor, "ReplaceSelection")(util::std2str(str));
		
		//update the contents of the vec.
		ClearAllContainers();
		std::string text = util::str2std(Call<const String (void)>("FindAndReplace", editor, "GetText")());
		vec.reserve(text.size()+1);
		Open();
		return toChange;
	}

}	//namespace ide

