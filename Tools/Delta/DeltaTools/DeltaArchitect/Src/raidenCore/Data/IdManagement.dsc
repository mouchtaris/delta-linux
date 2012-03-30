


function IdPatternProj {
	return [
		@PatternID  : 0,
		@PatternFS  : std::list_new(),
		method getID () {
			local id = ( @PatternFS.total() > 0 ? @PatternFS.pop_front() : @PatternID++ );
			return id;
		},
		method removeID (id)
			{	@PatternFS.push_back(id);	}
	];
}

/** 
 *  Description: Id system to free and give the id that has priority
 *
 **/
function IdManagement {
	return [
		@RequirementID	: 0,
		@OperationID	: 0,
		@RoleID			: 0,
		@InputID		: 0,
		@OutputID		: 0,
		@AddressID		: 0,
		@OfferID		: 0,
		@UseID			: 0,
		@PartOFID		: 0,
		// keep FS(FreeSlots) for tables with below stacks
		@RequirementFS	: std::list_new(),
		@OperationFS	: std::list_new(),
		@RoleFS			: std::list_new(),
		@InputFS		: std::list_new(),
		@OutputFS		: std::list_new(),
		@AddressFS		: std::list_new(),
		@OfferFS		: std::list_new(),
		@UseFS			: std::list_new(),
		@PartOFFS		: std::list_new(),
		method getID (type) {
			nameOfStack = type+"FS";
			local id = ( self[nameOfStack].total() > 0 ? self[nameOfStack].pop_front() : self[(type+"ID")]++ );
			return id;
		},
		method getSpID (type, id) {
			tmpfs = self[(type+"FS")];
			foreach (tid, tmpfs)
				if (tid==id) {
					self[(type+"FS")].remove(id);
					return id;
				}
			// invalid case
			assert(false);
			return;
		},
		method removeID (type, id)
			{	self[(type+"FS")].push_back(id);	}
	];
}


function ConnectionsId () {
	return [
		
		@ID_SAVEALL	: 15,
		@currId		: 30,	//1-29 are the id that program uses from load
		@freeIds	: std::list_new(),
		method getID () {
			return (@freeIds.total()>0 ? @freeIds.pop_front() : @currId++);
		},
		method removeID (id) {
			@freeIds.push_back(id);
		},
		method removeIDs (ids) {
			foreach (id, ids)
				@freeIds.push_back(id);
		}
	];
}


function Core () {
	if (std::isundefined(static dataid)) {
		dataid = [
			@MainPageID : 14
		];
	}
	return dataid;
}
