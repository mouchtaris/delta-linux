/*
 * FileTextSource.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@csd.uoc.gr	
 */
//TODO: na dior8wsw to unput wte na ananeonei thn pos 

#include <stdio.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "FileTextSource.h"

using namespace frep;


FileTextSource::FileTextSource(void){
	E_O_F			= false;
	currPos			= 0;
	position.row	= 1;
	position.col	= 1;
}

FileTextSource::~FileTextSource(void){
	vec.clear();
	markPos.clear();
}



//-----------------------------------------------------------------------
bool FileTextSource::Open (void){
	std::ifstream file(path.c_str());

	if (file.is_open()) {
		currPos			= 0;
		position.row	= 1;
		position.col	= 1;
		file.unsetf(std::ios::skipws);  //Turn of white space skipping
		
		std::copy(
			std::istream_iterator<char>(file),
			std::istream_iterator<char>(),
			std::back_inserter(vec)
		);
		
		file.close();
		return true;
	}
	return false;
}




//-----------------------------------------------------------------------
bool FileTextSource::Close (void){
	E_O_F			= false;
	currPos			= 0;
	position.row	= 1;
	position.col	= 1;
	
	vec.clear();
	markPos.clear();
	return true;
}



//-----------------------------------------------------------------------
bool FileTextSource::IsEof(void) const{
	return E_O_F;
}



//-----------------------------------------------------------------------
char FileTextSource::GetNext(){
	
	if( currPos < vec.size() ){
		char c = vec[currPos++];
		
		if( c == '\n'){
			position.row++;
			position.col = 1;
		}
		else { position.col++; }

		return c;
	}
	E_O_F = true;
	return EOF;
}




//-----------------------------------------------------------------------
bool FileTextSource::Unput(void){
	if( currPos > 0){
		if( (currPos < vec.size()) && (vec[currPos] == '\n') ){
			position.row++;
			//position.second Edw na to dior8wsw
		}
		else { position.col--; }
		currPos--;
	}
	return true;
}




//-----------------------------------------------------------------------
bool FileTextSource::Retract(unsigned marker){
	if( markPos.find(marker) != markPos.end() ){
		E_O_F			= false;		//mporei na eixame ftasei sto telos tou arxeiou
		currPos			= marker;
		position.row	= markPos[marker].row;
		position.col	= markPos[marker].col;
		Unmark(marker);
		return true;
	}
	return false;
}




//-----------------------------------------------------------------------
unsigned FileTextSource::Mark (void){
	markPos[currPos].row = position.row;
	markPos[currPos].col = position.col;
	return currPos;
}



//-----------------------------------------------------------------------
bool FileTextSource::Unmark (unsigned marked){
	if( markPos.erase(marked) )
		return true;
	else
		return false;
}



//-----------------------------------------------------------------------
void FileTextSource::UnmarkAll (void) {
	markPos.clear();
	return;
}



//-----------------------------------------------------------------------
const char FileTextSource::GetSpecificChar (unsigned c) const{
	return ((c >= vec.size()) || (c < 0))? EOF: vec[c];
}