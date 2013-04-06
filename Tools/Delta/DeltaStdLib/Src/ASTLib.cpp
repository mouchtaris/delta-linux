// ASTLib.cpp
// Garbage collectable ast type for delta.
// ScriptFighter Project.
// Y. Lilis, August 2011.
//

#include "DeltaLibraryCreators.h"
#include "ASTLib.h"
#include "ASTVisitorLib.h"
#include "ListLib.h"
#include "TreeNode.h"
#include "VMCompLib.h"

#include "ASTInjector.h"
#include "ASTTags.h"
#include "ASTMetaTags.h"
#include "ASTValidationVisitor.h"
#include "ASTSanitiseVisitor.h"

/////////////////////////////////////////////////////

static void	ToAST (DeltaValue* at, TreeNode* val);
static void AttributeToValue (DeltaValue* at, TreeAttribute* attr);
static bool ValueToAttribute (TreeAttribute** attr, DeltaValue& val, bool *numericConversionError = 0);
static void GetChildren(DeltaValue* at, TreeNode* ast);
static void GetAttributes(DeltaValue* at, TreeNode* ast);

static const std::string UnparseAST(TreeNode* val);

DLIB_REGAIN_COLLECTABLESIMPLE_FUNC(TreeNode, AST_TYPE_STR, AST_VAL)

/////////////////////////////////////////////////////

static DeltaExternIdFieldGetter* getter = (DeltaExternIdFieldGetter*) 0;

static bool GetKeys (void* val, DeltaValue* at) 
	{ return DPTR(getter)->GetAllKeys(val, at); }

//************************

static bool GetTag (void* val, DeltaValue* at) 
	{ at->FromString(AST_VAL(val)->GetTag()); return true; }

//************************

static bool GetAttributes (void* val, DeltaValue* at) {
	GetAttributes(at, AST_VAL(val));
	return true;
}

//************************

static bool GetChildren (void* val, DeltaValue* at) {
	GetChildren(at, AST_VAL(val));
	return true;
}

//************************

static bool GetParent (void* val, DeltaValue* at) {
	ToAST(at, AST_VAL(val)->GetParent());
	return true;
}

//************************

static DeltaExternIdFieldGetter::GetByStringFuncEntry getters[] = {
	{ "keys",		&GetKeys,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "tag",		&GetTag,		DELTA_GETBYSTRING_NO_PRECOND	},
	{ "attributes",	&GetAttributes,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "children",	&GetChildren,	DELTA_GETBYSTRING_NO_PRECOND	},
	{ "parent",		&GetParent,		DELTA_GETBYSTRING_NO_PRECOND	}
};

/////////////////////////////////////////////////////

static void ast_new_LibFunc (DeltaVirtualMachine*);
static void ast_get_tag_LibFunc (DeltaVirtualMachine*);
static void ast_get_child_LibFunc (DeltaVirtualMachine*);
static void ast_get_child_index_LibFunc (DeltaVirtualMachine*);
static void ast_get_children_LibFunc (DeltaVirtualMachine*);
static void ast_get_total_children_LibFunc (DeltaVirtualMachine*);
static void ast_get_parent_LibFunc (DeltaVirtualMachine*);
static void ast_is_descendant_LibFunc (DeltaVirtualMachine*);
static void ast_push_back_LibFunc (DeltaVirtualMachine*);
static void ast_push_front_LibFunc (DeltaVirtualMachine*);
static void ast_insert_after_LibFunc (DeltaVirtualMachine*);
static void ast_insert_before_LibFunc (DeltaVirtualMachine*);
static void ast_pop_back_LibFunc (DeltaVirtualMachine*);
static void ast_pop_front_LibFunc (DeltaVirtualMachine*);
static void ast_remove_LibFunc (DeltaVirtualMachine*);
static void ast_remove_from_parent_LibFunc (DeltaVirtualMachine*);
static void ast_replace_LibFunc (DeltaVirtualMachine*);
static void ast_get_attribute_LibFunc (DeltaVirtualMachine*);
static void ast_set_attribute_LibFunc (DeltaVirtualMachine*);
static void ast_get_attributes_LibFunc (DeltaVirtualMachine*);
static void ast_accept_postorder_LibFunc (DeltaVirtualMachine*);
static void ast_accept_preorder_LibFunc (DeltaVirtualMachine*);
static void ast_copy_LibFunc (DeltaVirtualMachine*);
static void ast_unparse_LibFunc (DeltaVirtualMachine*);
static void ast_escape_LibFunc (DeltaVirtualMachine*);
static void ast_decr_esc_cardinalities_LibFunc (DeltaVirtualMachine*);

