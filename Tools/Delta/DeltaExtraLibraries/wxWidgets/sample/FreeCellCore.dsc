using std;
using #PlayingPile;
using #WastePile;
using #FoundationPile;
using #Deck;

history = std::list_new();
future = std::list_new();

function FreeCell()
{
	return	[
				@playing	: [],
				@foundation	: [],
				@waste		: [],
				method new()
				{
					for(local i = 0; i < 8; i++)
					{
						self.playing[i] = PlayingPile::playingPile();
						self.playing[i].collectionNo = i;
					}

					for(local i = 0; i < 4; i++)
					{
						self.foundation[i] = FoundationPile::foundation();
						self.foundation[i].collectionNo = i;
					}
					
					for(local i = 0; i < 4; i++)
					{
						self.waste[i] = WastePile::waste();
						self.waste[i].collectionNo = i;
					}
					
					self.deal();
					history.clear();
					future.clear();
					
					return self;
				},
				
				method deal()
				{
					local deck = Deck::deck_new();
					local deck_card = 0;
					while(deck.size > 0)
					{
						for (local i = 0; i < std::tablength(self.playing); i++)
						{
							if (deck.size == 0)
								break;
							local card = deck.cards[deck_card];
							deck.cards[deck_card++] = nil;
							deck.size--;
							self.playing[i].concatenate(card);
						}
					}
				},
				method move(movingPile, source, destination)
				{
					c = movingPile.split(movingPile.cards[0]);
					if (destination.moveRules(c))
					{
						local movingPileCardsNo = c.size;
						source.concatenate(c);
						if (self.availableMoves(movingPileCardsNo, destination)) {
							c = source.split(source.cards[source.size - movingPileCardsNo]);
							destination.concatenate(c);
							future.clear();
							history.push_front([{"numberOfCards":c.size}, {"source":source}, {"destination":destination}]);
						} else {
							std::print("Cannot move so many cards.\n");
						}
					}
					else
					{
						source.concatenate(c);
						std::print("Invalid move.\n");
					}
				},
				method moveToFoundation()
				{
					topCards = [];
					for (local i = 0; i < 4; ++i) {
						if (self.foundation[i].size != 0) {
							topCards[self.foundation[i].cards[0].Suit] = [];
							topCards[self.foundation[i].cards[0].Suit].card = self.foundation[i].cards[self.foundation[i].size - 1];
							topCards[self.foundation[i].cards[0].Suit].foundation = i;
						}
					}
					for (local i = 0; i < 4; ++i) {
						if (self.waste[i].size != 0) {
							local cardSuit = self.waste[i].cards[0].Suit;
							local cardRank = self.waste[i].cards[0].Rank;
							if (topCards[cardSuit] != nil) {
								if (self.foundation[topCards[cardSuit].foundation].moveRules(self.waste[i])) {
									self.move(self.waste[i], self.waste[i], self.foundation[topCards[cardSuit].foundation]);
									self.moveToFoundation();
									return;
								}
							} else if (cardRank == "ACE") {
								for (local j = 0; j < 4; ++j) {
									if (self.foundation[j].size == 0) {
										self.move(self.waste[i], self.waste[i], self.foundation[j]);
										self.moveToFoundation();
										return;
									}
								}
							}
						}
					}
					for (local i = 0; i < 8; ++i) {
						if (self.playing[i].size != 0) {
							local cardSuit = self.playing[i].cards[self.playing[i].size - 1].Suit;
							local cardRank = self.playing[i].cards[self.playing[i].size - 1].Rank;
							local card = self.playing[i].split(self.playing[i].cards[self.playing[i].size - 1]);
							if (topCards[cardSuit] != nil) {
								if (self.foundation[topCards[cardSuit].foundation].moveRules(card)) {
									self.move(card, self.playing[i], self.foundation[topCards[cardSuit].foundation]);
									self.moveToFoundation();
									return;
								} else {
									self.playing[i].concatenate(card);
								}
							} else if (cardRank == "ACE") {
								for (local j = 0; j < 4; ++j) {
									if (self.foundation[j].size == 0) {
										self.move(card, self.playing[i], self.foundation[j]);
										self.moveToFoundation();
										return;
									}
								}
							} else {
								self.playing[i].concatenate(card);
							}
						}
					}
				},
				method availableMoves(noOfCards, destination)
				{
					local emptyWaste	= 0;
					local emptyPlaying	= 0;

					foreach(i, self.playing)
					{
						if(i.size == 0)
							emptyPlaying++;
					}
					
					foreach(i, self.waste)
					{
						if(i.size == 0)
							emptyWaste++;
					}

					if (destination.class == "playing") {
						if (destination.size == 0 and noOfCards <= (emptyWaste + 1)*emptyPlaying)
							return true;
						else if (destination.size != 0 and noOfCards <= (emptyWaste + 1)*(emptyPlaying+1))
							return true;
					}
					else if (noOfCards <= 1)
						return true;
					return false;
				}
			];
}

function sudoMove (card, source, destination)
{
	destination.concatenate(source.split(card));
}

function undo()
{
	if(history.total() > 0)
	{
		local m = history.pop_front();
		local card = m.destination.cards[m.destination.size - m.numberOfCards];
		sudoMove(card, m.destination, m.source);
		future.push_front([{"numberOfCards":m.numberOfCards},{"source":m.destination},{"destination":m.source}]);
		return m;
	}
	return nil;
}

function redo()
{
	if(future.total() > 0)
	{
		local m = future.pop_front();
		local card = m.destination.cards[m.destination.size - m.numberOfCards];
		sudoMove(card, m.destination, m.source);
		history.push_front([{"numberOfCards":m.numberOfCards},{"source":m.destination},{"destination":m.source}]);
		return m;
	}
	return nil;
}

function getUndoPile() { return history; }
function getRedoPile() { return future; }
function addUndoAction(numberOfCards, source, destination)
	{ history.push_back([{"numberOfCards":numberOfCards},{"source":source},{"destination":destination}]); }
function addRedoAction(numberOfCards, source, destination)
	{ future.push_back([{"numberOfCards":numberOfCards},{"source":source},{"destination":destination}]); }
