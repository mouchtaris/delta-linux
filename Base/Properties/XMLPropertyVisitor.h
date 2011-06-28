/**
 *	XMLPropertyVisitor.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef CONF_XMLPROPERTYVISITOR_H
#define CONF_XMLPROPERTYVISITOR_H

#include "Common.h"
#include "LibraryAPIDecls.h"
#include "PropertyVisitor.h"
#include "xml.h"

namespace conf {

////////////////////////////////////////////////////////////////////////

enum PathPolicy {
	MAINTAIN_PATHS		= 0,
	MAKE_PATHS_RELATIVE	= 1,
	MAKE_PATHS_ABSOLUTE	= 2
};

////////////////////////////////////////////////////////////////////////

class _BASE_API XMLPropertyVisitorBase : public PropertyVisitor {
public:
	const xml::Node&	GetParent (void) const { return m_parent; }
	void				SetParent (const xml::Node& parent) { m_parent = parent; }

private:
	xml::Node m_parent;
};

////////////////////////////////////////////////////////////////////////

class _BASE_API XMLValueSaver : public XMLPropertyVisitorBase {
public:
	void Visit (const std::string& id, IntProperty* prop);
	void Visit (const std::string& id, BoolProperty* prop);
	void Visit (const std::string& id, RealProperty* prop);
	void Visit (const std::string& id, StringProperty* prop);
	void Visit (const std::string& id, StdStringProperty* prop);
	void Visit (const std::string& id, IntRangeProperty* prop);
	void Visit (const std::string& id, EnumStringProperty* prop);
	void Visit (const std::string& id, StringListProperty* prop);
	void Visit (const std::string& id, AggregateProperty* prop);

	void Visit (const std::string& id, FontProperty* prop);
	void Visit (const std::string& id, ColorProperty* prop);
	void Visit (const std::string& id, FileProperty* prop);
	void Visit (const std::string& id, DirectoryProperty* prop);
	void Visit (const std::string& id, DateProperty* prop);
	void Visit (const std::string& id, FileListProperty* prop);
	void Visit (const std::string& id, DirectoryListProperty* prop);
	void Visit (const std::string& id, AggregateListProperty* prop);
	void Visit (const std::string& id, MultiChoiceProperty* prop);

	XMLValueSaver(PathPolicy policy = MAINTAIN_PATHS, const String& basepath = String());

private:
	void SavePath		(const std::string& id, const String& path);
	void SavePathList	(const std::string& id, const StringVec& paths);

	PathPolicy	policy;
	String		basepath;
};

////////////////////////////////////////////////////////////////////////

class _BASE_API XMLValueLoader : public XMLPropertyVisitorBase {
public:
	void Visit (const std::string& id, IntProperty* prop);
	void Visit (const std::string& id, BoolProperty* prop);
	void Visit (const std::string& id, RealProperty* prop);
	void Visit (const std::string& id, StringProperty* prop);
	void Visit (const std::string& id, StdStringProperty* prop);
	void Visit (const std::string& id, IntRangeProperty* prop);
	void Visit (const std::string& id, EnumStringProperty* prop);
	void Visit (const std::string& id, StringListProperty* prop);
	void Visit (const std::string& id, AggregateProperty* prop);

	void Visit (const std::string& id, FontProperty* prop);
	void Visit (const std::string& id, ColorProperty* prop);
	void Visit (const std::string& id, FileProperty* prop);
	void Visit (const std::string& id, DirectoryProperty* prop);
	void Visit (const std::string& id, DateProperty* prop);
	void Visit (const std::string& id, FileListProperty* prop);
	void Visit (const std::string& id, DirectoryListProperty* prop);
	void Visit (const std::string& id, AggregateListProperty* prop);
	void Visit (const std::string& id, MultiChoiceProperty* prop);

	XMLValueLoader(PathPolicy policy = MAINTAIN_PATHS, const String& basepath = String());

private:
	bool LoadPath		(const std::string& id, String* path);
	bool LoadPathList	(const std::string& id, StringVec* paths);

	PathPolicy	policy;
	String		basepath;
};


////////////////////////////////////////////////////////////////////////

} // namespace conf

#endif // CONF_XMLPROPERTYVISITOR_H