static DeltaLibraryObjectCreator::FuncEntry funcs[] = {
	{ "new",					ast_new_LibFunc						},
	{ "get_tag",				ast_get_tag_LibFunc					},
	{ "get_child",				ast_get_child_LibFunc				},
	{ "get_child_index",		ast_get_child_index_LibFunc			},
	{ "get_children",			ast_get_children_LibFunc			},
	{ "get_total_children",		ast_get_total_children_LibFunc		},
	{ "get_parent",				ast_get_parent_LibFunc				},
	{ "is_descendant",			ast_is_descendant_LibFunc			},
	{ "push_back",				ast_push_back_LibFunc				},
	{ "push_front",				ast_push_front_LibFunc				},
	{ "insert_after",			ast_insert_after_LibFunc			},
	{ "insert_before",			ast_insert_before_LibFunc			},
	{ "pop_back",				ast_pop_back_LibFunc				},
	{ "pop_front",				ast_pop_front_LibFunc				},
	{ "remove",					ast_remove_LibFunc					},
	{ "remove_from_parent",		ast_remove_from_parent_LibFunc		},
	{ "replace",				ast_replace_LibFunc					},
	{ "get_attribute",			ast_get_attribute_LibFunc			},
	{ "set_attribute",			ast_set_attribute_LibFunc			},
	{ "get_attributes",			ast_get_attributes_LibFunc			},
	{ "accept_postorder",		ast_accept_postorder_LibFunc		},
	{ "accept_preorder",		ast_accept_preorder_LibFunc			},
	{ "copy",					ast_copy_LibFunc					},
	{ "unparse",				ast_unparse_LibFunc					},
	{ "escape",					ast_escape_LibFunc					},
	{ "decr_esc_cardinalities",	ast_decr_esc_cardinalities_LibFunc	}
};

inline bool VAL_IS_AST (const DeltaValue& val)
	{ return val.Type() == DeltaValue_ExternId && val.GetExternIdTypeString() == AST_TYPE_STR; }

DELTALIBFUNC_DECLARECONSTS(1, uarraysize(funcs) - 1, "get_tag", "decr_esc_cardinalities")

DELTALIBFUNC_DEFINE_METHODS_TABLE_AND_CALLER_CHECKER(VAL_IS_AST(val), AST_TYPE_STR)

DLIB_INITIALIZE_CLEANUP_FUNCS_EX(
	AST, 
	"ast",
	DELTA_STDLIB_NAMESPACE_PREFIX "ast_",
	DELTALIBFUNC_CREATE_METHODS_TABLE();,
	DELTALIBFUNC_DESTROY_METHODS_TABLE();
)

static void ToString (DeltaString* at, void* val) {
	CollectableSimple* v = VGET_INST(CollectableSimple, val, AST_TYPE_STR);
	DASSERT(v);
	const std::string text = UnparseAST((TreeNode*)DPTR(v)->GetValue());
	DPTR(at)->Add(uconstructstr("%s(0x%x)\"%s\"", AST_TYPE_STR, DPTR(v)->GetSerialNo(), text.c_str()));
}

/////////////////////////////////////////////////////

DVM_FUNC void RemoveRefCounters (TreeNode* node) {
	struct RefCounterRemover : public TreeVisitor {
		static void Handle_All (AST_VISITOR_ARGS) {
			if (entering && node->HasAttribute(AST_ATTRIBUTE_REF_COUNTER))
				node->RemoveAttribute(AST_ATTRIBUTE_REF_COUNTER);
		}
		void operator()(TreeNode* root) { if(root) DPTR(root)->AcceptPreOrder(this); }
		RefCounterRemover(void) { SetDefaultHandler(&Handle_All); }
	};
	RefCounterRemover()(node);
}

DVM_FUNC bool HasRefCounter (TreeNode* node) {
	struct HasRefCounter : public TreeVisitor {
		static void Handle_All (AST_VISITOR_ARGS) {
			if (entering && node->HasAttribute(AST_ATTRIBUTE_REF_COUNTER))
				((HasRefCounter*) closure)->Stop();
		}
		bool operator()(TreeNode* root) { DASSERT(root); return !DPTR(root)->AcceptPreOrder(this); }
		HasRefCounter(void) { SetDefaultHandler(&Handle_All, this); }
	};
	return HasRefCounter()(node);
}

//************************

static void IncRefCounter (TreeNode* val) {
	if (TreeAttribute *refCounter = val->GetAttribute(AST_ATTRIBUTE_REF_COUNTER))
		refCounter->SetUInt(refCounter->GetUInt() + 1);
	else
		val->SetAttribute(AST_ATTRIBUTE_REF_COUNTER, (util_ui32) 1);
}

