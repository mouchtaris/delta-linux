/**
 *	Namespace.h
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2009
 */
#ifndef DELTA_NAMESPACE_H
#define DELTA_NAMESPACE_H

#include "Common.h"
#include <map>
#include <stack>
#include "DeltaStdDefs.h"

namespace delta {

////////////////////////////////////////////////////////////////////////

class Namespace {
public:
	typedef std::list<Namespace *> NamespaceList;
	typedef std::multimap<int, Namespace *> OpenedNamespaceMap;

	Namespace(const std::string& id, Namespace* parent = (Namespace*) 0);
	~Namespace();

	static void Initialize	(void);
	static void CleanUp		(void);

	static Namespace * GlobalNamespace	(void);
	static Namespace * CurrentNamespace	(void);

	static void Enter	(const std::string& id, bool removing = false);
	static void Exit	(bool removing = false);
	static bool Open	(int pos, const StdStringList& path, std::string *error = (std::string *) 0);
	static bool Open	(int pos, const Namespace *ns);
	static void CloseAll(void);

	static const Namespace* LookupNamespace(int pos, const StdStringList& id, std::string *error = (std::string *) 0);
	static bool HasFunction(int pos, const StdStringList& id, bool *std = (bool *) 0, std::string *error = (std::string *) 0);

	static const StdStringList CreateNamespacePathList(const std::string& path);
	static const std::string CreateNamespacePathStr(const StdStringList& nameList);

	//**********************************************************************

	void				AddFunction (const std::string& id, bool std);
	void 				RemoveFunction	(const std::string& id);
	bool 				HasFunction(const StdStringList& nameList, bool *std) const;
	bool 				HasFunction(const std::string& id, bool *std) const;

	const Namespace*	LookupNamespace(const StdStringList& nameList) const;
	const Namespace*	LookupNamespace(const std::string& id) const;
	const std::string&	GetId(void) const 
							{ return id; }
	const std::string&	GetAbsId(void) const 
							{ return absId; }
	static bool			IsAnyNamespace (const NamespaceList& l, const std::string& absId);
	static const NamespaceList 
						GetOpenedNamespacesAtPos(int pos);

private:

	typedef std::stack<Namespace *> NamespaceStack;
	typedef std::map<std::string, bool> FunctionMap;
	typedef std::map<std::string, unsigned> OverloadMap;
	static NamespaceStack existing;
	static OpenedNamespaceMap opened;

	std::string id;
	std::string absId;	// full path
	unsigned refCount;
	FunctionMap functions;
	OverloadMap overloaded;
	NamespaceList namespaces;
};

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_NAMESPACE_H