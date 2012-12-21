//////////////////////////////////////////////////////////////////////////////////
// Generic Undo-Redo pattern as a metaprogram
//

&using std;
using std;

//////////////////////////////////////////////////////////////////////////////////
// Utility metafunctions
//
&function id_new(name) { return std::ast_new("Name", [@name : name]); }

&function expr_new(name) {
	local lvalue = ast_new("LvalueIdent");
	lvalue.push_back(id_new(name), "name");
	local primaryExpr = ast_new("PrimaryExpression"); 
	primaryExpr.push_back(lvalue, "expr"); 
	return primaryExpr;
}

//////////////////////////////////////////////////////////////////////////////////
// Implementation of function generators for the undo-redo operations
//
&function OperationCreator(name, members, type) {
	local args = nil;
	local data = nil;
	foreach(local item, members) {
		local expr = expr_new(item);
		args = <<~args, ~expr>>;
		data = <<~data, { ~item : ~expr }>>;
	}
	return <<method ~(id_new(name))(~args){ return [ @type : ~type, @data: [~data] ]; }>>;
}

&function OperationPerformer(func, members) {
	local args = nil;
	foreach(local item, members)
		args = <<~args, data[~item]>>;
	return <<function (object, data){ object[[~func]](~args); }>>;
}

&function OverrideFunc(name, operation)
	{ return <<method ~(id_new(name))(...) { @newaction(self[~operation](...)); }>>; }

//////////////////////////////////////////////////////////////////////////////////
// Implementation of the UndoRedo pattern through subclassing a base API and
// overriding the required methods to become aware of the Undo/Redo.
// The code generated is based on the given specification:
// i)  Concept of undoable operation (op, data, opposite, functionality)
// ii) Mapping of operations to API methods
// (see example below for usage)
//
&function UndoRedo(operations, funcMappings) {
	local opCounter = 0;
	local opposites = nil;
	local creators = nil;
	local performers = nil;
	foreach(local op, operations) {
		local type = opCounter++;
		local creator = OperationCreator(op.name, op.data, type);
		local performer = OperationPerformer(op.performer, op.data);
		
		opposites = <<~opposites, {~type : ~(op.opposite)}>>;
		creators = <<~creators, ~creator>>;
		performers = <<~performers, {~type : ~performer}>>;
	}
	
	local overrideFuncs = nil;
	foreach(local mapping, funcMappings)
		overrideFuncs = <<~overrideFuncs, ~(OverrideFunc(mapping.func, mapping.operation))>>;
		
	return <<
		[
			@opposites : [~opposites],
			@actions : [~performers],
			@undoStack : list_new(),
			@redoStack : list_new(),
			
			method undo {
				if (@undoStack.total() > 0) {
					local action = @undoStack.pop_front();
					@redoStack.push_front(@opposite(action));
					@perform(action);
				}
			},
			method redo {
				if (@redoStack.total() > 0) {
					local action = @redoStack.pop_front();
					@undoStack.push_front(@opposite(action));
					@perform(action);
				}
			},

			method opposite(action) {
				assert @opposites[action.type];
				return @opposites[action.type](self, action.data);
			},
			method perform(action) {
				assert @actions[action.type];
				@actions[action.type](std::getbase(self, 0), action.data);
			},
			method newaction(action) {
				@undoStack.push_front(@opposite(action));
				@perform(action);
				@redoStack.clear();
			},
			~creators,
			~overrideFuncs
		]
	>>;
}

//////////////////////////////////////////////////////////////////////////////////
// Example using the UndoRedo pattern on a text editor
//
baseEditor = [
	@text : "",
	@caret : 0,
	method Display(msg) { print(msg, @text, "\n"); },
	method SetText(text) { @text = text; },
	method AddText(pos, text) {
		local finalText = text + strslice(@text, pos, 0);
		if (pos)
			finalText = strslice(@text, 0, pos) + finalText;
		@SetText(finalText);
	},
	method DeleteText(pos, len) {
		local finalText = strslice(@text, pos + len, 0);
		if (pos)
			finalText = strslice(@text, 0, pos) + finalText;
		@SetText(finalText);
	}
];

//Undoable methods: AddText, DeleteText, SetText
//Operations: Insert(pos, text), Delete(pos, len), Set(text)
//AddText	(pos, text)	-> Insert(pos, text)
//DeleteText(pos, len)	-> Delete(pos, len)
//SetText	(text)		-> Set(text)

undoRedoEditor = !(UndoRedo(
	[
		[
			@name : "Insert",
			@data : list_new("pos", "text"),
			@opposite : <<
				function (object, data)
					{ return object.Delete(data.pos, strlen(data.text)); }
			>>,
			@performer : "AddText"
		],
		[
			@name : "Delete",
			@data : list_new("pos", "len"),
			@opposite : <<
				function (object, data) {
					local pos = data.pos;
					return object.Insert(pos, strslice(object.text, pos, pos + data.len - 1));
				}
			>>,
			@performer : "DeleteText"
		],
		[
			@name : "Set",
			@data : list_new("text"),
			@opposite : <<
				function (object, data) { return object.Set(object.text); }
			>>,
			@performer : "SetText"
		]
	],
	[
		[ @func : "AddText", @operation : "Insert"],
		[ @func : "DeleteText", @operation : "Delete"],
		[ @func : "SetText", @operation : "Set"]
	]
));	//Apply the pattern
std::inherit(undoRedoEditor, baseEditor);

editor = undoRedoEditor;
editor.SetText("Hello world!");

editor.DeleteText(0, 5);
editor.AddText(0, "Bye cruel");

editor.Display("Final: ");
editor.undo();
editor.Display("Intermediate: ");
editor.undo();
editor.Display("Original: ");
editor.undo();
editor.Display("Initial: ");
editor.redo();
editor.redo();
editor.redo();
editor.Display("Final again: ");