static void DecRefCounter (TreeNode* val) {
	TreeAttribute *refCounter = val->GetAttribute(AST_ATTRIBUTE_REF_COUNTER);
	DASSERT(refCounter);
	util_ui32 count = refCounter->GetUInt();
	if (--count == 0) {
		val->RemoveAttribute(AST_ATTRIBUTE_REF_COUNTER);
		TreeNode* root = val->GetRoot();
		if (!HasRefCounter(root))
			TreeNode::Delete(root);
	}
	else
		refCounter->SetUInt(count);
}

/////////////////////////////////////////////////////

static void Destructor (void* val) { DecRefCounter((TreeNode*&) val); }

static void ToAST (DeltaValue* at, TreeNode* val) {
	DASSERT(at);
	if (val) {
		IncRefCounter(val);
		CollectableSimple* v = CollectableSimple::New(AST_TYPE_STR, val, &Destructor);
		DASSERT(v);
		DLIB_TOEXTERNID_VALUE(CollectableSimple, AST_TYPE_STR)
		DELTALIBFUNC_DELEGATE_METHODS(*at);
	}
	else
		at->FromNil();
}

/////////////////////////////////////////////////////

DVM_FUNC void DeltaAST_Make (DeltaValue* at, TreeNode* ast)
	{ ToAST(at, ast); }

DVM_FUNC TreeNode* DeltaAST_Get (DeltaValue& obj) {
	if (obj.Type() != DeltaValue_ExternId)
		return (TreeNode*) 0;
	else
		return DeltaAST_Get(DNULLCHECK(obj.ToExternId()), obj.GetExternIdTypeString());
}

DVM_FUNC TreeNode* DeltaAST_Get (void* val, const std::string& typeStr) 
	{ return AST_TYPE_STR != typeStr ? (TreeNode*) 0 : AST_VAL(val); }

/////////////////////////////////////////////////////
// Common error checks
//
#define CHECK_HAS_CHILDREN(ast, index)														\
	do {																					\
		if (!DPTR(ast)->GetTotalChildren()) {												\
			DPTR(vm)->PrimaryError("in '%s', arg #%d has no children", CURR_FUNC, index);	\
			DLIB_RESET_RETURN;																\
		}																					\
	} while(false)

#define CHECK_IS_CHILD(parent, index1, child, index2)															\
	do {																										\
		if (DPTR(child)->GetParent() != parent) {																\
			DPTR(vm)->PrimaryError("in '%s', arg #%d is not a child of arg #%d", CURR_FUNC, index2, index1);		\
			DLIB_RESET_RETURN;																					\
		}																										\
	} while(false)

#define CHECK_HAS_CHILD(parent, index, childIndex, printType, childIndexValue)			\
	do {																				\
		if (!DPTR(parent)->GetChild(childIndex)) {										\
			DPTR(vm)->PrimaryError(														\
				"in '%s', arg #%d does not have a child with index '" printType "'",	\
				CURR_FUNC,																\
				index,																	\
				childIndexValue															\
			);																			\
			DLIB_RESET_RETURN;															\
		}																				\
	} while(false)

#define CHECK_HAS_CHILD_WITH_STRING_INDEX(parent, index, childIndex)	\
	CHECK_HAS_CHILD(parent, index, childIndex, "%s", ucstringarg(childIndex))

#define CHECK_HAS_CHILD_WITH_NUMERIC_INDEX(parent, index, childIndex)	\
	CHECK_HAS_CHILD(parent, index, childIndex, "%d", childIndex)

#define CHECK_VALID_INSERTION(parent, index1, child, index2)								\
	do {																					\
		if (DPTR(child)->GetParent()) {														\
			DPTR(vm)->PrimaryError(															\
				"in '%s', arg #%d should be a root node but it has a parent",				\
				CURR_FUNC,																	\
				index2																		\
			);																				\
			DLIB_RESET_RETURN;																\
		}																					\
		else if (strcmp(DLIB_ARGVAL(id), "") && DPTR(parent)->GetChild(DLIB_ARGVAL(id)))	 {	\
			DPTR(vm)->PrimaryError(															\
				"in '%s', arg #%d already has a child with index '%s'",						\
				index1,																		\
				CURR_FUNC,																	\
				DLIB_ARGVAL(id)																\
			);																				\
			DLIB_RESET_RETURN;																\
		}																					\
	} while(false)

/////////////////////////////////////////////////////

