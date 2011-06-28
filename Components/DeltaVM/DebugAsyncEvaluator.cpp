// DebugAsyncEvaluator.cpp
// DeltaVM component.
// Splitting the posting of a request and the receipt of the response
// in two distinct threads at the debugger frontend.
// After major refactoring of code from Yannis Lilis.
// A. Savidis, Decemeber 2010.
//

#include "DebugAsyncEvaluator.h"

DebugAsyncEvaluator::EvalQueue	DebugAsyncEvaluator::queue;
boost::mutex					DebugAsyncEvaluator::evalMutex;

/////////////////////////////////////////////////////////////////////

void EvalExprAsyncEvaluation::DispatchResponse (DebugServerToClientResponse response) {
	switch(response) {
		case Debug_InfoValue		: DeltaDebugClient::GetInfoValue(&value);	break;
		case Debug_InfoExprTypeData	: DeltaDebugClient::GetInfoExprTypeData(&value); break;
		case Debug_InfoErrorValue	: DeltaDebugClient::GetInfoErrorValue(&error); break;
		default : DASSERT(false);	
	}
}

/////////////////////////////////////////////////////////////////////

void EvalGraphAsyncEvaluation::PrepareResult(void) {
	if (!HasError()) 
		if (IsGraph()) {
			DASSERT(simple.empty());
			BOOST_FOREACH(const Graph::value_type& pair, graph) {
				ObjectNodeList l;
				BOOST_FOREACH(const DeltaDebugClient::ObjectNode::value_type& val, pair.second)
					l.push_back(ObjectNodeTuple(val.first, val.second, val.third, val.fourth, val.fifth));
				result.get<0>()[pair.first] = l;
			}
		}
		else
			result.get<1>() = simple;
	else
		result.get<1>() = GetError();
	result.get<2>() = !HasError();
}

void EvalGraphAsyncEvaluation::DispatchResponse (DebugServerToClientResponse response) {
	switch(response) {
		case Debug_InfoObjectGraph	: DeltaDebugClient::GetInfoObjectGraph(graph);	break;
		case Debug_InfoValue		: DeltaDebugClient::GetInfoValue(&simple);		break;
		case Debug_InfoErrorValue	: DeltaDebugClient::GetInfoErrorValue(&error);	break;
		default : DASSERT(false);
	}
}

/////////////////////////////////////////////////////////////////////

void EvalBinaryDataAsyncEvaluation::DispatchResponse (DebugServerToClientResponse response) {
	switch(response) {
		case Debug_InfoBinaryData : { 
			void*		data = (void*) 0;
			util_ui32	size = 0;
			DeltaDebugClient::GetInfoBinaryData(&data, &size); 
			new (&result) InputBinaryBuffer(data, size); 
			DeltaDebugClient::ClearBinaryData(&data, &size);
			break; 
		}
		case Debug_InfoErrorValue : DeltaDebugClient::GetInfoErrorValue(&error); break;
		default : DASSERT(false);
	}
}

/////////////////////////////////////////////////////////////////////

void EvalMultipleExprAsyncEvaluation::PrepareResult(void) {
	BOOST_FOREACH(const EvalList::value_type& pair, values)
		result.push_back(util::std2str(pair.first));
}

void EvalMultipleExprAsyncEvaluation::DispatchResponse (DebugServerToClientResponse response) {
	switch(response) {
		case Debug_InfoValueMany		: DeltaDebugClient::GetInfoValueMany(values); break;
		case Debug_InfoExprTypeDataMany	: DeltaDebugClient::GetInfoExprTypeDataMany(values); break;
		case Debug_InfoErrorValue		: DeltaDebugClient::GetInfoErrorValue(&error); break;
		default : DASSERT(false);
	}
}

/////////////////////////////////////////////////////////////////////

DebuggerAsyncEvaluation* DebugAsyncEvaluator::Dequeue (void) {
	boost::mutex::scoped_lock lock(evalMutex);
	DASSERT(!queue.empty());
	DebuggerAsyncEvaluation* eval = queue.front();
	queue.pop_front();
	return eval;
}

//*****************************

void DebugAsyncEvaluator::Enqueue (DebuggerAsyncEvaluation* eval) {
	boost::mutex::scoped_lock lock(evalMutex);
	queue.push_back(eval);
}

//*****************************

void DebugAsyncEvaluator::Receive (DebugServerToClientResponse response) {
	DebuggerAsyncEvaluation* eval = Dequeue();
	eval->ExtractResponse(response);
}

//*****************************

void DebugAsyncEvaluator::CancelAll (void) {
	while (!queue.empty()) {
		DebuggerAsyncEvaluation* eval = Dequeue();
		eval->SetError("Connection broken!");
		eval->SetDone();
	}
}

/////////////////////////////////////////////////////////////////////
