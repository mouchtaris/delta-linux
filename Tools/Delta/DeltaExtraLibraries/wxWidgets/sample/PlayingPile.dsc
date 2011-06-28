using std;
using #CardCollection;

RanksEnum = CardCollection::getRanksEnum();

function playingPile(collection)
{
	playingPile_t = [
		@class		: "playing",
		method moveRules(toMove) {
			if (self.size == 0)
				return true;	// Anything can go to an empty playing pile.
			else if (toMove.cards[0].Color == self.cards[self.size - 1].Color)
				return false;	// Only "Red on Black" or "Black on Red"
			else if (RanksEnum[toMove.cards[0].Rank] + 1 != RanksEnum[self.cards[self.size - 1].Rank])
				return false;	// Only the next rank can go atop a card.
			else
				return true;
		},
		method selectRules(card) {
			local i = 0;
			local previousCard = 0;
			local currentCard = 0;
			
			for (; i < self.size; ++i) {
				if (self.cards[i] == card) {
					break;
				}
			}

			if (i < self.size) {
				previousCard = self.cards[i++];
			}
			
			for (; i < self.size; ++i) {
				currentCard = self.cards[i];
				if (previousCard.Color == currentCard.Color)
					return false;
				if (RanksEnum[previousCard.Rank] != RanksEnum[currentCard.Rank] + 1)
					return false;
				previousCard = currentCard;
			}
			
			return true;
		}
	];
	if(arguments.total() >= 1)
		inherit(playingPile_t, CardCollection::cardCollection(collection));
	else
		inherit(playingPile_t, CardCollection::cardCollection());
	return playingPile_t;
};