DLIB_FUNC_START_OPT_ARGS(ast_new, 1, 1, Nil)
DLIB_ARG(const char*, tag)
DeltaTable* defaultTable = 0;
DLIB_OPT_ARG(DeltaTable*, attributes, defaultTable)
TreeNode* node = TreeNode::New(DLIB_ARGVAL(tag));
if (DLIB_ARGVAL(attributes))
	for (DeltaTable::const_iterator i = DLIB_ARGVAL(attributes)->begin(); i != DLIB_ARGVAL(attributes)->end(); ++i) {
		DeltaValue index, value;
		i->GetIndex(&index);
		//ignore non string index entries and skip the internal ref counter attribute
		if (!index.IsString() || index.ToString() == AST_ATTRIBUTE_REF_COUNTER)
			continue;
		i->GetContent(&value);
		DASSERT(!value.IsNil());
		TreeAttribute* attr;
		if (ValueToAttribute(&attr, value)) {	//again, ignore non-convertible entries
			node->SetAttribute(index.ToString(), *attr);
			DDELETE(attr);
		}
	}
ToAST(DLIB_RETVAL_PTR, node);
DLIB_SET_CREATOR_INFORMATION();
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_get_tag, 1, Empty)
DLIB_GET_AST
DLIB_RETVAL_REF.FromString(DPTR(ast)->GetTag());
DLIB_FUNC_END

//---------------------------------------------------

#define	IS_STRING_OR_NUMER(t)	((t) == DeltaValue_Number || (t) == DeltaValue_String)

DLIB_FUNC_START(ast_get_child, 2, Nil)
DLIB_GET_AST
DeltaValue* index;
DeltaGetOverloadedArg(index, 1, IS_STRING_OR_NUMER, "String or Number", CURR_FUNC, DLIB_RESET_RETURN;);

TreeNode* child = (TreeNode*) 0;
if (index->IsString())
	child = DPTR(ast)->GetChild(index->ToString());
else {
	DASSERT(index->IsNumber());
	DeltaNumberValueType num = index->ToNumber();
	if (num < 0) {
		DPTR(vm)->PrimaryError(
			"unsigned expected (negative '%d' passed) in '%s'",
			(util_i32) num,
			CURR_FUNC
		);
		DLIB_RESET_RETURN;
	}
	else if ((util_ui32) num >= DPTR(ast)->GetTotalChildren()) {
		DPTR(vm)->PrimaryError(
			"in '%s', child index '%d' exceeds total children '%d'",
			CURR_FUNC,
			(util_i32) num,
			DPTR(ast)->GetTotalAttributes()
		);
		DLIB_RESET_RETURN;
	}
	else
		child = DPTR(ast)->GetChild((util_ui32) num);
}
ToAST(DLIB_RETVAL_PTR, child);
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_get_child_index, 2, Nil)
DLIB_GET_AST
DLIB_GET_AST_EX(child)
CHECK_HAS_CHILDREN(ast, 0);
CHECK_IS_CHILD(ast, 0, child, 1);
const TreeNode::Index index = DPTR(ast)->GetChildIndex(child);
DeltaObject* object = DeltaObject::NewObject();
DLIB_RETVAL_REF.FromTable(object);
object->Set("index", (DeltaNumberValueType) index.first);
object->Set("id", index.second);
DLIB_FUNC_END

//---------------------------------------------------

static void GetChildren(DeltaValue* at, TreeNode* ast) {
	DeltaList_Make(*at);
	std::list<DeltaValue>* l = DeltaList_Get(*at);
	for (util_ui32 i = 0; i < DPTR(ast)->GetTotalChildren(); ++i) {
		DeltaValue child;
		ToAST(&child, DPTR(ast)->GetChild(i)); 
		l->push_back(child);
	}
}

DLIB_FUNC_START(ast_get_children, 1, Nil)
DLIB_GET_AST
GetChildren(DLIB_RETVAL_PTR, ast);
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_get_total_children, 1, Zero)
DLIB_GET_AST
DLIB_RETVAL_REF.FromNumber(DPTR(ast)->GetTotalChildren());
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_get_parent, 1, Nil)
DLIB_GET_AST
ToAST(DLIB_RETVAL_PTR, DPTR(ast)->GetParent());
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_is_descendant, 2, False)
DLIB_GET_AST
DLIB_GET_AST_EX(node)
DLIB_RETVAL_REF.FromBool(DPTR(ast)->IsDescendant(node));
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START_OPT_ARGS(ast_push_back, 2, 1, Empty)
DLIB_GET_AST
DLIB_GET_AST_EX(node)
DLIB_OPT_ARG(const char*, id, "")
CHECK_VALID_INSERTION(ast, 0, node, 1);
DPTR(ast)->PushBack(node, DLIB_ARGVAL(id));
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START_OPT_ARGS(ast_push_front, 2, 1, Empty)
DLIB_GET_AST
DLIB_GET_AST_EX(node)
DLIB_OPT_ARG(const char*, id, "")
CHECK_VALID_INSERTION(ast, 0, node, 1);
DPTR(ast)->PushFront(node, DLIB_ARGVAL(id));
DLIB_FUNC_END

