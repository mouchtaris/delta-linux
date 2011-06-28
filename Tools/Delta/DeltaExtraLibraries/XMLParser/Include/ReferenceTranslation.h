#ifndef	REFERENCETRANSLATION_H
#define	REFERENCETRANSLATION_H

#include <map>
#include <string>
#include "DDebug.h"

class ReferenceTranslation
{
private:
	std::map<std::string,
		std::string>				ReferenceMap;
	std::map<std::string,
		std::string>				PEReferenceMap;
	ReferenceTranslation(void);
	

	static ReferenceTranslation*	instance;

	int								TransformStrToInt ( std::string const& );
	int								TransformHexStrToInt ( std::string const& );
public:

	static ReferenceTranslation*	Singleton(void)
	{
		if (instance == 0)
			ReferenceTranslation::Initialize();
		return instance;
	}
	
	static void						Initialize(void)
	{
		instance = DNEW(ReferenceTranslation);
	}

	static void						CleanUp(void)
	{
		DDELETE(instance);
		instance = 0;
	}

	void							AddReference( std::string const&, std::string);
	void							AddPEReference( std::string const&, std::string);
	std::string*					PERefValue( std::string const&);
	std::string*					EntityRefValue( std::string const&);
	std::string*					CharRefValue( std::string const&);
	std::string*					SubstituteReferences( std::string* );
	std::string*					SubstitutePEReferences( std::string* );
	std::string*					RemoveIllegalXmlCharacters( const std::string* );
	~ReferenceTranslation(void);
};

#endif
