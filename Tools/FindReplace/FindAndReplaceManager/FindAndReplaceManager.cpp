/*
 * FindAndRplaceManager.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */
#include <stack>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <windows.h>
#include <iostream>


#include "RegExpr.h"
#include "KeyLogger.h"
#include "MyTemplates.h"
#include "FindAndReplaceManager.h"


//-----------------------------------------------------------------------
#define FAIL(X)						FindAndReplaceManager::SetError(X); return false
#define CREATE_REPLACMENT_STR(X)	if (regEx) { if (!CreateReplacmentString((X)) ){ return false; } }\
									else { with2 = with; } 

namespace frep {

bool					FindAndReplaceManager::prevIsSearchUp;
bool					FindAndReplaceManager::regEx;
bool					FindAndReplaceManager::hasError;
char *					FindAndReplaceManager::errorMsg;
std::string				FindAndReplaceManager::what;
std::string				FindAndReplaceManager::with;
std::string				FindAndReplaceManager::what2;
std::string				FindAndReplaceManager::with2;
DFAMatching				FindAndReplaceManager::dfaMatching;
BasicStringMatching		FindAndReplaceManager::strMatching;
RegExStringReplace		FindAndReplaceManager::replaceStringCreator;
TextSource::Block		FindAndReplaceManager::deletedBlock;
TextSource::Block		FindAndReplaceManager::selectedBlock;
TextSource::Block		FindAndReplaceManager::currBlock;


std::list<TextSource *>				FindAndReplaceManager::stream;
FindAndReplaceManager::Results		FindAndReplaceManager::results;
FindAndReplaceManager::FindSettings	FindAndReplaceManager::settings;






//******************************************************************************************
//		Nonmembers functions

//-----------------------------------------------------------------------

bool IsGreatEqual(const TextSource::Pos & currPos, const TextSource::Pos & fileStartPos){
	if( currPos.col >= fileStartPos.col )	//g.col <= s.col
		return true;
	return false;
}

//-----------------------------------------------------------------------

bool IsLessEqual(const TextSource::Pos & currPos, const TextSource::Pos & fileCurrPos){
	if( currPos.col < fileCurrPos.col )	//g.col <= s.col
		return true;
	return false;
}





//******************************************************************************************
//		private functions

//-----------------------------------------------------------------------

void FindAndReplaceManager::Clear(void){
	results.clear();
	hasError = false;
	if (errorMsg) {
		delete []errorMsg;
		errorMsg = (char *)0;
	}
}

//-----------------------------------------------------------------------

void FindAndReplaceManager::SetFindSettings(void){
	dfaMatching.SetFindSetting(settings.isCaseSensitive, settings.wholeWord);
	strMatching.SetFindSetting(settings.isCaseSensitive, settings.wholeWord);
	if( !settings.isCaseSensitive )	//To string isodou einai plewn se lower case mode 
		what = ToLowerCopy(what);
}

//-----------------------------------------------------------------------

void FindAndReplaceManager::SetError(const char * msg){
	hasError = true;
	errorMsg = _strdup(msg);
}

//-----------------------------------------------------------------------
//	An epistreyei false uparxei kapoio la8os
bool FindAndReplaceManager::RunAlgorithm (StringMatching & strMatch, TextSource * s){
	Block block;
	
	block.path = s->GetPath();
	if ( strMatch.Run(what, *s, block) ) { 
		results.push_back(block); 
	}
	else {
		if (strMatch.HasError()){
			SetError(strMatch.GetError().c_str());
			strMatch.Clear();
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------
//	Diatrexei olh thn lista me ta input stream kai gia to ka8e ena 
//	arxizei na efarmozei to DFA apo thn arxh tou automatou mexrh to telos
bool FindAndReplaceManager::DoSeartch (StringMatching & strMatch){
	assert( !results.size() );
	Files::iterator s = stream.begin();
	for(;  s != stream.end() && !HasError(); s++ )	{ //Diatrexoume olh thn lista me ta stream
		assert(*s);
		if ( (*s)->Open() ){
			for (char c = 's'; !(*s)->IsEof() && !HasError() && c != EOF; c = (*s)->GetNext()) {
				if (!RunAlgorithm(strMatch, (*s))) { return false; }
			}
			(*s)->Close();
		}//if
	}//for

	return results.size() != 0;
}



//***********************************************
//	The next 3 functions use in FindNext  
//	public function
/*
//-----------------------------------------------------------------------
TextSource::Block & FindAndReplaceManager::GetNext (void){
	assert(results.size() && currFind < results.size());
	currFind = ++currFind % results.size();   
	return results.at(currFind);
}



//-----------------------------------------------------------------------
TextSource::Block & FindAndReplaceManager::GetPrev (void){
	assert(results.size() && currFind < results.size() );
	if (!currFind)
		currFind = (unsigned)(results.size() - 1); //telos lista
	else
		currFind = --currFind % results.size();
	return results.at(currFind);
}
*/

///----------------------------------------------------------------------

//-----------------------------------------------------------------------

TextSource::Block &  FindAndReplaceManager::GetNext(void) {
	assert(results.size() > 0);
	bool a = false;

	for( Results::iterator block = results.begin(); block != results.end(); ++block) {
		if (block->file->IsFocused()) {
			a = true;
			if (!IsLessEqual(block->start, block->file->GetCurrentFilePos()))
				return currBlock = (*block);
		}
		else if(a)
			return currBlock = (*block);
	}
	return currBlock = *results.begin();
}

//-----------------------------------------------------------------------

TextSource::Block & FindAndReplaceManager::GetPrev(void) {
	assert(results.size() > 0);
	bool a = false;

	Results::reverse_iterator block;

	for (block = results.rbegin(); block != results.rend(); ++block) {
		if (block->file->IsFocused()) {
			assert (block->file->GetPath() == block->path);
			a = true;
			//to +1 epidei exoume diafora sto pos parousiazei ta dedomena o sentila kai pos ta pernw exw egw
			if (!IsGreatEqual(TextSource::Pos(block->end.row, block->end.col+1), block->file->GetCurrentFilePos()))
				return currBlock = (*block);
		}
		else if(a)
			return currBlock = (*block);
	}
	return currBlock = results.back();
}

///-------------------------------------------------------------------

//-----------------------------------------------------------------------

void FindAndReplaceManager::UpdateResultsPosition (const Block & block){
	int shiftRight = (int)(with2.size() - what2.size());
		
	for (Results::iterator i = results.begin(); i != results.end(); ++i){
		if (((*i).path == block.path) && IsLessEqual(block.start,(*i).start)){
			(*i).start.col	= (*i).start.col + shiftRight;
			(*i).end.col	= (*i).end.col + shiftRight;
		}
	}
	return;
}

//-----------------------------------------------------------------------

bool FindAndReplaceManager::CreateReplacmentString (const Block & b) {
	if ( b.hasTags )
		CopyMap<unsigned, std::string>(b.tags, TagExpr::allTags);

	if ( !replaceStringCreator.CreateReplacmentString(with) && replaceStringCreator.HasError() )
		{ FAIL(replaceStringCreator.GetError().c_str()); }
	else 
		with2 = replaceStringCreator.GetReplaceString();	

	return true;
}













//******************************************************************************************
//		public functions

//-----------------------------------------------------------------------
void FindAndReplaceManager::Initialise (void){
	regEx		= false;
	what		= "";
	with		= "";
	hasError	= false;
	errorMsg	= (char *)0;
//	currFind	= 0;
	prevIsSearchUp = false;
	
	settings.wholeWord			= false;
	settings.searchUp			= false;
	settings.isCaseSensitive	= false;
	results.reserve(100);
	return;
}

//-----------------------------------------------------------------------

void FindAndReplaceManager::CleanUp (void){
	Clear();
	return;
}

//-----------------------------------------------------------------------

bool FindAndReplaceManager::Find (void){
	Clear();
	SetFindSettings();

	assert( !results.size() );	
	return (regEx? DoSeartch(dfaMatching) : DoSeartch(strMatching));
}

//-----------------------------------------------------------------------

void FindAndReplaceManager::SetFindParam(const std::string _what, const bool _reg){
	what	= _what;
	regEx	= _reg;
	return;
}

//-----------------------------------------------------------------------

void FindAndReplaceManager::SetReplaceParam(
		const std::string & _what, 
		const std::string & _with, 
		const bool _reg
	) {
	what	= _what;
	with	= _with;
	regEx	= _reg;
	return;
}

//-----------------------------------------------------------------------

void FindAndReplaceManager::SelectText (const Block & b){
	assert(stream.size());
	selectedBlock	= b;
	b.file->Select(b.start.row, b.start.col, b.end.row, b.end.col);
}

//-----------------------------------------------------------------------

TextSource::Block & FindAndReplaceManager::FindNext(void){
	assert(results.size());
	
	if (settings.searchUp)
		return GetPrev();
	else
		return GetNext();
}

//-----------------------------------------------------------------------

void FindAndReplaceManager::BookmarkAll (void){
	for (Results::iterator block = results.begin(); block != results.end(); ++block)
			block->file->Bookmark(block->start.row, block->start.col);
}



//-----------------------------------------------------------------------
//public function. This functions doing the replace
/*bool FindAndReplaceManager::Replace (void) {
	//bool re = true;
	unsigned oldCurrFind = 0;
	Results::iterator ite;

	if ( !results.size() ) return false;
	
	Block area = GetCurrentBlock();

	CREATE_REPLACMENT_STR(area)
	
	deletedBlock =  area;
	what2		 = area.file->Replace(area.start.row, area.start.col, area.end.row, area.end.col, with2);			
	
	if ( (ite = find(results.begin(), results.end(), area)) != results.end() ){
		results.erase( ite );
//		if ( currFind >= results.size())
//			currFind = 0;
		UpdateResultsPosition(area);
	}
	else
		assert(0);	//must find the area into the results.

	if (HasResults())
		SelectText(currBlock);
		//SelectText(GetCurrentBlock());

	return true;
}*/

//-----------------------------------------------------------------------

bool FindAndReplaceManager::Replace (void) {
	if ( !results.size() ) return false;
	
	CREATE_REPLACMENT_STR(currBlock)
	
	deletedBlock = currBlock;
	what2		 = currBlock.file->Replace(currBlock.start.row, currBlock.start.col, currBlock.end.row, currBlock.end.col, with2);			
	
	Results::iterator ite;
	if ( (ite = find(results.begin(), results.end(), currBlock)) != results.end() ){
		results.erase( ite );
		UpdateResultsPosition(currBlock);
		
		if (results.size() != 0)
			SelectText(GetNext());
	}
	else
		assert(0);	//currBock must be in results

	return true;
}

//-----------------------------------------------------------------------
//Replace all the blocks 
bool FindAndReplaceManager::ReplaceAll (void){
	for (Results::iterator block = results.begin(); block != results.end(); ++block ){
		CREATE_REPLACMENT_STR((*block));
		what2 = block->file->Replace(block->start.row, block->start.col, block->end.row, block->end.col, with2);
		UpdateResultsPosition((*block));
	}
	results.clear();
	return true;
}

//-----------------------------------------------------------------------

bool FindAndReplaceManager::HasResults (void) 
	{ return results.size() != 0; }

//-----------------------------------------------------------------------

const std::string FindAndReplaceManager::WriteResultsToOutput (void) {
	std::string msg("");

	for (Results::iterator block = results.begin(); block != results.end(); ++block){
		msg.append(block->path + "(line:" + string_cast(block->start.row) + " - cols:");
		msg.append(string_cast(block->start.col) + ", " + string_cast(block->end.col));
		msg.append(") " + block->file->GetLineText(block->start.row) + "\n");
	}
	return msg;
}

//-----------------------------------------------------------------------

void FindAndReplaceManager::WriteResults( Block & r, FILE *fp){
	fprintf(fp, "File Name: %s\n", r.path.c_str());
	fprintf(fp, "Start	Row: %d\n", r.start.row);
	fprintf(fp, "Start	Col: %d\n", r.start.col);
	fprintf(fp, "End	Row: %d\n", r.end.row);
	fprintf(fp, "End	Col: %d\n", r.end.col);
	fprintf(fp, "Have tag: %s\n", (r.hasTags?"Yes":"No") );

	if( r.hasTags ){
		for(Block::BlockTagMap::iterator i = r.tags.begin(); i != r.tags.end(); ++i){
			fprintf(fp, "Tag Number: %d\n", i->first);
			fprintf(fp, "Tag string: >%s<\n", i->second.c_str() );
		}//end of for
	}

	fprintf(fp, "\n\n\n");
	return;
}

//-----------------------------------------------------------------------

void FindAndReplaceManager::DumpResults(FILE *fp){

	for (Results::iterator r = results.begin(); r != results.end(); r++) {
		WriteResults( *r, fp );
	}
	return;
}

//-----------------------------------------------------------------------

bool FindAndReplaceManager::IsRightDeletion (const std::string & uri, const int pos, const int length) {
	unsigned deletionLenth = deletedBlock.end.col - deletedBlock.start.col;
	if ( (pos == deletedBlock.start.col) && (deletionLenth == length) && BlockExists(uri))
		return true;
	return false;
}

//-----------------------------------------------------------------------

TextSource::Block * FindAndReplaceManager::BlockExists (const std::string & uri) {
	Results::iterator i;
	for(i = results.begin(); i != results.end(); ++i) {
		if ((*i).path == uri)// {
			break;
			//currBlock = selectedBlock	= (*i);
		//}
	}
	return ((i!=results.end())? &(*i): static_cast<Block *>(0));
}

//-----------------------------------------------------------------------

TextSource::Block * FindAndReplaceManager::BlockExists (
						const std::string & uri, 
						const int startPos, 
						const int endPos
	) {
	unsigned index = 0;
	Results::iterator i;
	for(i = results.begin(); i != results.end(); ++i, ++index) {
		if ((*i).path		== uri		&&
			(*i).start.col	== startPos &&
			(*i).end.col	== endPos )
		{
			break;
		}
	}
	return ((i!=results.end())? &(*i): static_cast<Block *>(0));
}

//-----------------------------------------------------------------------

bool FindAndReplaceManager::IsRightSelection (
		const std::string & uri, 
		const int startPos, 
		const int endPos
	)  {
	if (selectedBlock.start.col == startPos	&&
		selectedBlock.end.col	== endPos	&&
		selectedBlock.path		== uri
	) { return true; }
	return false;
}

} //namspace frep
