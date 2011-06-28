/**
 *	Namespace.cpp
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2009
 */

#include "Namespace.h"
#include "StringUtils.h"

#include <boost/foreach.hpp>

#define	DELTA_LIBRARYNAMESPACE_GLOBAL	"$global"	// hidden name

namespace delta {

////////////////////////////////////////////////////////////////////////
// Namespace class
//

Namespace::NamespaceStack Namespace::existing;
Namespace::OpenedNamespaceMap Namespace::opened;

//**********************************************************************

Namespace::~Namespace(){
	std::for_each(
		namespaces.begin(), 
		namespaces.end(), 
		util::deleter<Namespace>()
	);
}

Namespace::Namespace(const std::string& id, Namespace* parent) : id(id), refCount(0) {
	if (parent && parent->id != DELTA_LIBRARYNAMESPACE_GLOBAL) 
		absId = parent->absId + DELTA_LIBRARYNAMESPACE_SEPARATOR + id; 
	else
		absId = id;
}

//**********************************************************************

void Namespace::Initialize(void)
{
	Namespace *global = new Namespace(DELTA_LIBRARYNAMESPACE_GLOBAL);
	existing.push(global);
	Open(0, global);
}

void Namespace::CleanUp(void)
{
	assert(existing.size() == 1 && existing.top()->GetId() == DELTA_LIBRARYNAMESPACE_GLOBAL);
	delete existing.top();
	opened.clear();
}

//**********************************************************************

Namespace * Namespace::GlobalNamespace(void)
{
	assert(existing.size() == 1 && existing.top()->GetId() == DELTA_LIBRARYNAMESPACE_GLOBAL);
	return existing.top();
}

Namespace * Namespace::CurrentNamespace(void)
{
	assert(!existing.empty());
	return existing.top();
}

//**********************************************************************

void Namespace::Enter(const std::string& id, bool removing)
{
	assert(!existing.empty());
	Namespace *ns = const_cast<Namespace*>(existing.top()->LookupNamespace(id));
	assert(!removing || removing && ns);
	if (!ns)
		existing.top()->namespaces.push_back(ns = new Namespace(id, existing.top()));
	if (!removing)
		++ns->refCount;
	existing.push(ns);
}

void Namespace::Exit(bool removing)
{
	if (existing.size() > 1)
		if (removing && --existing.top()->refCount == 0)
		{
			Namespace *ns = existing.top();
			assert(ns->functions.empty() && ns->namespaces.empty());
			existing.pop();
			existing.top()->namespaces.remove(ns);
			delete ns;
		}
		else
			existing.pop();
}

//**********************************************************************

bool Namespace::Open(int pos, const Namespace *ns)
{
	opened.insert(std::make_pair(pos, const_cast<Namespace*>(ns)));
	return true;
}

void Namespace::CloseAll(void)
{
	opened.clear();
	assert(existing.size() == 1 && existing.top()->GetId() == DELTA_LIBRARYNAMESPACE_GLOBAL);
	Open(0, existing.top());
}

const Namespace::NamespaceList Namespace::GetOpenedNamespacesAtPos(int pos)
{
	NamespaceList openedAtPos;
	for(OpenedNamespaceMap::const_iterator i = opened.begin(); i != opened.end(); ++i)
		if (i->first <= pos)
			openedAtPos.push_back(i->second);
	return openedAtPos;
}

//**********************************************************************

bool Namespace::Open(int pos, const StdStringList& nameList, std::string* error)
{
	if (nameList.front() == DELTA_LIBRARYNAMESPACE_SEPARATOR) {
		StdStringList pathList(nameList);
		pathList.pop_front();

		if (const Namespace* ns = GlobalNamespace()->LookupNamespace(pathList))
			return Open(pos, ns);
		else
		{
			if (error)
				*error = "Invalid namespace path";
			return false;
		}
	}
	else {
		if (const Namespace* ns = Namespace::LookupNamespace(pos, nameList, error))
			return Open(pos, ns);
		else
		{
			if (error && error->empty())
				*error = "Could not find namespace in any opened namespace";
			return false;
		}
	}
}

bool Namespace::HasFunction(int pos, const StdStringList& nameList, bool *std, std::string *error)
{
	if (nameList.front() == DELTA_LIBRARYNAMESPACE_SEPARATOR) {
		StdStringList pathList(nameList);
		pathList.pop_front();
		if (GlobalNamespace()->HasFunction(pathList, std))
			return true;
		else
		{
			if (error)
				*error = "Invalid namespace path";
			return false;
		}
	}
	else
	{
		const NamespaceList openedAtPos = GetOpenedNamespacesAtPos(pos);
		bool found = false;
		for(NamespaceList::const_iterator i = openedAtPos.begin(); i != openedAtPos.end(); ++i)
			if ((*i)->HasFunction(nameList, std))
			{
				if (found) {
					if (error)
						*error = "Function matches more than one paths (use namespace path)";
					found = false;
					break;
				}
				else
					found = true;
			}
		if (!found && error && error->empty())
			*error = "Could not find library function";
		return found;
	}
}

const Namespace* Namespace::LookupNamespace(int pos, const StdStringList& nameList, std::string *error)
{
	if (nameList.front() == DELTA_LIBRARYNAMESPACE_SEPARATOR) {
		StdStringList pathList(nameList);
		pathList.pop_front();
		return GlobalNamespace()->LookupNamespace(pathList);
	}
	else
	{
		const Namespace *target = (const Namespace *) 0;
		NamespaceList openedAtPos = GetOpenedNamespacesAtPos(pos);
		for(NamespaceList::const_iterator i = openedAtPos.begin(); i != openedAtPos.end(); ++i)
			if (const Namespace *ns = (*i)->LookupNamespace(nameList))
			{
				if (target) {
					if (error)
						*error = "Namespace '" + CreateNamespacePathStr(nameList) + 
						"' matches more than one paths (use namespace path)";
					target = (const Namespace *) 0;
					break;
				}
				else
					target = ns;
			}
		return target;
	}
}

//**********************************************************************

const StdStringList Namespace::CreateNamespacePathList(const std::string& path)
{
	StdStringList nameList;
	util::stringtokenizer(nameList, path, DELTA_LIBRARYNAMESPACE_SEPARATOR);
	if (path.find(DELTA_LIBRARYNAMESPACE_SEPARATOR) == 0)
		nameList.push_front(DELTA_LIBRARYNAMESPACE_SEPARATOR);
	return nameList;
}

const std::string Namespace::CreateNamespacePathStr(const StdStringList& nameList)
{
	std::string str;
	for(StdStringList::const_iterator i = nameList.begin(); i != nameList.end(); ++i) {
		if (i != nameList.begin())
			str += DELTA_LIBRARYNAMESPACE_SEPARATOR;
		str += *i;
	}
	return str;
}

//**********************************************************************

void Namespace::AddFunction (const std::string& id, bool std) { 
	functions[id] = std; 
	if (overloaded.find(id) == overloaded.end())
		overloaded[id] = 1;
	else
		++overloaded[id];
}

//**********************************************************************

void Namespace::RemoveFunction (const std::string& id)
{
	FunctionMap::iterator i = functions.find(id);
	OverloadMap::iterator j = overloaded.find(id);
	
	assert(j != overloaded.end());
	assert(i != functions.end());
	assert(j->second);

	if (!--j->second) 
		{ functions.erase(i); overloaded.erase(j); }
}

bool Namespace::HasFunction(const StdStringList& nameList, bool *std) const
{
	if (nameList.empty())
		return false;
	else if (nameList.size() == 1)
		return HasFunction(nameList.front(), std);
	else if (const Namespace *ns = LookupNamespace(nameList.front()))
	{
		StdStringList rest(nameList);
		rest.pop_front();
		return ns->HasFunction(rest, std);
	}
	else
		return false;
}

bool Namespace::HasFunction(const std::string& id, bool *std) const {
	FunctionMap::const_iterator i = functions.find(id);
	bool result = i != functions.end();
	if (result && std)
		*std = i->second;
	return result;
}

const Namespace* Namespace::LookupNamespace(const StdStringList& nameList) const
{
	if (nameList.empty())
		return (const Namespace *) 0;
	else if (nameList.size() == 1)
		return LookupNamespace(nameList.front());
	else if (const Namespace *ns = LookupNamespace(nameList.front()))
	{
		StdStringList rest(nameList);
		rest.pop_front();
		return ns->LookupNamespace(rest);
	}
	else
		return (const Namespace *) 0;
}

const Namespace* Namespace::LookupNamespace(const std::string& id) const
{
	BOOST_FOREACH(const Namespace *ns, namespaces)
		if (ns->GetId() == id)
			return ns;
	return (const Namespace *) 0;
}

bool Namespace::IsAnyNamespace (const NamespaceList& l, const std::string& absId) {
	for (NamespaceList::const_iterator i = l.begin(); i != l.end(); ++i)
		if ((*i)->GetAbsId() == absId)
			return true;
	return false;
}

////////////////////////////////////////////////////////////////////////

} // namespace delta