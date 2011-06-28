/////////////////////////////////////////////////////////////////////
// Example shows how to make dynamic object factories.
// A. Savidis, July 2009.
/////////////////////////////////////////////////////////////////////

const nl = "\n";

pt = [
	@x { 
		@set (method(x) { @x = x + 1; })
		@get (method() { return @x; })
	},
	{.x		: 12 },
	{"."	: (method(t, i) { 
					if (isoverloadableoperator(i))
						return nil; 
					return @x; 
				})},
	{".="	: (method(t, i, v) { @x = v; })}
];

pt.x = 20;
pt.y = 30;
print(pt.x, nl);
print(pt, nl);


function Bind1st_ (f, x) {
	return [	
		{ "x"	: x }, 
		{ "f"	: f },
		(method(){
			return @f(@x, |arguments|); 
		})
	][0];
}

function ListForEach_(l, f) {
	for (	local i = listiter_new(), listiter_setbegin(i, l);
			not listiter_checkend(i, l);
			listiter_fwd(i)	)
			f(listiter_getval(i));
}

/////////////////////////////////////////////////////////////////////

function Sig_(t) {
	for (local s = "", local n = tablength(t), local i = 0; i < n; ++i)
		if ((typeof(t[i]) == "Table" or typeof(t[i]) == "Object") and t[i]..class != nil)
			s += t[i]..class;
		else
			s += typeof(t[i]);
	if (s == "") s = "void";
	return s;
}

/////////////////////////////////////////////////////////////////////

function MakeClass_(id) {

	function AddObjectField_ (obj, field){
		local accessors = [ 
				{.field : field }, { .obj : obj },
				(method(){ return @obj[@field]; }),
				(method(x){ @obj[@field] = x; })
			];					
		obj["get_" + field] = accessors[0];
		obj["set_" + field] = accessors[1];
	}
	
	function AddObjectMethod_(obj, info){
		if (typeof(info.func) == "MethodFunc") 
			{ info.func.owner = obj; obj[info.name] = info.func; }
		else
		if (not isoverloadableoperator(info.name))
			obj[info.name] = Bind1st_(info.func, obj);
		else
			obj[info.name] = info.func;		
	}
	
	function EraseClassMembers_(obj){
		obj.new					= 
		obj.constructors		=
		obj.AddCtor_			=
		obj.AddField_			= 
		obj.fields				= 
		obj.AddMethod_			=
		obj.methods				= nil;
		return obj;
	}
	
	//****************************
							
	return [ 
		{ .class : id },
		
		{ .new : (method() {
					local ctor = @constructors[Sig_(arguments)];
					if (not ctor) 
						return nil;
					else {
						local obj = EraseClassMembers_(tabcopy(self));
												
						ListForEach_(@fields, Bind1st_(AddObjectField_, obj));
						ListForEach_(@methods, Bind1st_(AddObjectMethod_, obj));
						ctor(obj, |arguments|);
						
						return obj;
					}
				})},
		
		{.constructors : [ { .void : (function(t) {}) } ] },
		
		{.fields : list_new() },
		{.methods: list_new() },
		
		{.AddField_	: (method(field) { 
							@fields.push_back(field); 
							return self;
						})},
						
		
					
		{.AddCtor_	: (method(sig, ctor) {
							@constructors[sig] = ctor;
							return self;
						})},
						
		{.AddMethod_: (method(name, func) {
							@methods.push_back([{.name : name}, {.func : func} ]);
							return self;
						})}
		];						
}

/////////////////////////////////////////////////////////////////////

Point =	MakeClass_("Point").
			AddCtor_(
				"NumberNumber", 
				(function(p, x, y){ p.set_x(x); p.set_y(y); })
			).
			AddField_("x").AddField_("y").
			AddMethod_(
				"tostring()",
				(function(p) { return "(" + p.get_x() + "," + p.get_y() + ")"; })
			).
			AddMethod_(
				"move",
				(function(p, dx, dy){ p.set_x(dx + p.get_x()); p.set_y(dy + p.get_y()); })
			).
			AddMethod_(
				"=",
				(function(p1, p2){ p1.set_x(p2.get_x()); p1.set_y(p2.get_y()); })
			);
	
p1 = Point.new(-5,-7);	
print(typeof(p1));
p1.move(2,2);
print(p1, "\n"); // (-3,-5)

p2 = Point.new(10,20);
p1 = p2;
print(p1, "\n");  // (10,20)

/////////////////////////////////////////////////////////////////////
