/////////////////////////////////////////////////////
// This file is directly included in DeltaValue body.
// A. Savidis, Original version: October 1999.
// Updated June 2009, introduced Operations friend nested class since
// friend static functions are not allowed.


/////////////////////////////////////////////////////

friend class	DeltaTable;
friend class	DeltaVirtualMachine;
friend class	CollectableContainer;
friend struct	Operations;

/////////////////////////////////////////////////////

public:

#define	DELTAVALUE_OPERATION					\
	DeltaValue::Operations::

#define	CALL_FUNC_ARGS							\
	DeltaValue*					func,			\
	DeltaVirtualMachine*		vm

#define	RELATIONAL_FUNC_ARGS					\
	DeltaValue*						lhs,		\
	DeltaValue*						rhs,		\
	DeltaRelOpType					relOp,		\
	DeltaRelationalErrorHandler*	handler,	\
	DeltaVirtualMachine*			vm,			\
	void**							cache

 #define	ARITHMETIC_FUNC_ARGS					\
	DeltaValue*						lhs,		\
	DeltaValue*						rhs,		\
	DeltaValue*						result,		\
	DeltaArithOpType				arithOp,	\
	DeltaArithmeticErrorHandler*	handler,	\
	DeltaVirtualMachine*			vm,			\
	void**							cache