//---------------------------------------------------

#define	IS_STRING_NUMER_OR_EXTERNID(t)	((t) == DeltaValue_Number || (t) == DeltaValue_String || (t) == DeltaValue_ExternId)

#define INSERT_AT_POS(pos, func)					\
	DLIB_GET_AST_EX(node)							\
	DLIB_OPT_ARG(const char*, id, "")				\
	CHECK_VALID_INSERTION(ast, 0, node, 2);			\
	DPTR(ast)->func(pos, node, DLIB_ARGVAL(id))

#define INSERT_FUNC_IMPL(name, pos, func)			\
	DLIB_FUNC_START_OPT_ARGS(name, 3, 1, Empty)		\
	DLIB_GET_AST									\
	DeltaValue* position;							\
	DeltaGetOverloadedArg(							\
		position,									\
		1,											\
		IS_STRING_NUMER_OR_EXTERNID,				\
		"String, Number or ExternId",				\
		CURR_FUNC,									\
		DLIB_RESET_RETURN;							\
	);												\
	if (position->IsString()) {						\
		DLIB_ARG(const char*, pos)					\
		INSERT_AT_POS(DLIB_ARGVAL(pos), func);		\
	}												\
	else if (position->IsNumber()) {					\
		DLIB_ARG(util_ui32, pos)					\
		INSERT_AT_POS(DLIB_ARGVAL(pos), func);		\
	}												\
	else {											\
		DLIB_GET_AST_EX(pos)						\
		CHECK_IS_CHILD(ast, 0, pos, 1);				\
		INSERT_AT_POS(pos, func);					\
	}												\
	DLIB_FUNC_END

INSERT_FUNC_IMPL(ast_insert_after, after, InsertAfter)
INSERT_FUNC_IMPL(ast_insert_before, before, InsertBefore)

//---------------------------------------------------

DLIB_FUNC_START(ast_pop_back, 1, Empty)
DLIB_GET_AST
CHECK_HAS_CHILDREN(ast, 0);
DPTR(ast)->PopBack();
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_pop_front, 1, Empty)
DLIB_GET_AST
CHECK_HAS_CHILDREN(ast, 0);
DPTR(ast)->PopFront();
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_remove, 2, Empty)
DLIB_GET_AST
CHECK_HAS_CHILDREN(ast, 0);
DeltaValue* index;
DeltaGetOverloadedArg(index, 1, IS_STRING_NUMER_OR_EXTERNID, "String, Number or ExternId", CURR_FUNC, DLIB_RESET_RETURN;);
if (index->IsString()) {
	CHECK_HAS_CHILD_WITH_STRING_INDEX(ast, 0, index->ToString());
	DPTR(ast)->Remove(index->ToString());
}
else if (index->IsNumber()) {
	DeltaNumberValueType num = index->ToNumber();
	if (num < 0) {
		DPTR(vm)->PrimaryError(
			"unsigned expected (negative '%d' passed) in '%s'",
			(util_i32) num,
			CURR_FUNC
		);
		DLIB_RESET_RETURN;
	}
	else {
		CHECK_HAS_CHILD_WITH_NUMERIC_INDEX(ast, 0, (util_ui32) num);
		DPTR(ast)->Remove((util_ui32) num);
	}
}
else {
	DLIB_GET_AST_EX(child)
	CHECK_IS_CHILD(ast, 0, child, 1);
	DPTR(ast)->Remove(child);
}
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_remove_from_parent, 1, Empty)
DLIB_GET_AST
if (!DPTR(ast)->GetParent())
	DPTR(vm)->PrimaryError("in '%s', arg #0 does not have a parent",	 CURR_FUNC);
else
	DPTR(ast)->RemoveFromParent();
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_replace, 3, Empty)
DLIB_GET_AST
DLIB_GET_AST_EX(oldNode)
DLIB_GET_AST_EX(newNode)
CHECK_IS_CHILD(ast, 0, oldNode, 1);
DPTR(ast)->Replace(oldNode, newNode);
DLIB_FUNC_END

//---------------------------------------------------

