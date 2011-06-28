#include "ReferenceTranslation.h"
#include <cstdlib>
#include <iostream>

ReferenceTranslation *ReferenceTranslation::instance;

//At the Initialization add the predefined entities.
ReferenceTranslation::ReferenceTranslation(void)
{
	ReferenceMap["amp"]		= "&";
	ReferenceMap["lt"]		= "<";
	ReferenceMap["gt"]		= ">";
	ReferenceMap["apos"]	= "'";
	ReferenceMap["quot"]	= "\"";
}

//Clear the maps at clean up, so they are ready for initialization.
ReferenceTranslation::~ReferenceTranslation(void)
{
	ReferenceMap.erase( ReferenceMap.begin(),
						ReferenceMap.end() );
	PEReferenceMap.erase( PEReferenceMap.begin(),
						  PEReferenceMap.end() );
}

//Substitute all the References and PEReferences found in value before add it.
void ReferenceTranslation::AddReference(std::string const& index,
										std::string value)
{
	ReferenceMap[index]		= *SubstitutePEReferences(SubstituteReferences(&value));
}

//Substitute all the References and PEReferences found in value before add it.
void ReferenceTranslation::AddPEReference(std::string const& index,
										  std::string value)
{
	PEReferenceMap[index]	= *SubstitutePEReferences(SubstituteReferences(&value));
}

std::string* ReferenceTranslation::PERefValue(std::string const& index)
{
	return &(PEReferenceMap[index]);
}

std::string* ReferenceTranslation::EntityRefValue(std::string const& index)
{
	return &(ReferenceMap[index]);
}

//Take the integer representation, and create the corresponding character
//The function returns string(instead of char) for compatibility.
std::string* ReferenceTranslation::CharRefValue(std::string const& str)
{
	int charvalue;
	std::string *ret_string = DNEW(std::string);
	if (str[1] == 'x') {
		charvalue = TransformHexStrToInt( str.substr(2) );
	} else {
		charvalue = TransformStrToInt( str.substr(1) );
	}
	if (charvalue < 256) {
		(*ret_string) += (char)(charvalue);
	}
	return ret_string;
}

int ReferenceTranslation::TransformStrToInt(std::string const& str)
{
	char * pEnd;
	return strtol(str.c_str(), &pEnd, 10);
}

int ReferenceTranslation::TransformHexStrToInt(std::string const& str)
{
	char * pEnd;
	return strtol(str.c_str(), &pEnd, 16);
}

//Get a string as an input, and returns the string with replaced all the References(CharRef & EntityRef).
std::string* ReferenceTranslation::SubstituteReferences(std::string* str)
{
	std::string::size_type amp_pos = 0;
	std::string::size_type semicol_pos = 0;
	while ((amp_pos = str->find_first_of('&'), amp_pos) != std::string::npos) {
		if ((semicol_pos = str->find_first_of(';', amp_pos)) != std::string::npos) {
			if ((*str)[amp_pos+1] == '#') {
				(*str) = str->replace(amp_pos, semicol_pos - amp_pos + 1,
					*CharRefValue(str->substr(amp_pos + 1, semicol_pos - amp_pos - 1)) );
			} else {
				(*str) = str->replace(amp_pos, semicol_pos - amp_pos + 1,
					*EntityRefValue(str->substr(amp_pos + 1, semicol_pos - amp_pos - 1)) );
			}
		} else {
			break;
		}
		amp_pos++;
	}
	return str;
}

//Get a string as an input, and returns the string with replaced all the PEReferences.
std::string* ReferenceTranslation::SubstitutePEReferences(std::string *str)
{
	std::string::size_type percent_pos = 0;
	std::string::size_type semicol_pos = 0;
	while ((percent_pos = str->find_first_of('%')) != std::string::npos) {
		semicol_pos = str->find_first_of(';', percent_pos);
		(*str) = str->replace(percent_pos, semicol_pos - percent_pos + 1,
			*PERefValue(str->substr(percent_pos + 1, semicol_pos - percent_pos - 1)) );
	}
	return str;
}

std::string* ReferenceTranslation::RemoveIllegalXmlCharacters(const std::string *str)
{
	std::string::size_type amp_pos = 0, lt_pos = 0, gt_pos = 0, apos_pos = 0, quot_pos = 0;
	std::string *ret_str = new std::string(*str);
	while ((amp_pos = ret_str->find('&', amp_pos)) != std::string::npos) {
		(*ret_str) = ret_str->replace(amp_pos, 1, "&amp;");
		amp_pos++;
	}
	while ((lt_pos = ret_str->find('<', lt_pos)) != std::string::npos) {
		(*ret_str) = ret_str->replace(lt_pos, 1, "&lt;");
		lt_pos++;
	}
	while ((gt_pos = ret_str->find('>', gt_pos)) != std::string::npos) {
		(*ret_str) = ret_str->replace(gt_pos, 1, "&gt;");
		gt_pos++;
	}
	while ((apos_pos = ret_str->find('\'', apos_pos)) != std::string::npos) {
		(*ret_str) = ret_str->replace(apos_pos, 1, "&apos;");
		apos_pos++;
	}
	while ((quot_pos = ret_str->find('"', quot_pos)) != std::string::npos) {
		(*ret_str) = ret_str->replace(quot_pos, 1, "&quot;");
		quot_pos++;
	}
	return ret_str;
}