struct Operations {

static void Callfunc_Libfunc				(CALL_FUNC_ARGS);
static void Callfunc_String					(CALL_FUNC_ARGS);
static void Callfunc_ProgramFunc			(CALL_FUNC_ARGS);
static void Callfunc_Table					(CALL_FUNC_ARGS);
static void Callfunc_Table					(DeltaTable* table, DeltaVirtualMachine* vm);
static void Callfunc_ExternId				(CALL_FUNC_ARGS);
static void Callfunc_MethodFunc				(CALL_FUNC_ARGS);
static void Callfunc_Error					(CALL_FUNC_ARGS);

//***********************

static bool NumberEQNumber 					(RELATIONAL_FUNC_ARGS);
static bool NumberNENumber 					(RELATIONAL_FUNC_ARGS);
static bool NumberGTNumber 					(RELATIONAL_FUNC_ARGS);
static bool NumberGENumber 					(RELATIONAL_FUNC_ARGS);
static bool NumberLTNumber 					(RELATIONAL_FUNC_ARGS);
static bool NumberLENumber 					(RELATIONAL_FUNC_ARGS);
static bool NumberEQNumberCached 			(RELATIONAL_FUNC_ARGS);
static bool NumberNENumberCached 			(RELATIONAL_FUNC_ARGS);
static bool NumberGTNumberCached 			(RELATIONAL_FUNC_ARGS);
static bool NumberGENumberCached 			(RELATIONAL_FUNC_ARGS);
static bool NumberLTNumberCached 			(RELATIONAL_FUNC_ARGS);
static bool NumberLENumberCached 			(RELATIONAL_FUNC_ARGS);
static bool NumberEQBool					(RELATIONAL_FUNC_ARGS);
static bool NumberNEBool					(RELATIONAL_FUNC_ARGS);
static bool NumberEQBoolCached				(RELATIONAL_FUNC_ARGS);
static bool NumberNEBoolCached				(RELATIONAL_FUNC_ARGS);

static bool StringEQString 					(RELATIONAL_FUNC_ARGS);
static bool StringNEString 					(RELATIONAL_FUNC_ARGS);
static bool StringGTString 					(RELATIONAL_FUNC_ARGS);
static bool StringGEString 					(RELATIONAL_FUNC_ARGS);
static bool StringLTString 					(RELATIONAL_FUNC_ARGS);
static bool StringLEString 					(RELATIONAL_FUNC_ARGS);
static bool StringEQStringCached 			(RELATIONAL_FUNC_ARGS);
static bool StringNEStringCached 			(RELATIONAL_FUNC_ARGS);
static bool StringGTStringCached 			(RELATIONAL_FUNC_ARGS);
static bool StringGEStringCached			(RELATIONAL_FUNC_ARGS);
static bool StringLTStringCached 			(RELATIONAL_FUNC_ARGS);
static bool StringLEStringCached 			(RELATIONAL_FUNC_ARGS);
static bool StringEQBool 					(RELATIONAL_FUNC_ARGS);
static bool StringNEBool 					(RELATIONAL_FUNC_ARGS);
static bool StringEQBoolCached 				(RELATIONAL_FUNC_ARGS);
static bool StringNEBoolCached 				(RELATIONAL_FUNC_ARGS);

static bool TableEQTable 					(RELATIONAL_FUNC_ARGS);
static bool TableNETable 					(RELATIONAL_FUNC_ARGS);

static bool ProgramFuncEQProgramFunc 		(RELATIONAL_FUNC_ARGS);
static bool ProgramFuncNEProgramFunc 		(RELATIONAL_FUNC_ARGS);
static bool ProgramFuncEQProgramFuncCached 	(RELATIONAL_FUNC_ARGS);
static bool ProgramFuncNEProgramFuncCached 	(RELATIONAL_FUNC_ARGS);
static bool ProgramFuncEQBool 				(RELATIONAL_FUNC_ARGS);
static bool ProgramFuncNEBool 				(RELATIONAL_FUNC_ARGS);
static bool ProgramFuncEQBoolCached 		(RELATIONAL_FUNC_ARGS);
static bool ProgramFuncNEBoolCached 		(RELATIONAL_FUNC_ARGS);

static bool MethodFuncEQMethodFunc 			(RELATIONAL_FUNC_ARGS);
static bool MethodFuncNEMethodFunc 			(RELATIONAL_FUNC_ARGS);
static bool MethodFuncEQMethodFuncCached  	(RELATIONAL_FUNC_ARGS);
static bool MethodFuncNEMethodFuncCached  	(RELATIONAL_FUNC_ARGS);
static bool MethodFuncEQBool 				(RELATIONAL_FUNC_ARGS);
static bool MethodFuncNEBool 				(RELATIONAL_FUNC_ARGS);
static bool MethodFuncEQBoolCached 			(RELATIONAL_FUNC_ARGS);
static bool MethodFuncNEBoolCached 			(RELATIONAL_FUNC_ARGS);

static bool LibraryFuncEQLibraryFunc 		(RELATIONAL_FUNC_ARGS);
static bool LibraryFuncNELibraryFunc 		(RELATIONAL_FUNC_ARGS);
static bool LibraryFuncEQLibraryFuncCached  (RELATIONAL_FUNC_ARGS);
static bool LibraryFuncNELibraryFuncCached  (RELATIONAL_FUNC_ARGS);
static bool LibraryFuncEQBool 				(RELATIONAL_FUNC_ARGS);
static bool LibraryFuncNEBool 				(RELATIONAL_FUNC_ARGS);
static bool LibraryFuncEQBoolCached  		(RELATIONAL_FUNC_ARGS);
static bool LibraryFuncNEBoolCached  		(RELATIONAL_FUNC_ARGS);

static bool BoolEQNumber					(RELATIONAL_FUNC_ARGS);
static bool BoolNENumber					(RELATIONAL_FUNC_ARGS);
static bool BoolEQString					(RELATIONAL_FUNC_ARGS);
static bool BoolNEString					(RELATIONAL_FUNC_ARGS);
static bool BoolEQTable						(RELATIONAL_FUNC_ARGS);	// No caching
static bool BoolNETable						(RELATIONAL_FUNC_ARGS);	// No caching
static bool BoolEQProgramFunc				(RELATIONAL_FUNC_ARGS);
static bool BoolNEProgramFunc				(RELATIONAL_FUNC_ARGS);
static bool BoolEQMethodFunc				(RELATIONAL_FUNC_ARGS);
static bool BoolNEMethodFunc				(RELATIONAL_FUNC_ARGS);
static bool BoolEQLibraryFunc				(RELATIONAL_FUNC_ARGS);
static bool BoolNELibraryFunc				(RELATIONAL_FUNC_ARGS);
static bool BoolEQBool						(RELATIONAL_FUNC_ARGS);
static bool BoolNEBool						(RELATIONAL_FUNC_ARGS);
static bool BoolEQExternId					(RELATIONAL_FUNC_ARGS);	// No caching
static bool BoolNEExternId					(RELATIONAL_FUNC_ARGS);	// No caching
static bool BoolEQNil						(RELATIONAL_FUNC_ARGS);
static bool BoolNENil						(RELATIONAL_FUNC_ARGS);
static bool BoolEQUndef						(RELATIONAL_FUNC_ARGS);
static bool BoolNEUndef						(RELATIONAL_FUNC_ARGS);
static bool NilEQBool						(RELATIONAL_FUNC_ARGS);
static bool NilNEBool						(RELATIONAL_FUNC_ARGS);
static bool UndefEQBool						(RELATIONAL_FUNC_ARGS);
static bool UndefNEBool						(RELATIONAL_FUNC_ARGS);
static bool BoolEQNumberCached 				(RELATIONAL_FUNC_ARGS);
static bool BoolNENumberCached 				(RELATIONAL_FUNC_ARGS);
static bool BoolEQStringCached 				(RELATIONAL_FUNC_ARGS);
static bool BoolNEStringCached 				(RELATIONAL_FUNC_ARGS);
static bool BoolEQProgramFuncCached 		(RELATIONAL_FUNC_ARGS);
static bool BoolNEProgramFuncCached 		(RELATIONAL_FUNC_ARGS);
static bool BoolEQMethodFuncCached			(RELATIONAL_FUNC_ARGS);
static bool BoolNEMethodFuncCached			(RELATIONAL_FUNC_ARGS);
static bool BoolEQLibraryFuncCached 		(RELATIONAL_FUNC_ARGS);
static bool BoolNELibraryFuncCached 		(RELATIONAL_FUNC_ARGS);
static bool BoolEQBoolCached 				(RELATIONAL_FUNC_ARGS);
static bool BoolNEBoolCached 				(RELATIONAL_FUNC_ARGS);
static bool BoolEQNilCached 				(RELATIONAL_FUNC_ARGS);
static bool BoolNENilCached 				(RELATIONAL_FUNC_ARGS);
static bool BoolEQUndefCached 				(RELATIONAL_FUNC_ARGS);
static bool BoolNEUndefCached 				(RELATIONAL_FUNC_ARGS);
static bool NilEQBoolCached					(RELATIONAL_FUNC_ARGS);
static bool NilNEBoolCached					(RELATIONAL_FUNC_ARGS);
static bool UndefEQBoolCached				(RELATIONAL_FUNC_ARGS);
static bool UndefNEBoolCached				(RELATIONAL_FUNC_ARGS);

static bool ExternIdEQExternId				(RELATIONAL_FUNC_ARGS);
static bool ExternIdNEExternId				(RELATIONAL_FUNC_ARGS);
static bool ExternIdEQBool					(RELATIONAL_FUNC_ARGS);	// No caching
static bool ExternIdNEBool					(RELATIONAL_FUNC_ARGS);	// No caching
static bool ExternIdEQExternIdCached		(RELATIONAL_FUNC_ARGS);
static bool ExternIdNEExternIdCached		(RELATIONAL_FUNC_ARGS);

static bool NilEQNil						(RELATIONAL_FUNC_ARGS);
static bool NilNENil						(RELATIONAL_FUNC_ARGS);
static bool NilEQNilCached					(RELATIONAL_FUNC_ARGS);
static bool NilNENilCached					(RELATIONAL_FUNC_ARGS);

static bool TableEQBool						(RELATIONAL_FUNC_ARGS);
static bool TableEQBoolCached				(RELATIONAL_FUNC_ARGS);
static bool TableNEBool						(RELATIONAL_FUNC_ARGS);
static bool TableNEBoolCached				(RELATIONAL_FUNC_ARGS);

//***********************

static void NumberADDNumber 		(ARITHMETIC_FUNC_ARGS);
static void NumberSUBNumber 		(ARITHMETIC_FUNC_ARGS);
static void NumberMULNumber 		(ARITHMETIC_FUNC_ARGS);
static void NumberDIVNumber 		(ARITHMETIC_FUNC_ARGS);
static void NumberMODNumber 		(ARITHMETIC_FUNC_ARGS);
static void NumberADDNumberCached 	(ARITHMETIC_FUNC_ARGS);
static void NumberSUBNumberCached 	(ARITHMETIC_FUNC_ARGS);
static void NumberMULNumberCached 	(ARITHMETIC_FUNC_ARGS);
static void NumberDIVNumberCached 	(ARITHMETIC_FUNC_ARGS);
static void NumberMODNumberCached 	(ARITHMETIC_FUNC_ARGS);

static void NumberADDBool 			(ARITHMETIC_FUNC_ARGS);
static void NumberSUBBool 			(ARITHMETIC_FUNC_ARGS);
static void NumberMULBool 			(ARITHMETIC_FUNC_ARGS);
static void NumberADDBoolCached 	(ARITHMETIC_FUNC_ARGS);
static void NumberSUBBoolCached 	(ARITHMETIC_FUNC_ARGS);
static void NumberMULBoolCached 	(ARITHMETIC_FUNC_ARGS);

static void ArithmeticObj1_String	(ARITHMETIC_FUNC_ARGS);
static void TableADDTable			(ARITHMETIC_FUNC_ARGS);

static void BoolADDBool 			(ARITHMETIC_FUNC_ARGS);
static void BoolSUBBool 			(ARITHMETIC_FUNC_ARGS);
static void BoolMULBool 			(ARITHMETIC_FUNC_ARGS);
static void BoolADDNumber			(ARITHMETIC_FUNC_ARGS);
static void BoolSUBNumber			(ARITHMETIC_FUNC_ARGS);
static void BoolMULNumber			(ARITHMETIC_FUNC_ARGS);
static void BoolADDBoolCached 		(ARITHMETIC_FUNC_ARGS);
static void BoolSUBBoolCached 		(ARITHMETIC_FUNC_ARGS);
static void BoolMULBoolCached 		(ARITHMETIC_FUNC_ARGS);
static void BoolADDNumberCached		(ARITHMETIC_FUNC_ARGS);
static void BoolSUBNumberCached		(ARITHMETIC_FUNC_ARGS);
static void BoolMULNumberCached		(ARITHMETIC_FUNC_ARGS);

};

