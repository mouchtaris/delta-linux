using std;
using #Card;

Suits = Card::getSuits();
Ranks = Card::getRanks();

cardcollectionvm = vmget("CardCollection");

if(not cardcollectionvm) {
	cardcollectionvm = vmload("CardCollection.dbc", "CardCollection");
	if (cardcollectionvm)
		vmrun(cardcollectionvm);
	else
		assert 0;
}

cardCollectionFunctions = cardcollectionvm.funcs();

function deck_new ()
{
	inherit(deck_t = [@class:"deck"], cardCollectionFunctions.cardCollection());
	local deckTable = [];
	randomise();
	local i = 0;
	foreach (rank, Ranks)
	{
		foreach (suit, Suits)
		{
			local c = Card::card(rank, suit);
			local tmp  = random(52);
			
			while(deckTable[tmp])
				tmp  = random(52);
			
			deckTable[tmp] = c;
		}
	}
	
	foreach (card, deckTable)
		deck_t.concatenate(card);
	
	return deck_t;
}
