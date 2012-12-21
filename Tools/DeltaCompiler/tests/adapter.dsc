//////////////////////////////////////////////////////////////////////////////////
// Adapter pattern as a metaprogram
//
&using std;

//////////////////////////////////////////////////////////////////////////////////
// Utility metafunctions
//
&function id_new(name) { return std::ast_new("Name", [@name : name]); }

&function GetMethodName(m) {
	if ((local n = m.get_child("name")) and n.get_tag() == "Name")
		return n.get_attribute("name");
	else
		return "";
}

//Just for now hardcode the class def. TODO: fix it properly later.
&function GetClassDefByName(...) {
	return [
		@methods : <<
			method Draw() {},
			method SetWholeScreen() {},
			method Iconify() {}
		>>,
		method getMethods() { return @methods.get_children(); }
	];
}

//////////////////////////////////////////////////////////////////////////////////
// Implementation of the Adapter pattern using both subclassing and delegation.
// The code generated is based on the given methods to be adapted.
// (see example below for usage)
//
&function AdapterBySubclassing() {
	return [
		method adapt (spec) {
			local adaptedMethods = nil;
			local class = GetClassDefByName(spec.original);
			foreach(local m, class.getMethods()) {
				local name = GetMethodName(m);
				if (local newName = spec.renames[name])
					adaptedMethods = <<
						~adaptedMethods,
						method ~(id_new(newName)) (...) { self[~name](...); }
					>>;
			}
			return <<
				function ~(spec.adapted)(...) {
					local base = ~(spec.original)(...);
					local derived = [ ~adaptedMethods ];
					std::inherit(derived, base);
					return derived;
				}
			>>;
		}
	];	
}
&function AdapterByDelegation() {
	return [
		method adapt (spec) {
			local methods = nil;
			local class = GetClassDefByName(spec.original);
			foreach(local m, class.getMethods()) {
				local name = GetMethodName(m);
				local newName = spec.renames[name];
				if (not newName) newName = name;	//keep original name
				methods = <<
					~methods,
					method ~(id_new(newName)) (...) { @instance[~name](...); }
				>>;
			}
			return <<
				function ~(spec.adapted) (...) {
					return [
						@instance : ~(spec.original)(...),
						~methods
					];
				}
			>>;
		}
	];
}

&AdapterFactory = [
	method Install(type, func) { self[type] = func; },
	method New(type) { return self[type](); }
];

&AdapterFactory.Install("subclassing", AdapterBySubclassing);
&AdapterFactory.Install("delegation", AdapterByDelegation);

//////////////////////////////////////////////////////////////////////////////////
// Example using the Adapter pattern for a window class
//
&adapterType = "delegation";	//dynamically read
&adapter = AdapterFactory.New(adapterType);

function Window(args) { 
	return [
		method Draw() { std::print("Draw\n"); },
		method SetWholeScreen() { std::print("SetWholeScreen\n"); },
		method Iconify() { std::print("Iconify\n"); }
	];
}

&windowAdapterData = [
	@original: <<Window>>,
	@adapted : <<WindowAdapter>>,
	@renames : [
		{ "SetWholeScreen"	: "Maximize" },
		{ "Iconify"			: "Minimize" }
	]
];
!(adapter.adapt(windowAdapterData));

window = WindowAdapter();
window.Draw();
window.Maximize();

&windowAdapterData.adapted = <<WindowAdapter2>>;
!(AdapterFactory.New("subclassing").adapt(windowAdapterData));

window2 = WindowAdapter2();
window2.Draw();
window2.Minimize();