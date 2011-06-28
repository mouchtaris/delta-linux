using std;
using #CardCollection;

RanksEnum = CardCollection::getRanksEnum();

function waste(collection)
{
	waste_t = [
		@class		: "waste",
		method moveRules(toMove) {
			if (toMove.size == 1 and self.size == 0)
				return true;
			return false;			// Only 1 card at a time can be there.
		},
		method selectRules(card) {
			if (self.size == 1)
				return true;						// You can take the card out of the waste.
			return false;
		}
	];
	if(arguments.total() >= 1)
		inherit(waste_t, CardCollection::cardCollection(collection));
	else
		inherit(waste_t, CardCollection::cardCollection());
	return waste_t;
};
