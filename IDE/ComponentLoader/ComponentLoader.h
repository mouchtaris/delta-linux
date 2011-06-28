/**
 *	ComponentLoader.h
 *
 *	-- Component Infrastructure --
 *
 *	ComponentLoader is responsible for scanning the
 *	file system for component entries, building a
 *	directory used at runtime for loading of components
 *	on damnd.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef IDE_COMPONENT_LOADER_H_
#define IDE_COMPONENT_LOADER_H_

#include "Common.h"
#include "IDECommon.h"

#include "Singleton.h"
#include "DynamicLoader.h"

#include <boost/function.hpp>
#include <boost/signal.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <map>

using namespace boost::multi_index;

namespace ide
{
	//----------------------------
	//-- class ComponentLoader

	class _IDE_API ComponentLoader :
		public util::Singleton
	{
		DECLARE_SINGLETON_WITH_ARGS(ComponentLoader, (const std::string&));

	public:
		///--- type definitions
		typedef boost::function<bool (const std::string&)> Loader;
		typedef boost::function<bool (const std::string&)> Unloader;

		typedef boost::signal<void (const std::string&)> ComponentLoadedSignal;
		typedef boost::signal<void (const std::string&)> ComponentUnloadedSignal;

		///--- public API
		bool RegisterLoaders(const std::string& type,
			const Loader& loader, const Unloader& unloader);
		bool UnregisterLoaders(const std::string& type);

		bool LoadDefinitionFile(const std::string& uri);
		bool UnloadDefinitionFile(const std::string& uri);

		bool LoadComponent(const std::string& id);
		bool UnloadComponent(const std::string& id);

		void ListAvailableComponents(StdStringList& components) const;

		bool IsLoaded(const std::string& id) const;
		const std::string& GetComponentType(const std::string& id) const;
		const std::string& GetComponentURI(const std::string& id) const;

		///--- Signals
		ComponentLoadedSignal	sigComponentLoaded;
		ComponentUnloadedSignal	sigComponentUnloaded;

	protected:
		///--- protected API
		ComponentLoader(const std::string& path);

		void OnInitialize(void);
		void OnCleanUp(void);

	private:
		//----------------------------
		//-- struct ComponentDefinition
		struct ComponentDefinition
		{
			///--- type definitions

			///--- constructors / destructor
			ComponentDefinition(void)
				: loaded(0) {}
			ComponentDefinition(const std::string& id, const std::string& type, const std::string& uri)
				: loaded(0), id(id), type(type), uri(uri) {}

			///--- public API
			operator bool (void) const { return !type.empty(); }

			///--- public members
			mutable uint loaded;

			std::string id;
			std::string type;
			std::string uri;
		};

		///--- private API
		bool DoUnloadComponent(const std::string& id);

		bool LoadBuiltIn(const std::string& uri);
		bool UnloadBuiltIn(const std::string& uri);
		bool LoadDynamicLibrary(const std::string& uri);
		bool UnloadDynamicLibrary(const std::string& uri);

		void MarkExtensionAsLoaded(const std::string& uri, uint loaded);
		const ComponentDefinition& GetComponentDefinition(const std::string& id) const;

		void slotRequestedUnregisteredComponent(const std::string& id);

		///--- tags for multi-index
		struct by_id {};
		struct by_uri {};
		struct by_load_seq {};

		///--- multi-index structure of component's directory
		typedef multi_index_container<
			ComponentDefinition,
			indexed_by<
				ordered_unique<		///< sort by id
					tag<by_id>, member<ComponentDefinition, std::string, &ComponentDefinition::id>
				>,
				ordered_non_unique<	///< sort by uri
					tag<by_uri>, member<ComponentDefinition, std::string, &ComponentDefinition::uri>
				>,
				ordered_non_unique<		///< sort by load sequence
					tag<by_load_seq>, member<ComponentDefinition, uint, &ComponentDefinition::loaded>
				>
			>
		> ComponentDirectory;

		///--- private members
		static uint loadedComponents;

		std::string configFile;				///< configuration file for the component definitions

		typedef std::pair<Loader, Unloader> ComponentTypeLoader;
		typedef std::map<std::string, ComponentTypeLoader> ComponentLoaderMapping;
		ComponentLoaderMapping loaders;		///< mapping of component types and their loaders

		typedef std::map<std::string, util::DynamicLoader*> PluginMapping;
		PluginMapping plugins;				///< dynamically loaded plugins that are managed

		ComponentDirectory directory;		///< directory of component definitions

		//-- Loaders, dynamically loaded at start-up
		util::DynamicLoader* deltaScriptLoader;
	};
}


#endif	//IDE_COMPONENT_LOADER_H_