/////////////////////////////////////////////////////

protected:

class PushArguments {
	public:
	virtual bool operator()(DeltaVirtualMachine*) = 0;
};

bool operator()(PushArguments& f, DeltaValue* result);

//***********************
// A functor call method parameterized on the way
// the arguments are pushed on the VM stack.

class PushArrayArguments : public PushArguments {
	private:
	UPTR(const DeltaValue)*	args;
	util_ui16				n;

	public:
	bool operator()(DeltaVirtualMachine* vm);
	PushArrayArguments(UPTR(const DeltaValue)* _args, util_ui16 _n): args(_args), n(_n) {}
};

//***********************

class PushArrayListArguments : public PushArguments {
	private:
	UPTR(const DeltaValue)* args;

	public:
	bool operator()(DeltaVirtualMachine* vm);
	PushArrayListArguments(UPTR(const DeltaValue)* _args): args(_args){}
};

//***********************

class PushListArguments : public PushArguments {
	private:
	const std::list<DeltaValue>& args;

	public:
	bool operator()(DeltaVirtualMachine* vm);
	PushListArguments(const std::list<DeltaValue>&	_args) : args(_args){}
};

class PushListPointerArguments : public PushArguments {
	private:
	const std::list<DeltaValue*>& args;

	public:
	bool operator()(DeltaVirtualMachine* vm);
	PushListPointerArguments(const std::list<DeltaValue*>&	_args) : args(_args){}
};

//***********************

class PushSingleArgument: public PushArguments {
	private:
	const DeltaValue& arg;

	public:
	bool operator()(DeltaVirtualMachine* vm);
	PushSingleArgument(const DeltaValue& _arg): arg(_arg){}
};

/////////////////////////////////////////////////////
