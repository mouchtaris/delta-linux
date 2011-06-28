Suits = 	["CLUBS", "DIAMONDS", "HEARTS", "SPADES"];
Ranks = 	["ACE", "DEUCE", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT", "NINE", "TEN", "JACK", "QUEEN", "KING"];

function contains (table, var)
{
	foreach(x, table)
	{ 
		if (x == var)
			return true;
	}
	return false;
}

function card (rank, suit)
{
	return [
			//----- The color of the card -----//
			@Color : 0,
			//----- The suit of the card -----//
			@Suit{
				@set (method(suit) {
					@Suit = suit;
					if ((suit == "DIAMONDS") or (suit == "HEARTS"))
						self.Color = "Red";
					else
						self.Color = "Black";
				})
				@get (method() { return @Suit; })
			},
			@Suit : suit,
			//----- The rank of the card -----//
			@Rank : rank,
			//----- The side of the card -----//
			@hidden : false,
			//----- Class -----//
			@class	: "card",
			
			//===== Methods =====//
			//----- flip the side of the card -----//
			method flip ()
			{
				self.hidden = not self.hidden;
			}
		];
}

function getRanks() { return Ranks; }
function getSuits() { return Suits; }
