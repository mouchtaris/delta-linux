// ShMemLibP.h
// Simple intra-process shared memory class for VMs.
// ScriptFigher Project.
// A. Savidis, December 1999.
//

#ifndef	SHMEMLIBP_H
#define	SHMEMLIBP_H

#include "DeltaValue.h"
#include <map>
#include <string>

#define	DELTA_STD_SHARED_OBJECT_DESC	"shared object"

class DVM_CLASS DeltaSharedMemory {
	private:
	typedef std::map<std::string, DeltaValue> Memory;
	static Memory* mem;

	public:
	static void	Write		(const std::string& s, const DeltaValue& val);
	static bool	Read		(const std::string& s, DeltaValue* val);
	static bool	Exists		(const std::string& s);
	static bool	Remove		(const std::string& s);

	static void Initialise	(void);
	static void CleanUp		(void);
};

#endif	// Do not add stuff beyond this point.
