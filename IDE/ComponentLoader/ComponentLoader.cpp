/**
 *	ComponentLoader.cpp
 *
 *	-- Component Infrastructure --
 *
 *	ComponentLoader is responsible for scanning the
 *	file system for component entries, building a
 *	directory used at runtime for loading of components
 *	on demand.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "ComponentLoader.h"
#include "ComponentInitializer.h"
#include "ComponentRegistrar.h"
#include "ComponentRegistry.h"
#include "DelayedCaller.h"
#include "Call.h"

#include "xml.h"
#include "Adaptors.h"
#include "Algorithms.h"
#include "StringUtils.h"
#include "Streams.h"

#include <wx/filename.h>

#include <boost/range.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

namespace ide
{
	//-------------------------------------------------------//
	//---- static members -----------------------------------//

	uint ComponentLoader::loadedComponents=0;


	//-------------------------------------------------------//
	//---- class ComponentLoader ----------------------------//

	IMPLEMENT_SINGLETON_WITH_ARGS(ComponentLoader, (const std::string& path), (path));

	//-----------------------------------------------------------------------

	ComponentLoader::ComponentLoader(const std::string& path) : configFile(path)
	{
	}

	//-----------------------------------------------------------------------

	void ComponentLoader::OnInitialize(void)
	{
		//-- Register built-in and dll loaders
		RegisterLoaders("BuiltIn",
			boost::bind(&ComponentLoader::LoadBuiltIn, this, _1),
			boost::bind(&ComponentLoader::UnloadBuiltIn, this, _1)
		);
		RegisterLoaders("DynamicLib",
			boost::bind(&ComponentLoader::LoadDynamicLibrary, this, _1),
			boost::bind(&ComponentLoader::UnloadDynamicLibrary, this, _1)
		);

		const std::string sparrowDir = IDECore::GetInstallationDir();
		const std::string xdfDirectory(sparrowDir + "xdf/");
		xml::Document doc;
		const String configUri = util::std2str(sparrowDir + configFile);
		if (wxFileName::FileExists(configUri) && doc.Load(configUri)) {
			xml::Node root(doc.GetRoot());
			if (root.Name() == _T("components"))
				BOOST_FOREACH(
					const xml::Node& node,
					boost::make_iterator_range(
						root.ChildBegin(_T("component")),
						root.ChildEnd(_T("component"))
					)
				)
					LoadDefinitionFile(xdfDirectory + util::str2std(node[_T("path")]));
		}

		//-- Register to Registry's "not found component", "unregister component" signals
		ComponentRegistry::Instance().sigRequestedUnregisteredComponent.connect(
			boost::bind(&ComponentLoader::slotRequestedUnregisteredComponent, this, _1)
		);

		//-- Register all built-in components
		StdStringList builtins;
		ComponentInitializer::GetPendingComponents(builtins);
		BOOST_FOREACH(const std::string& builtin, builtins)
			LoadComponent(builtin);
	}

	//-----------------------------------------------------------------------

	void ComponentLoader::OnCleanUp(void)
	{
		//-- unregister to Registry's "not found component", "unregister component" signals
		ComponentRegistry::Instance().sigRequestedUnregisteredComponent.disconnect(
			boost::bind(&ComponentLoader::slotRequestedUnregisteredComponent, this, _1)
		);

		//-- Unload all loaded components
		BOOST_FOREACH(
			const ComponentDefinition& def,
			boost::make_iterator_range(
				directory.get<by_load_seq>().begin(),
				directory.get<by_load_seq>().end()
			)
		)
			if (def.loaded && def.type == "DeltaScript")
				DoUnloadComponent(def.id);

		BOOST_FOREACH(
			const ComponentDefinition& def,
			boost::make_iterator_range(
				directory.get<by_load_seq>().begin(),
				directory.get<by_load_seq>().end()
			)
		)
			if (def.loaded)
				DoUnloadComponent(def.id);

		std::for_each(
			plugins.begin(),
			plugins.end(),
			util::project2nd<PluginMapping::value_type>(
				util::deleter<util::DynamicLoader>()
			)
		);
		plugins.clear();

		////-- Unload dynamic loaders
		//delete deltaScriptLoader;
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::RegisterLoaders(const std::string& type,
		const Loader& loader, const Unloader& unloader)
	{
		if (loaders.find(type) != loaders.end())
			return false;

		loaders[type] = std::make_pair(loader, unloader);
		return true;
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::UnregisterLoaders(const std::string& type)
	{
		ComponentLoaderMapping::iterator it = loaders.find(type);
		if (it == loaders.end())
			return false;

		loaders.erase(it);
		return true;
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::LoadDefinitionFile(const std::string& curi)
	{
		const String uri = util::std2str(curi);
		xml::Document doc;
		if (!wxFileName::FileExists(uri) || !doc.Load(uri))
			return false;
		xml::Node root(doc.GetRoot());

		//-- Start processing the XML file
		if (root.Name() != _T("extension"))
			return false;

		const std::string sparrowDir = IDECore::GetInstallationDir();
		//-- Load component entries
		BOOST_FOREACH(
			const xml::Node& node,
			boost::make_iterator_range(
				root.ChildBegin(_T("component")),
				root.ChildEnd(_T("component"))
			)
		)
		{
			directory.get<by_id>().insert(
				ComponentDefinition(
					util::str2std(node[_T("id")]),
					util::str2std(node[_T("type")]),
					sparrowDir + util::str2std(node[_T("uri")])
				)
			);
		}

		return true;
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::UnloadDefinitionFile(const std::string& uri)
	{
		directory.get<by_uri>().erase(uri);
		return true;
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::LoadComponent(const std::string& id)
	{
		const ComponentDefinition& def = GetComponentDefinition(id);
		if (!def)				///< component does not exist
			return false;
		else if (def.loaded)	///< component is already loaded
			return true;

		//-- Load extensions according to its type
		ComponentLoaderMapping::const_iterator it = loaders.find(def.type);
		if (it == loaders.end())
			return false;
		def.loaded = ++loadedComponents;
		bool success = it->second.first(def.uri);	///< try load component with appropriate loader

		if (success)
		{
			if (def.type == "DeltaScript")
				ComponentPostInitializer::DelayedInit(ComponentPostInitializer::TupleType(id));
			ComponentInitializer::InitializePendingComponent(id);
			if (ComponentRegistry::Instance().GetComponentEntry(id).HasFunction("Initialize", false))
				Call<void (void)>("Loader", id, "Initialize")();

			sigComponentLoaded(id);
		}
		else
			def.loaded = 0;	///< loading failed

		return success;
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::UnloadComponent(const std::string& id)
	{
		const ComponentDefinition& def = GetComponentDefinition(id);
		if (!def || def.type == "BuiltIn")	///< component does not exist or is built-in
			return false;
		else if (!def.loaded)	///< component is not loaded
			return true;

		const std::string storedId = id;
		timer::DelayedCaller::Instance().PostDelayedCall(
			boost::bind(&ComponentLoader::DoUnloadComponent, this, storedId)
		);
		return true;
	}

	//-----------------------------------------------------------------------

	void ComponentLoader::ListAvailableComponents(StdStringList& components) const
	{
		BOOST_FOREACH(
			const ComponentDefinition& comp,
			boost::make_iterator_range(
				directory.get<by_id>().begin(),
				directory.get<by_id>().end()
			)
		)	///< collect all available component names
			components.push_back(comp.id);
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::IsLoaded(const std::string& id) const
	{
		const ComponentDefinition& def = GetComponentDefinition(id);
		return (def && def.loaded) ? true : false;
	}

	//-----------------------------------------------------------------------

	const std::string& ComponentLoader::GetComponentType(const std::string& id) const
	{
		const ComponentDefinition& def = GetComponentDefinition(id);
		return def.type;
	}

	//-----------------------------------------------------------------------

	const std::string& ComponentLoader::GetComponentURI(const std::string& id) const
	{
		const ComponentDefinition& def = GetComponentDefinition(id);
		return def.uri;
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::DoUnloadComponent(const std::string& id)
	{
		const ComponentDefinition& def = GetComponentDefinition(id);
		assert(def && def.loaded);

		def.loaded = 0;	///< mark the component as unloaded

		//-- ComponentLoader deliberately unloads all components residing in
		//	 the same extension, this is a design choice not inability to do
		//	 otherwise.
		MarkExtensionAsLoaded(def.uri, 0);
		BOOST_FOREACH(
			const ComponentDefinition& comp,
			boost::make_iterator_range(
				directory.get<by_uri>().lower_bound(def.uri),
				directory.get<by_uri>().upper_bound(def.uri)
			)
		)	///< unregister components from the Registry
		{
			if (ComponentRegistry::Instance().GetComponentEntry(comp.id).HasFunction("CleanUp", false))
				Call<void (void)>("Loader", comp.id, "CleanUp")();
			ComponentRegistry::Instance().Unregister(comp.id);
		}

		//-- Unload extensions according to its type
		ComponentLoaderMapping::const_iterator it = loaders.find(def.type);
		if (it == loaders.end())
			return false;
		bool success = it->second.second(def.uri);	///< try unload component with appropriate loader

		if (success)
			sigComponentUnloaded(def.id);

		return success;
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::LoadBuiltIn(const std::string& uri)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::UnloadBuiltIn(const std::string& uri)
	{
		return true;
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::LoadDynamicLibrary(const std::string& uri)
	{
		if (plugins.find(uri) != plugins.end())
			return true;	///< in case I have already loaded the plugin for another component

#ifndef NDEBUG
		std::string uri_d = uri.substr(0, uri.find_last_of('.'))
			+ "_d" + uri.substr(uri.find_last_of('.'));
		util::DynamicLoader* plugin = new util::DynamicLoader(uri_d);
#else
		util::DynamicLoader* plugin = new util::DynamicLoader(uri);
#endif

		if (!(*plugin))
			///< check if plugin loaded successfully
		{
			delete plugin;
			return false;
		}

		plugin->Call("Initialize");
		plugins[uri] = plugin;

		return true;
	}

	//-----------------------------------------------------------------------

	bool ComponentLoader::UnloadDynamicLibrary(const std::string& uri)
	{
		//-- retrieve plugin from holder
		PluginMapping::iterator it = plugins.find(uri);
		if (it == plugins.end())
			return false;

		//-- call clean up and free the library
		it->second->Call("CleanUp");
		delete it->second;
		plugins.erase(it);

		return true;
	}

	//-----------------------------------------------------------------------

	void ComponentLoader::MarkExtensionAsLoaded(const std::string& uri, uint loaded)
	{
		BOOST_FOREACH(
			const ComponentDefinition& def,
			boost::make_iterator_range(
				directory.get<by_uri>().lower_bound(uri),
				directory.get<by_uri>().upper_bound(uri)
			)
		)
			def.loaded = loaded;
	}

	//-----------------------------------------------------------------------

	const ComponentLoader::ComponentDefinition& ComponentLoader::GetComponentDefinition(
		const std::string& id) const
	{
		static ComponentDefinition null_entity;

		ComponentDirectory::index_iterator<by_id>::type it =
			directory.get<by_id>().find(id);
		return (it != directory.get<by_id>().end())
			? *it
			: null_entity;
	}

	//-----------------------------------------------------------------------

	void ComponentLoader::slotRequestedUnregisteredComponent(const std::string& id)
	{
		LoadComponent(id);
	}

	//-----------------------------------------------------------------------
}
