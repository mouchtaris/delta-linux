/**
 *	XMLPropertyVisitor.cpp
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */

#include "XMLPropertyVisitor.h"
#include "Adaptors.h"
#include "Algorithms.h"
#include "Encoding.h"
#include "ExtSerialization.h"
#include "StringUtils.h"
#include "Streams.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <wx/filename.h>

////////////////////////////////////////////////////////////////////////

namespace conf {

////////////////////////////////////////////////////////////////////////

inline const String _Id2XMLTag (const std::string& id)
{
	std::string result(id);
	std::replace(result.begin(), result.end(), ' ', '_');
	std::replace(result.begin(), result.end(), '+', '_');
	if (result[0] >= '0' && result[0] <= '9')
		result = "Num_" + result;
	return util::std2str(result);
}

////////////////////////////////////////////////////////////////////////

inline const String ConvertPath(const String& path, PathPolicy policy, const String& basepath)
{
	wxFileName filename(path);
	if (policy == MAKE_PATHS_RELATIVE && filename.IsAbsolute()) {
		if (wxFileName::DirExists(path))
			filename.AssignDir(path);
		if (filename.SameAs(basepath))
			return _T("./");
		else
			filename.MakeRelativeTo(basepath);
	}
	else if (policy == MAKE_PATHS_ABSOLUTE && filename.IsRelative())
		filename.MakeAbsolute(basepath);
	return util::normalizeslashes(filename.GetFullPath());
}

////////////////////////////////////////////////////////////////////////

template<class TProperty>
void save_value (const std::string& id, TProperty* prop, const xml::Node& parent)
{
	xml::Node node;
	node.Create(_Id2XMLTag(id), parent);
	node.SetContent(prop->Serialize());
}

//**********************************************************************

template<class TProperty>
void load_value (const std::string& id, TProperty* prop, const xml::Node& parent)
{
	const xml::Node node = parent.GetChild(_Id2XMLTag(id));
	if (!node.isNull() && !prop->Deserialize(node.GetContent()))
		DBGOUT << "Error loading property '" << id << "'" << DBGENDL;
}

////////////////////////////////////////////////////////////////////////

#define IMPLEMENT_SAVE_VISIT(className, propertyType)					\
	void className::Visit (const std::string& id, propertyType* prop)	\
		{ save_value(id, prop, this->GetParent()); }

#define IMPLEMENT_LOAD_VISIT(className, propertyType)					\
	void className::Visit (const std::string& id, propertyType* prop)	\
		{ load_value(id, prop, this->GetParent()); }

////////////////////////////////////////////////////////////////////////
// XMLValueSaver class
XMLValueSaver::XMLValueSaver(PathPolicy policy, const String& basepath) :
	policy(policy), basepath(basepath)
{
}

//**********************************************************************

IMPLEMENT_SAVE_VISIT(XMLValueSaver, IntProperty);
IMPLEMENT_SAVE_VISIT(XMLValueSaver, BoolProperty);
IMPLEMENT_SAVE_VISIT(XMLValueSaver, RealProperty);
IMPLEMENT_SAVE_VISIT(XMLValueSaver, StringProperty);
IMPLEMENT_SAVE_VISIT(XMLValueSaver, StdStringProperty);
IMPLEMENT_SAVE_VISIT(XMLValueSaver, IntRangeProperty);
IMPLEMENT_SAVE_VISIT(XMLValueSaver, EnumStringProperty);
IMPLEMENT_SAVE_VISIT(XMLValueSaver, StringListProperty);
IMPLEMENT_SAVE_VISIT(XMLValueSaver, FontProperty);
IMPLEMENT_SAVE_VISIT(XMLValueSaver, ColorProperty);
IMPLEMENT_SAVE_VISIT(XMLValueSaver, DateProperty);
IMPLEMENT_SAVE_VISIT(XMLValueSaver, MultiChoiceProperty);

//**********************************************************************

void XMLValueSaver::Visit (const std::string& id, FileProperty* prop)
	{ SavePath(id, prop->GetValue()); }

//**********************************************************************

void XMLValueSaver::Visit (const std::string& id, DirectoryProperty* prop)
	{ SavePath(id, prop->GetValue()); }

//**********************************************************************

void XMLValueSaver::Visit (const std::string& id, FileListProperty* prop)
	{ SavePathList(id, prop->GetValues()); }

//**********************************************************************

void XMLValueSaver::Visit (const std::string& id, DirectoryListProperty* prop)
	{ SavePathList(id, prop->GetValues()); }

//**********************************************************************

void XMLValueSaver::Visit (const std::string& id, AggregateProperty* prop)
{
	xml::Node node;
	node.Create(_Id2XMLTag(id), this->GetParent());

	XMLValueSaver saver(policy, basepath);
	saver.SetParent(node);
	BOOST_FOREACH(const PropertyMap::value_type& propPair, prop->GetPropertyMap())
		propPair.second->Accept(propPair.first, &saver);
}

//**********************************************************************

void XMLValueSaver::Visit (const std::string& id, AggregateListProperty* prop)
{
	xml::Node node;
	node.Create(_Id2XMLTag(id), this->GetParent());

	XMLValueSaver saver(policy, basepath);
	saver.SetParent(node);
	uint count = 0;
	BOOST_FOREACH(AggregateProperty* p, prop->GetPropertyList())
		p->Accept(std::string("value") + boost::lexical_cast<std::string>(count++), &saver);
}

//**********************************************************************

void XMLValueSaver::SavePath (const std::string& id, const String& path)
{
	xml::Node node;
	node.Create(_Id2XMLTag(id), this->GetParent());
	node.SetContent(ConvertPath(path, policy, basepath));
}

//**********************************************************************

void XMLValueSaver::SavePathList (const std::string& id, const StringVec& paths)
{
	xml::Node node;
	node.Create(_Id2XMLTag(id), this->GetParent());

	String value;
	BOOST_FOREACH(const String& path, paths) {
		if (!value.empty())
			value += _T(";");
		value += ConvertPath(path, policy, basepath);
	}
	node.SetContent(value);
}

////////////////////////////////////////////////////////////////////////
// XMLValueLoader class
//
XMLValueLoader::XMLValueLoader(PathPolicy policy, const String& basepath) :
	policy(policy), basepath(basepath)
{
}

//**********************************************************************

IMPLEMENT_LOAD_VISIT(XMLValueLoader, IntProperty);
IMPLEMENT_LOAD_VISIT(XMLValueLoader, BoolProperty);
IMPLEMENT_LOAD_VISIT(XMLValueLoader, RealProperty);
IMPLEMENT_LOAD_VISIT(XMLValueLoader, StringProperty);
IMPLEMENT_LOAD_VISIT(XMLValueLoader, StdStringProperty);
IMPLEMENT_LOAD_VISIT(XMLValueLoader, IntRangeProperty);
IMPLEMENT_LOAD_VISIT(XMLValueLoader, EnumStringProperty);
IMPLEMENT_LOAD_VISIT(XMLValueLoader, StringListProperty);
IMPLEMENT_LOAD_VISIT(XMLValueLoader, FontProperty);
IMPLEMENT_LOAD_VISIT(XMLValueLoader, ColorProperty);
IMPLEMENT_LOAD_VISIT(XMLValueLoader, DateProperty);
IMPLEMENT_LOAD_VISIT(XMLValueLoader, MultiChoiceProperty);

//**********************************************************************

void XMLValueLoader::Visit (const std::string& id, FileProperty* prop)
{
	String path;
	if (LoadPath(id, &path))
		prop->SetValue(path);
}

//**********************************************************************

void XMLValueLoader::Visit (const std::string& id, DirectoryProperty* prop)
{
	String path;
	if (LoadPath(id, &path))
		prop->SetValue(path);
}

//**********************************************************************

void XMLValueLoader::Visit (const std::string& id, FileListProperty* prop)
{
	StringVec paths;
	if (LoadPathList(id, &paths))
		prop->SetValues(paths);
}

//**********************************************************************

void XMLValueLoader::Visit (const std::string& id, DirectoryListProperty* prop)
{
	StringVec paths;
	if (LoadPathList(id, &paths))
		prop->SetValues(paths);
}

//**********************************************************************

void XMLValueLoader::Visit (const std::string& id, AggregateProperty* prop)
{
	xml::Node node = this->GetParent().GetChild(_Id2XMLTag(id));
	if (!node.isNull()) {
		XMLValueLoader loader(policy, basepath);
		loader.SetParent(node);
		// TODO: make new properties as they are loaded form xml.
		BOOST_FOREACH(const PropertyMap::value_type& propPair, prop->GetPropertyMap())
			propPair.second->Accept(propPair.first, &loader);
	}
}

//**********************************************************************

void XMLValueLoader::Visit (const std::string& id, AggregateListProperty* prop)
{
	xml::Node node = this->GetParent().GetChild(_Id2XMLTag(id));
	if (!node.isNull()) {
		XMLValueLoader loader(policy, basepath);
		loader.SetParent(node);
		uint count = 0;
		for (xml::Node::iterator i = node.ChildBegin(); i != node.ChildEnd(); ++i) {
			AggregateProperty* p = prop->NewProperty();
			p->Accept(std::string("value") + boost::lexical_cast<std::string>(count++), &loader);
		}
	}
}

//**********************************************************************

bool XMLValueLoader::LoadPath (const std::string& id, String* path)
{
	assert(path);
	xml::Node node = this->GetParent().GetChild(_Id2XMLTag(id));
	if (!node.isNull()) {
		String value;
		try { comm::decoder(util::str2std(node.GetContent())) >> value; }	//Backward compatibility
		catch(...) { value = node.GetContent(); }
		*path = ConvertPath(value, policy, basepath);
		return true;
	}
	else
		return false;
}

//**********************************************************************

bool XMLValueLoader::LoadPathList (const std::string& id, StringVec* paths)
{
	assert(paths);
	xml::Node node = this->GetParent().GetChild(_Id2XMLTag(id));
	if (!node.isNull()) {
		String value;
		StringVec values;
		try { comm::decoder(util::str2std(node.GetContent())) >> value; }	//Backward compatibility
		catch(...) { value = node.GetContent(); }
		util::stringtokenizer(values, value, _T(";"));
		BOOST_FOREACH(const String& path, values)
			paths->push_back(ConvertPath(path, policy, basepath));
		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////

} // namespace conf