static void AttributeToValue (DeltaValue* at, TreeAttribute* attr) {
	if (attr)
		switch(attr->GetType()) {
			case AttributeTypeInt:			at->FromNumber(attr->GetInt());		break;
			case AttributeTypeUInt:			at->FromNumber(attr->GetUInt());		break;
			case AttributeTypeDouble:		at->FromNumber(attr->GetDouble());	break;
			case AttributeTypeBool:			at->FromBool(attr->GetBool());		break;
			case AttributeTypeString:		at->FromString(attr->GetString());	break;
			case AttributeTypeStringList:	{
				DeltaList_Make(*at);
				std::list<DeltaValue>* l = DeltaList_Get(*at);
				const std::list<std::string>& strs = attr->GetStringList();
				for (std::list<std::string>::const_iterator i = strs.begin(); i != strs.end(); ++i)
					l->push_back(DeltaValue(*i));
				break;
			}
			case AttributeTypePointer:		at->FromExternId(attr->GetPointer());	break;
			default:						DASSERT(false);
		}
	else
		at->FromNil();
}

//---------------------------------------------------

static bool IsValidNumericType (const std::string& type) {
	const char* types[] = { "int", "uint", "double" };
	const char** end = types + uarraysize(types);
	return std::find(types, end, type) != end;
}

static TreeAttribute* CreateNumericAttribute(double value, const std::string& type) {
	if (type == "int")
		return DNEWCLASS(TreeAttribute, ((util_i32) value));
	else if (type == "uint")
		return DNEWCLASS(TreeAttribute, ((util_ui32) value));
	else {
		DASSERT(type == "double");
		return DNEWCLASS(TreeAttribute, (value));
	}
}

static bool ValueToAttribute (TreeAttribute** attr, DeltaValue& val, bool *numericConversionError) {
	DASSERT(attr);
	switch(val.Type()) {
		case DeltaValue_Number: {
									std::string type = val.GetTypeTag();
									if (type.empty())
										type = "double";
									if (!IsValidNumericType(type)) {
										if (numericConversionError)
											*numericConversionError = true;
										return false;
									}
									*attr = CreateNumericAttribute(val.ToNumber(), type);
									break;
								}
		case DeltaValue_String:		*attr = DNEWCLASS(TreeAttribute, (val.ToString()));		break;
		case DeltaValue_Bool:		*attr = DNEWCLASS(TreeAttribute, (val.ToBool()));		break;
		case DeltaValue_ExternId:	if (std::list<DeltaValue>* l = DeltaList_Get(val)) {
										std::list<std::string> strs;
										for (std::list<DeltaValue>::const_iterator i = l->begin(); i != l->end(); ++i)
											if (i->IsString())
												strs.push_back(i->ToString());
										*attr = DNEWCLASS(TreeAttribute, (strs));
									}
									else
										*attr = DNEWCLASS(TreeAttribute, (val.ToExternId()));
									break;

		default:					return false;
	}
	return true;
}

//---------------------------------------------------

DLIB_FUNC_START(ast_get_attribute, 2, Nil)
DLIB_GET_AST
DLIB_ARG(const char*, id)
if (!strcmp(DLIB_ARGVAL(id), AST_ATTRIBUTE_REF_COUNTER))	//do not expose the internal ref counter attribute
	DLIB_RETVAL_REF.FromNil();
else
	AttributeToValue(DLIB_RETVAL_PTR, DPTR(ast)->GetAttribute(DLIB_ARGVAL(id)));
DLIB_FUNC_END

//---------------------------------------------------

static void GetAttributes (DeltaValue* at, TreeNode* ast) {
	const TreeNode::Attrs& attrs = ast->GetAttributes();
	DeltaObject* object = DeltaObject::NewObject();
	at->FromTable(object);
	for (TreeNode::Attrs::const_iterator i = attrs.begin(); i != attrs.end(); ++i)
		if (i->first != AST_ATTRIBUTE_REF_COUNTER) {	//skip the internal ref counter attribute
			DeltaValue attribute;
			AttributeToValue(&attribute, i->second);
			object->Set(i->first, attribute);
		}
}

DLIB_FUNC_START(ast_get_attributes, 1, Nil)
DLIB_GET_AST
GetAttributes(DLIB_RETVAL_PTR, ast);
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_set_attribute, 3, False)
DLIB_GET_AST
DLIB_ARG(const char*, id)
DLIB_ARG(DeltaValue*, val)
if (!strcmp(DLIB_ARGVAL(id), AST_ATTRIBUTE_REF_COUNTER))	//do not expose the internal ref counter attribute
	DLIB_RESET_RETURN;
if (DLIB_ARGVAL(val)->IsNil())
	DPTR(ast)->RemoveAttribute(DLIB_ARGVAL(id));
