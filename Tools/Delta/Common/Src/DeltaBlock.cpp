// DeltaBlock.cpp
// A. Savidis, Feburary 2005.
// Refactored July 2007, adding error propagation.
//

#include "DeltaBlock.h"
#include "uerrorclass.h"
#include "ufiles.h"
#include <algorithm> 
#include <functional>

////////////////////////////////////////////////////////

void DeltaDebugBlock::Write (GenericWriter& writer) const {
	writer.write((util_ui16) locals.size());
	writer.write(blockId);
	for (std::list<DeltaDebugLocal>::const_iterator i = locals.begin(); i != locals.end(); ++i)
		i->Write(writer);
}

////////////////////////////////////////////////////////

void DeltaDebugBlock::WriteText (FILE* fp) const {
	fprintf(fp,"\tBLOCKID %d, #LOCALS %d.\n", blockId, locals.size());

	std::for_each(
		locals.begin(),
		locals.end(),
		std::bind2nd(std::mem_fun_ref(&DeltaDebugLocal::WriteText), fp)
	);
}

////////////////////////////////////////////////////////

#define	ERROR_HANDLER(what, errclass) \
	uerror::GetSingleton().post##errclass( \
		"Loading (debug info) block: error in reading '%s'!", what \
	); goto FAIL; \

bool DeltaDebugBlock::Read (GenericReader& reader) {

	util_ui16 totalLocals;
	
	UCHECK_PRIMARY_ERROR(reader.read(&totalLocals), "total locals");
	UCHECK_PRIMARY_ERROR(reader.read(&blockId), "block id");

	for (util_ui16 i = 0; i < totalLocals; ++i) {
		DeltaDebugLocal local;
		UCHECK_DOMINO_ERROR(local.Read(reader), uconstructstr("local #%d", i));
		Add(local);
		offsets.push_back(local.GetOffset());
	}

	return true;
	FAIL: Clear(); return false;
}

////////////////////////////////////////////////////////

void DeltaDebugBlock::Add (const DeltaDebugLocal& l) {
	DASSERT(
		std::find_if(
			locals.begin(),
			locals.end(),
			ubinary_or(
				std::bind2nd(std::mem_fun_ref(&DeltaDebugLocal::MatchesName), l.GetName().c_str()),
				std::bind2nd(std::mem_fun_ref(&DeltaDebugLocal::MatchesOffset), l.GetOffset())
			)
		) == locals.end()
	);

	std::list<DeltaDebugLocal>::iterator i = locals.begin();
	while (i != locals.end() && i->GetOffset() < l.GetOffset())
		++i;
	locals.insert(i, l);
}

////////////////////////////////////////////////////////

bool DeltaDebugBlock::Get (const std::string& name, util_ui16* offset) const {
	std::list<DeltaDebugLocal>::const_iterator i;
	i =	std::find_if(
				locals.begin(), 
				locals.end(), 
				std::bind2nd(std::mem_fun_ref(&DeltaDebugLocal::MatchesName), name.c_str())
			);
	if (i == locals.end())
		return false;
	else {
		*offset = i->GetOffset();
		return true;
	}
}

////////////////////////////////////////////////////////

bool DeltaDebugBlock::Get (std::string* name, util_ui16 offset) const {
	std::list<DeltaDebugLocal>::const_iterator i;
	i =	std::find_if(
				locals.begin(), 
				locals.end(), 
				std::bind2nd(std::mem_fun_ref(&DeltaDebugLocal::MatchesOffset), offset)
			);
	if (i == locals.end())
		return false;
	else {
		*name= i->GetName();
		return true;
	}
}

////////////////////////////////////////////////////////
