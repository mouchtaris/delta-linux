using std;

function cardCollection (collection)
{
	cardcollection = [
		@cards			: [],
		@size 			: 0,
		@class			: "cardCollection",
		@collectionNo	: -1,
		
		//===== Methods =====//
		//----- Concatenate -----//	
		method concatenate (collection)
		{
			if(collection.class == #cardCollection or
			   collection.class == #playing or
			   collection.class == #waste)
			{
				for (local i = 0, j = self.size; i < collection.size; ++i, ++j) {
					self.cards[j] = collection.cards[i];
				}
			}
			else if(collection.class == #card)
				self.cards[self.size] = collection;
			else
				error("Error @cardCollection.concatenate: Argument is not a card or cardCollection.\n");
			
			self.size = tablength(self.cards);
		},
		//----- Split -----//
		method split (where)
		{
			if(where.class != #card)
				error("Error @cardCollection.split: Argument is not a card.\n");

			ret = cardCollection();
			local i = 0;
			for (; i < self.size; ++i) {
				if (self.cards[i] == where) {
					break;
				}
			}
			for (local j = 0; i < self.size; ++i, ++j) {
				ret.cards[j] = self.cards[i];
				self.cards[i] = nil;
			}
			self.size = tablength(self.cards);
			ret.size = tablength(ret.cards);

			return ret;
		}
	];

	if(arguments.total() >= 1)
	{
		if(collection.class == #cardCollection or collection.class == #card)
			cardcollection.concatenate(collection);
		else
			error("Error @cardCollection.new: Argument is not a cardCollection or a card.");
	}
	cardcollection.size = tablength(cardcollection.cards);
	return cardcollection;
}

RanksEnum = [{"ACE":1}, {"DEUCE":2}, {"THREE":3}, {"FOUR":4}, {"FIVE":5}, {"SIX":6}, {"SEVEN":7}, {"EIGHT":8}, {"NINE":9}, {"TEN":10}, {"JACK":11}, {"QUEEN":12}, {"KING":13}];

function getRanksEnum() { return RanksEnum; }
