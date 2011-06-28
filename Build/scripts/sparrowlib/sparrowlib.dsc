//////////////////////////////////////////////////////////////////////////////

using std;

//-- Helper functions
//
function bind(f...) {
	local t = [
		@f : f, @args : [],
		method @operator() 
			{ return @f(|@args|,...); }
	];
	(t.args = arguments).pop_front();
	return t;
}

//-- overloading function of operator .. in order for a handle
//	 to dynamically dispatch function calling and propertiis
//
function component_dispatcher (handle, funcName) {

	 if (isoverloadableoperator(funcName))
	 	return nil;

	//-- there are three basic handle members that are not looked up
	if (funcName == "Handle" or funcName == "class_id" or funcName == "serial")
		return tabget(handle, funcName);

	//-- the decorator object wraps the bound spw::call in order
	//	 to imbue some return values with further properties
	function decorator(f) {
		return [
			@f : f,
			method @operator() {
				if (typeof(local retval = @f(...)) == "Object" and retval.class == "Handle")
					retval["."] = component_dispatcher;
				return retval;
			}
		];
	}

	//-- bind the function call and decorate it in order to overload
	//	 handle's operator .
	return decorator(::bind(spw::call, handle, funcName));
}

//-- global function, generator of a sparrow object
function sparrow {
	//-- library for the delta ide extension API
	//	 this lib is a wrapper of the library functions
	//	 provided from deltaide in order to give a more
	//	 natural interface
	//
	return [
		method handle (class_id, serial) {
			return [
				@class 		: #Handle,
				@class_id 	: class_id,
				@serial 	: serial,
				{ "." 		: component_dispatcher }
			];
		},

		@components : [		///< table containing every component of the system
			method @operator.(t, classId) {
				if (isoverloadableoperator(classId))
					return nil;
					
				//-- retrieve component entry
				local handle = spw::getcomponent(classId);

				//-- allow the component to load new exported functions on demand
				handle["."] = component_dispatcher;
				return handle;
			}
		],

		method createcomponent (classId) {	///< classId[, parent]
			local handle = spw::createcomponent(classId);
			if (arguments.total() > 1)
				spw::setparent(handle, arguments[1]);
			handle["."] = component_dispatcher;
			return handle;
		},

		method destroycomponent (component)	///< can either be a string or a Handle
			{ spw::destroycomponent(component); },

		method decorate (handle) 
			{ tabset(handle, ".", component_dispatcher); return handle; },

		method getcomponents				///< as handles only
			{ return spw::getcomponents(); }
	];
}

//////////////////////////////////////////////////////////////////////////////
