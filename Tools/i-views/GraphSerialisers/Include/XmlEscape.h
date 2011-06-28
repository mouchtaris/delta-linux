#ifndef	XML_ESCAPE_H	
#define	XML_ESCAPE_H


#include <map>
#include <string>

namespace iviews {
class XmlEscape {
public:
	static void				Initialise (void);
	static void				CleanUp (void);
	static bool				IsInitialised (void);
	static bool				IsCleanedIp (void);
	static std::string		StringToXMLString (const std::string & str);

	
private:
	typedef std::map<char, std::string>	XMLEntities;

	static bool				isInitialised;
	static bool				isCleanedUp;
	static XMLEntities		xmlEntities;
};


}	//namespace iviews

#endif	//XML_ESCAPE_H