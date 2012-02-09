////////////////////////////////////////////////////////////////////////////////////////////////////////
// A Holder for UIAPI objects
////////////////////////////////////////////////////////////////////////////////////////////////////////

using std;

///////////////////////////////////////////////////////////////

//
// UIAPIHolder
// (UIAPIHolder singleton Object) ()
//

function GetInstance {
	static instance;
	
	if (isundefined(instance)) {
		instance = [
			@apis : list_new(),
			method AddAPI (api)
				{ @apis.push_back(api); },
			method iterator {
				return [
					@it : listiter_new(),
					method setbegin (x)		{ @it.setbegin(x.apis); },
					method setend (x)		{ @it.setend(x.apis); },
					method checkend (x)  	{ return @it.checkend(x.apis); },
					method checkbegin(x)	{ return @it.checkbegin(x.apis); },
					method getval			{ return @it.getval(); },
					method fwd 				{ @it.fwd(); },
					method bwd				{ @it.bwd(); },
					method assign (y)		{ @it.assign(y.it); },
					method equal (y)		{ return @it.equal(y.it); }
				];
			}
		];
	}
	
	return instance;
}

///////////////////////////////////////////////////////////////