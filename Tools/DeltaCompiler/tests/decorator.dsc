//////////////////////////////////////////////////////////////////////////////////
// Decorator pattern as a metaprogram
//
&using std;
using std;

//////////////////////////////////////////////////////////////////////////////////
// Utility metafunctions
//
&function id_new(name) { return std::ast_new("Name", [@name : name]); }

&function GetMethods(ctor) {
	local methods = list_new();
	local visitor = astvisitor_new();
	local functionNesting = 0;
	visitor.set_handler("Function", function(node, id, entering) {
		if (entering)
			++functionNesting;
		else
			if (--functionNesting == 0 and node.get_attribute("class") == "Method") {
				local child = node.get_child("name");
				methods.push_back(child.get_attribute("name"));
			}
	});
	ctor.accept_preorder(visitor);
	return methods;
}

//////////////////////////////////////////////////////////////////////////////////
// Implementation of the Decorator pattern for constructor functions 
// The code generated is based on the given base members and desired decorations.
// (see example below for usage)
//
&function MakeDecorators(func, allDecoratorSpecs) {
	func = func.get_child(0);
	local methods = GetMethods(func.get_child("body"));
	local delegations = nil;
	foreach(local name, methods)
		delegations = <<~delegations, method ~(id_new(name)) { @instance[~name](); }>>;
	
	local decorators = nil;
	foreach(local decoratorSpecs, allDecoratorSpecs) {
		local decorator = <<
			function ~(id_new(decoratorSpecs.name)) (instance) {
				return [
					@instance : instance,
					~delegations,
					~(decoratorSpecs.overrides)
				];
			}
		>>;
		decorators = <<~decorators, ~decorator>>;
	}
	return decorators;
}

//////////////////////////////////////////////////////////////////////////////////
// Example using the Decorator pattern for car addons
//
&func = <<
	function Car() {
		return [
			method Move { print("Car::Move\n"); },
			method Stop { print("Car::Stop\n"); },
			method Break { print("Car::Break\n"); },
			method Accelerate { print("Car::Accelerate\n"); },
			method FourWheelDrive { print("Car::4WD\n"); }
		];
	}
>>;
!(MakeDecorators(
	func,
	[
		[
			@name : "ABSCar",
			@overrides : <<
				method Break { print("ABSCar::Break\n"); }
			>>
		],
		[
			@name : "TurboCar",
			@overrides : <<
				method Move { print("TurboCar::Move\n"); },
				method Accelerate { print("TurboCar::Accelerate\n"); } 
			>>
		],
		[
			@name : "Jeep",
			@overrides : <<
				method FourWheelDrive { print("Jeep::4WD\n"); }
			>>
		]
	]
));	//Apply the pattern
!(func);

car = Jeep(TurboCar(Car()));
car.Accelerate();
car.FourWheelDrive();
car.Break();