else {
	TreeAttribute* attr;
	bool numericConversionError = false;
	if (ValueToAttribute(&attr, *DLIB_ARGVAL(val), &numericConversionError)) {
		DPTR(ast)->SetAttribute(DLIB_ARGVAL(id), *attr);
		DDELETE(attr);
	}
	else {
		if (numericConversionError)
			DPTR(vm)->PrimaryError(
				"in %s, invalid typetag for arg 'type', expected empty, 'int', 'uint' or 'double' (passed %s)",
				CURR_FUNC,
				ucstringarg(DLIB_ARGVAL(val)->GetTypeTag())
			);
		else
			DPTR(vm)->PrimaryError(
				DEFORMAT_ARGTYPE,
				2,
				"String, Number, Bool, Nil or ExternId",
				DLIB_ARGVAL(val)->TypeStr(),
				CURR_FUNC
			);
		DLIB_RESET_RETURN;
	}
}
DLIB_RETVAL_REF.FromBool(true);
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_accept_postorder, 2, Empty)
DLIB_GET_AST
DLIB_GET_ASTVISITOR
DeltaValue visitorCopy(*DPTR(vm)->GetActualArg(1));	//to ensure the visitor is not collected during traversal
DPTR(ast)->AcceptPostOrder(visitor);
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_accept_preorder, 2, Empty)
DLIB_GET_AST
DLIB_GET_ASTVISITOR
DeltaValue visitorCopy(*DPTR(vm)->GetActualArg(1));	//to ensure the visitor is not collected during traversal
DPTR(ast)->AcceptPreOrder(visitor);
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_copy, 1, Nil)
DLIB_GET_AST
TreeNode* copy = DPTR(ast)->Clone();
RemoveRefCounters(copy);
ToAST(DLIB_RETVAL_PTR, copy);
DLIB_FUNC_END

//---------------------------------------------------

static const std::string UnparseAST(TreeNode* ast) {
	CompilerIFace* compiler = CompilerIFace::New();
	const std::string result = DPTR(compiler)->Unparse(ast);
	CompilerIFace::Delete(DPTR(compiler));
	return result;
}

DLIB_FUNC_START(ast_unparse, 1, Nil)
DLIB_GET_AST
DLIB_RETVAL_REF.FromString(UnparseAST(ast));
DLIB_FUNC_END

//---------------------------------------------------

#define	AST_ATTRIBUTE_INJECTED		"injected"

static TreeNode* CreateASTNode (const std::string& tag, void*) { return TreeNode::New(tag); }

//For non-root nodes, the ExternId holding the ast will continue to exist after the injection, so delete them normally.
//For root nodes, the return value of ast_inject will be nil, meaning that the 
//ExternId will be collected upon next getretval, so skip deletion here.
static void DeleteASTNode (TreeNode*& node) { if (DPTR(node)->GetParent()) TreeNode::Delete(node); }

static TreeNode* Extend(TreeNode* child, const std::string& tag, const std::string& childId) {
	TreeNode* node = TreeNode::New(tag);
	DPTR(node)->PushBack(child, childId);
	return node;
}

DLIB_FUNC_START(ast_escape, 2, Nil)
DLIB_GET_AST
TreeNode* node = (TreeNode*) 0;
DeltaValue* nodeArg = vm->GetActualArg(1);
DeltaValueType type = nodeArg->Type();
switch(type) {
	case DeltaValue_Nil:		break;

	case DeltaValue_Number:		node = TreeNode::New(AST_TAG_NUM_CONST);
								node->SetAttribute(AST_ATTRIBUTE_CONSTVALUE, nodeArg->ToNumber());
								break;

	case DeltaValue_Bool:		node = TreeNode::New(AST_TAG_BOOL_CONST);
								node->SetAttribute(AST_ATTRIBUTE_CONSTVALUE, nodeArg->ToBool());
								break;

	case DeltaValue_String: 	node = TreeNode::New(AST_TAG_STRING_CONST);
								node->SetAttribute(AST_ATTRIBUTE_ITEMS, std::list<std::string>(1, nodeArg->ToString()));
								break;
	
	case DeltaValue_ExternId:	{ DLIB_GET_AST_EX(n); node = n->Clone(); RemoveRefCounters(node); }
								break;

	default:					DPTR(vm)->PrimaryError(
									DEFORMAT_ARGTYPE,
									1,
									"String, Number, Bool, Nil or ExternId",
									nodeArg->TypeStr(),
									CURR_FUNC
								);
								DLIB_RESET_RETURN;
}

if (type == DeltaValue_Number || type == DeltaValue_Bool || type == DeltaValue_String)
	node = Extend(node, AST_TAG_PRIMARY_EXPRESSION, AST_CHILD_EXPR);

bool first = true;
AST::ValidationVisitor nodeValidator(true);
nodeValidator.EnterQuotes();
if (!nodeValidator(DPTR(ast)) || (first = false) || node && !nodeValidator(DPTR(node))) {
	DPTR(vm)->PrimaryError(
		"in %s, arg #%d is not a valid ast: %s",
		CURR_FUNC,
		first ? 0 : 1,
		nodeValidator.GetValidationError().c_str()
	);
	DLIB_RESET_RETURN;
}

