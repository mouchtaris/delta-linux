&function OrderedAlternatives(l, ex) {
	local found = <<$found>>;
	local counter = <<$counter>>;
	local code = nil;
	foreach(local alternative, l) {
		local tryTrap = <<
			if (not ~found)
				try {
					~alternative;
					~found = true;
				}
				trap $ex {
					if ($ex != ~ex) throw $ex;		//other exceptions are still thrown
					if (++~counter == ~(l.total()))	//run out of alternatives
						throw $ex;
				}
		>>;
		code = <<~code; ~tryTrap;>>;
	}
	return <<{
		local ~found = false;
		local $counter = 0;
		~code;
	}>>;
}

&function UnorderedAlternatives(t, ex) {
	local selected = <<$selected>>;
	local code = nil;
	local entryIndices = nil;
	local total_alternatives = std::tablength(t);
	for(local i = 0; i < total_alternatives; ++i) {
		local tryTrap = <<
			if (~selected == ~i)
				try {
					~(t[i]);
					break;
				}
				trap $ex { if ($ex != ~ex) throw $ex; }
		>>;
		code = <<~code; ~tryTrap;>>;
		entryIndices = <<~entryIndices, ~i>>;
	}
	
	return <<{
		local alternatives = std::list_new(~entryIndices);
		for (local i = 0; i < ~total_alternatives; ++i) {
			local index = std::random(alternatives.total());
			local iter = alternatives.iterator();
			for (local j = 0; j < index; ++j)
				iter.fwd();
			local ~selected = iter.getval();
			alternatives.erase(iter);
			~code;
		}
		if (i == ~total_alternatives) throw ~ex;
	}>>;
}

function Hotel(hotel, total_rooms) {
	return [
		@vacant_rooms : total_rooms,
		@total_rooms : total_rooms,
		@name : hotel,
		method Book () {
			if(@vacant_rooms == 0)
				throw "HotelFullException";
			--@vacant_rooms;
			std::print("Booking at ", @name, "\n");
		}
	];
}

h1 = Hotel("President", 5);
h2 = Hotel("Royal", 4);
h3 = Hotel("Waikiki", 3);

std::print("Booking a hotel using ordered alternatives [President, Royal, Waikiki]:\n");
for(i = 0; i < 12; ++i)
	!(OrderedAlternatives(
		std::list_new(<<h1.Book()>>, <<h2.Book()>>, <<h3.Book()>>),
		"HotelFullException"
	));
	
h1 = Hotel("President", 5);
h2 = Hotel("Royal", 4);
h3 = Hotel("Waikiki", 3);

std::randomise();
std::print("\nBooking a hotel using unordered alternatives [President, Royal, Waikiki]:\n");
for(i = 0; i < 13; ++i)
	!(UnorderedAlternatives(
		[<<h1.Book()>>, <<h2.Book()>>, <<h3.Book()>>],
		"HotelFullException"
	));