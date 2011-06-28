using std;
using #CardCollection;

RanksEnum = CardCollection::getRanksEnum();

function foundation(collection)
{
	foundation_t = [
		@class		: "foundation",
		method moveRules(toMove) {
			if (toMove.size != 1)
				return false;	// Only one card at a time can be moved to a foundation.
			else if (self.size == 0) {
				if (toMove.cards[0].Rank == "ACE") {
					return true;	// Only an ACE can go to an empty foundation.
				} else {
					return false;
				}
			} else if (toMove.cards[0].Suit != self.cards[self.size - 1].Suit)
				return false;	// Only cards of the same suit can go to the same foundation.
			else if (RanksEnum[toMove.cards[0].Rank] != RanksEnum[self.cards[self.size - 1].Rank] + 1)
				return false;	// Only the next rank can go atop a card.
			else
				return true;
		},
		method selectRules(card) {
			return false;						// You cannot take a card out of a foundation.
		}
	];
	if(arguments.total() >= 1)
		inherit(foundation_t, CardCollection::cardCollection(collection));
	else
		inherit(foundation_t, CardCollection::cardCollection());
	return foundation_t;
};
