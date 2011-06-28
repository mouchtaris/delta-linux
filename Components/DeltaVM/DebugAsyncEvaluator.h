// DebugAsyncEvaluator.h
// DeltaVM component.
// Splitting the posting of a request and the receipt of the response
// in two distinct threads at the debugger frontend.
// After major refactoring of code from Yannis Lilis.
// A. Savidis, Decemeber 2010.
//

#ifndef	DEBUGASYNCEVALUATOR_H
#define	DEBUGASYNCEVALUATOR_H

#include "BinaryBuffer.h"
#include "DeltaVMFuncReturnTypes.h"
#include "DebugClient.h"
#include <boost/thread/mutex.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>

#pragma warning(disable: 4251)

/////////////////////////////////////////////////////////////////////

class DebuggerAsyncEvaluation {	// Super class.

	public:
	typedef boost::function<void (void)> RequestFunctor;

	protected:
	std::string		error;
	RequestFunctor	requestor;
	bool			isDone;

	public:
	bool			IsDone (void) const
						{ return isDone; }
	void			SetDone (void)
						{ DASSERT(!isDone); isDone = true; }
	bool			HasError (void) const 
						{ return !error.empty(); }
	void			SetError (const std::string& _error)
						{ error = _error; }
	const std::string&
					GetError (void) const
						{ return error; }
	void			SendRequest (void) const 
						{ requestor(); }

	virtual void	ExtractResponse (DebugServerToClientResponse response)  {	// Template (pattern) method.
						DispatchResponse(response);
						DeltaDebugClient::ResponseProcessed();
						PrepareResult();
						SetDone();
					}
	virtual void	DispatchResponse (DebugServerToClientResponse response) = 0;
	virtual void	PrepareResult (void) = 0;

	DebuggerAsyncEvaluation (const RequestFunctor& f) : requestor(f), isDone(false) {}
	virtual ~DebuggerAsyncEvaluation(){}
};

//********************************

class EvalExprAsyncEvaluation : public DebuggerAsyncEvaluation {
	public:
	typedef StringBoolPair ResultType;

	private:
	ResultType	result;
	std::string value;

	virtual void		PrepareResult(void) {
							if (result.second = !HasError())
								result.first = util::std2str(value); 
							else
								result.first = util::std2str(GetError()); 
						}

	public:
	const ResultType&	GetResult (void) const
							{ return result; }
	virtual void		DispatchResponse (DebugServerToClientResponse response);

	EvalExprAsyncEvaluation (const RequestFunctor& f) : DebuggerAsyncEvaluation(f), result(String(), false) {}
	~EvalExprAsyncEvaluation(){}
};

//********************************

class EvalGraphAsyncEvaluation : public DebuggerAsyncEvaluation {
	public:
	typedef ObjectGraphData ResultType;

	private:
	typedef DeltaDebugClient::ObjectGraph Graph;
	Graph		graph;
	std::string	simple;
	ResultType	result;
	bool				IsGraph (void) const 
							{ return !graph.empty();	}
	virtual void		PrepareResult(void);

	public:
	const ResultType&	GetResult (void) const
							{ return result; }
	virtual void		DispatchResponse (DebugServerToClientResponse response);

	EvalGraphAsyncEvaluation (const RequestFunctor& f) : DebuggerAsyncEvaluation(f){}
	~EvalGraphAsyncEvaluation(){}
};

//********************************

class EvalBinaryDataAsyncEvaluation : public DebuggerAsyncEvaluation {

	public:
	typedef InputBinaryBuffer ResultType;

	private:
	ResultType result;

	virtual void		PrepareResult (void) {}

	public:
	const ResultType&	GetResult (void) const 
							{ return result; }
	virtual void		DispatchResponse (DebugServerToClientResponse response); 

	EvalBinaryDataAsyncEvaluation (const RequestFunctor& f) : DebuggerAsyncEvaluation(f){}
	~EvalBinaryDataAsyncEvaluation(){}
};

//********************************

class EvalMultipleExprAsyncEvaluation : public DebuggerAsyncEvaluation {
	public:
	typedef StringList ResultType;

	private:
	typedef std::list<std::pair<std::string, bool>>	EvalList;
	EvalList	values;
	ResultType	result;

	virtual void		PrepareResult(void);

	public:
	const ResultType&	GetResult (void) const 
							{ return result; }

	virtual void		DispatchResponse (DebugServerToClientResponse response);

	EvalMultipleExprAsyncEvaluation (const RequestFunctor& f) : DebuggerAsyncEvaluation(f){}
	~EvalMultipleExprAsyncEvaluation(){}
};

//********************************

class DebugAsyncEvaluator {
	
	private:
	typedef std::list<DebuggerAsyncEvaluation*> EvalQueue;
	static EvalQueue		queue;
	static boost::mutex		evalMutex;

	static DebuggerAsyncEvaluation*	Dequeue (void);
	static void						Enqueue (DebuggerAsyncEvaluation* eval);

	public:
	static void						Receive (DebugServerToClientResponse response);

	template <class Teval>
	static const typename Teval::ResultType		
									Eval (Teval& eval) {
										Enqueue(&eval);
										try
											{ eval.SendRequest(); }
										catch (DeltaDebugClient::BrokenConnection) 
											{ return eval.GetResult(); }

										while(!eval.IsDone())	// Busy waiting. Receive() called by another thread.
											Sleep(100);
										return eval.GetResult();
									}

	static void						CancelAll (void);

};

/////////////////////////////////////////////////////////////////////

#endif	// Do not add stuff beyond this point.
