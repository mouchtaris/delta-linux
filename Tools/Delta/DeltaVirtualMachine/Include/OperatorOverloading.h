// OperatorOverloading.h
// Dynamic operator overloading in Delta. Singleton class.
// ScriptFighter Project.
// A. Savidis, July 2005.
//

#ifndef	OPERATOROVERLOADING_H
#define	OPERATOROVERLOADING_H

#include "DeltaVirtualMachine.h"
#include "DeltaExceptionHandling.h"
#include "DeltaTable.h"
#include "DeltaValue.h"
#include "usingleton.h"

#include <string>
#include <map>

////////////////////////////////////////////////////////////////

class OperatorOverloading {
	private:
	USINGLETON_APISTYLE_DECLARE_PRIVATESTDMETHODS
		
	struct GlobalOperators {
		DeltaValue		arithmetic[TOTAL_DELTAOBJECT_ARITHOP_TYPES];
		DeltaValue		relational[TOTAL_DELTAOBJECT_RELOP_TYPES];
		DeltaValue		assignment;
		DeltaValue		typeCasting;
	};

	static GlobalOperators*	global;
	static GlobalOperators&	
					Global (void) 
						{ DASSERT(IsSingletonCreated()); return *DPTR(global); }

	static  const std::string	
					GetOperatorString (
						const char* op,
						bool		callerIsSecondArg
					);

	static bool		PassArguments (
						DeltaValue*				first,
						DeltaValue*				second,
						DeltaVirtualMachine*	vm					
					);

	static bool		CallOperatorFunction (
						DeltaValue&				func,
						DeltaVirtualMachine*	vm
					);

	//////////////////////////////////////////////////////////////

	public:
	USINGLETON_APISTYLE_DECLARE_PUBLICSTDMETHODS
	static bool		GetOperatorMember (
						DeltaValue*				caller,
						const char*				opStr,
						DeltaValue*				func,
						DeltaVirtualMachine*	vm = (DeltaVirtualMachine*) 0,	// Needed only if 'issueError'.
						bool					issueError = false	// Post error via 'vm'.
					);

	//////////////////////////////////////////////////////////////

	enum ArithAssignResult {
		ArithAssign_NotOverloaded				= 0,
		ArithAssign_OverloadedAndSucceeded		= 1,
		ArithAssign_OverloadedButRequestedRetry	= 2,
		ArithAssign_ExecutionError				= 3,
		ArithAssign_Exception					= 4
	};

	static ArithAssignResult		HandleArithmetic (
										const char*				op,
										bool					callerIsSecondArg,	// If <arg, op, caller>
										DeltaValue*				caller,
										DeltaValue*				arg,
										DeltaValue*				result,
										DeltaVirtualMachine*	vm 
									);

	static bool						HasGlobalArithmetic (DeltaArithOpType op);

	static ArithAssignResult		HandleGlobalArithmetic (
										DeltaArithOpType		op,
										DeltaValue*				left,
										DeltaValue*				right,
										DeltaValue*				result,
										DeltaVirtualMachine*	vm 
									);

	static void						SetGlobalArithmetic (
										DeltaArithOpType		op,
										const DeltaValue*		func
									);
	static const DeltaValue&		GetGlobalArithmetic (DeltaArithOpType op);

	static DeltaArithOpType			GetArithmeticOp (const char* opStr);

	//////////////////////////////////////////////////////////////

	public:
	static ArithAssignResult		HandleAssign (
										DeltaValue*			lv,
										const DeltaValue*	rv
									);

	static void						SetGlobalAssign (const DeltaValue* func);
	static const DeltaValue&		GetGlobalAssign (void);

	private:
	static bool						HasGlobalAssign (void);

	static ArithAssignResult		HandleLocalAssign (
										DeltaValue*			lv,
										const DeltaValue*	rv
									);

	static ArithAssignResult		HandleGlobalAssign (
										DeltaValue*			lv,
										const DeltaValue*	rv
									);

	//////////////////////////////////////////////////////////////
	
	public:
	enum TypeCastingResult {
		TypeCasting_NotOverloaded				= 0,
		TypeCasting_OverloadedAndSucceeded		= 1,
		TypeCasting_OverloadedButRequestedRetry	= 2,
		TypeCasting_ExecutionError				= 3,
		TypeCasting_Exception					= 4
	};
	
	static TypeCastingResult		HandleTypeCasting (
										DeltaValue*			lvalue,
										const DeltaValue*	rvalue
									);

	static void						SetGlobalTypeCasting (const DeltaValue* func);
	static const DeltaValue&		GetGlobalTypeCasting (void);

	private:
	static TypeCastingResult		HandleLocalTypeCasting (
										DeltaValue*			lvalue,
										const DeltaValue*	rvalue
									);

	static bool						HasGlobalTypeCasting (void);

	static TypeCastingResult		HandleGlobalTypeCasting (
										DeltaValue*			lvalue,
										const DeltaValue*	rvalue
									);

	//////////////////////////////////////////////////////////////

	public:
	enum RelationalResult {
		Relational_True							= 0,
		Relational_False						= 1,
		Relational_NotOverloaded				= 2,
		Relational_OverloadedButRequestedRetry	= 3,
		Relational_ExecutionError				= 4,
		Relational_Exception					= 5
	};

	static RelationalResult			HandleRelational (
										const char*				op,
										bool					callerIsSecondArg,	// If <arg, op, caller>
										DeltaValue*				caller,	
										DeltaValue*				arg,
										DeltaVirtualMachine*	vm = (DeltaVirtualMachine*) 0
									);

	static bool						HasGlobalRelational (DeltaRelOpType op);

	static RelationalResult			HandleGlobalRelational (
										DeltaRelOpType			op,
										DeltaValue*				left,	
										DeltaValue*				right,
										DeltaVirtualMachine*	vm = (DeltaVirtualMachine*) 0
									);
	
	static void						SetGlobalRelational (
										DeltaRelOpType		op,
										const DeltaValue*	func
									);
	static const DeltaValue&		GetGlobalRelational(DeltaRelOpType op);

	static DeltaRelOpType			GetRelationalOp (const char* opStr);
};

////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.

