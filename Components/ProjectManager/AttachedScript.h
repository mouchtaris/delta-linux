/**
 *	AttachedScript.h
 *
 *	-- IDE AttachedScript Component --
 *
 *	AttachedScript component represents a script that can
 *  be attached to another script within the Workspace.
 *
 *	Ioannis Lilis <lilis@ics.forth.gr>
 *	September 2013
 */
#ifndef PROJECT_MANAGER_ATTACHED_SCRIPT_H_
#define PROJECT_MANAGER_ATTACHED_SCRIPT_H_

#include "Common.h"
#include "Script.h"

namespace ide
{
	class AttachedScript :
		public Script
	{
		DECLARE_IDE_COMPONENT(AttachedScript);

	public:
		///--- type definitions

		///--- constructors / destructor
		AttachedScript(void);
		~AttachedScript(void);

		///--- public API

		///--- exported API
		DECLARE_EXPORTED_STATIC(void, Initialize, (void));
		DECLARE_EXPORTED_STATIC(void, CleanUp, (void));
		DECLARE_EXPORTED_FUNCTION(void, Open, (void));

	protected:
		///--- protected API
		virtual void ComponentAddedProperty (const std::string& id, conf::Property* prop);
	};
}

#endif	//PROJECT_MANAGER_ATTACHED_SCRIPT_H_