class EscapeLocator : public TreeVisitor {
	std::list<TreeNode*> nodes;
	bool forTarget;
	static void Handle_Escape (AST_VISITOR_ARGS) {
		if (entering) {
			EscapeLocator* visitor = (EscapeLocator*) closure;
			if (!visitor->forTarget)
				visitor->nodes.push_back(node);
			else if (!DPTR(node)->HasAttribute(AST_ATTRIBUTE_INJECTED) && CARDINALITY(node) == 1) {
				visitor->nodes.push_back(node);
				visitor->Stop();
			}
		}
	}
	public:
	std::list<TreeNode*> operator()(TreeNode* root) { if (root) DPTR(root)->AcceptPreOrder(this); return nodes; }
	EscapeLocator(bool forTarget) : forTarget(forTarget) { SetHandler(AST_TAG_ESCAPE, &Handle_Escape, this); }
};

std::list<TreeNode*> escapes = EscapeLocator(true)(ast);
DASSERT(escapes.empty() || escapes.size() == 1);
if (escapes.empty()) {
	DPTR(vm)->PrimaryError(
		"in %s, given ast has no escapes",
		CURR_FUNC
	);
	DLIB_RESET_RETURN;
}

std::list<TreeNode*> nodeEscapes = EscapeLocator(false)(node);
for (std::list<TreeNode*>::const_iterator i = nodeEscapes.begin(); i != nodeEscapes.end(); ++i) {
	DASSERT((*i)->GetTag() == AST_TAG_ESCAPE);
	(*i)->SetAttribute(AST_ATTRIBUTE_INJECTED, true);
}

TreeNode* result = ast;
ASTInjector(umakecallback(CreateASTNode, (void*) 0), &DeleteASTNode)(escapes.front(), node, &result);
if (result) {
	AST::SanitiseVisitor()(result);
	AST::ValidationVisitor validator(true);
	validator.EnterQuotes();
	if (!validator(result)) {
		DPTR(vm)->PrimaryError(
			"in %s, invalid escape: %s",
			CURR_FUNC,
			validator.GetValidationError().c_str()
		);
		DLIB_RESET_RETURN;
	}
	ToAST(DLIB_RETVAL_PTR, result);
}
else
	DLIB_RETVAL_REF.FromNil();
DLIB_FUNC_END

//---------------------------------------------------

DLIB_FUNC_START(ast_decr_esc_cardinalities, 1, Empty)
if (vm->GetActualArg(0)->IsNil())	//do nothing on empty asts
	DLIB_RESET_RETURN;
DLIB_GET_AST
AST::ValidationVisitor nodeValidator(true);
nodeValidator.EnterQuotes();
if (!nodeValidator(ast)) {
	DPTR(vm)->PrimaryError(
		"in %s, arg #0 is not a valid ast: %s",
		CURR_FUNC,
		nodeValidator.GetValidationError().c_str()
	);
	DLIB_RESET_RETURN;
}

class EscapeCardinalityHandler : public TreeVisitor {
	bool status;
	static void Handle_Escape (AST_VISITOR_ARGS) {
		if (entering) {
			if (DPTR(node)->HasAttribute(AST_ATTRIBUTE_INJECTED))
				DPTR(node)->RemoveAttribute(AST_ATTRIBUTE_INJECTED);
			else {
				TreeAttribute* cardinality = DPTR(node)->GetAttribute(AST_ATTRIBUTE_CARDINALITY);
				DASSERT(cardinality && cardinality->IsUInt() && cardinality->GetUInt() >= 1);
				util_ui32 val = cardinality->GetUInt();
				if (val == 1) {
					((EscapeCardinalityHandler*) closure)->status = false;
					((EscapeCardinalityHandler*) closure)->Stop();
					return;
				}
				else
					cardinality->SetUInt(val - 1);
			}
			((EscapeCardinalityHandler*) closure)->Leave();
		}
	}
	public:
	bool operator()(TreeNode* root) { if (root) DPTR(root)->AcceptPreOrder(this); return status; }
	EscapeCardinalityHandler() : status(true) { SetHandler(AST_TAG_ESCAPE, &Handle_Escape, this); }
};
if (!EscapeCardinalityHandler()(ast)) {
	DPTR(vm)->PrimaryError("in %s, given ast contains normal escapes (cardinality == 1)", CURR_FUNC);
	DLIB_RESET_RETURN;
}
DLIB_FUNC_END

//////////////////////////////////////////////////////
