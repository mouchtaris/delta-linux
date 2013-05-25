using wx;
using #Card;
using #FreeCellCore;

dll2 = std::dllimportdeltalib(wx::DLL);
dll = std::dllimportdeltalib(xml::DLL);

Suits = Card::getSuits();
Ranks = Card::getRanks();

function bitmaps_load()
{
	initallimagehandlers();
	local bitmaps = [];
	foreach(x, Suits) {
		bitmaps[x] = [];
		foreach(y, Ranks) {
			bitmaps[x][y] = bitmap_construct(image_construct("cards\\" + y + "_" + x + ".png"));
		}
	}
	bitmaps["FOUNDATION"] = bitmap_construct(image_construct("cards\\FOUNDATION.png"));
	bitmaps["EMPTY_PILE"] = bitmap_construct(image_construct("cards\\EMPTY_PILE.png"));
	bitmaps["EMPTY_PLAYINGPILE"] = bitmap_construct(image_construct("cards\\EMPTY_PLAYINGPILE.png"));
	bitmaps["HIDDEN"] = bitmap_construct(image_construct("cards\\HIDDEN.png"));
	bitmaps.width = bitmaps["HIDDEN"].getwidth();
	bitmaps.height = bitmaps["HIDDEN"].getheight();
	bitmaps.size = size_construct(bitmaps.width, bitmaps.height);
	return bitmaps;
}
bitmaps = bitmaps_load();

function identifyCollection(collection, game)
{
	local className = collection[0]["$Attributes"].Class;
	local collectionNo = std::strtonum(collection[0]["$Attributes"].CollectionNo);
	if (className == "playing") {
		return game.playingPileGUI[collectionNo];
	} else if (className == "foundation") {
		return game.foundationGUI[collectionNo];
	} else if (className == "waste") {
		return game.freeCellGUI[collectionNo];
	} else {
		assert(false);
	}
	return nil;
}

//Check nothing got corrupted or changed in the input file.
function getCheckSum(game)
{
	local RanksEnum = [{"ACE":1}, {"DEUCE":2}, {"THREE":3}, {"FOUR":4}, {"FIVE":5},
		{"SIX":6}, {"SEVEN":7}, {"EIGHT":8}, {"NINE":9}, {"TEN":10}, {"JACK":11},
		{"QUEEN":12}, {"KING":13}];
	local SuitsEnum = [{"CLUBS":1}, {"DIAMONDS":2}, {"HEARTS":3}, {"SPADES":4}];
	local checkSum = 0;
	local multiplier = 1;
	checkSum += game.gamesWon * multiplier++;
	checkSum += game.gamesLost * multiplier++;
	checkSum += game.gamesStreakWon * multiplier++;
	checkSum += game.gamesStreakLost * multiplier++;
	for (local i = 0; i < 8; ++i) {
		local size = game.gameGUI.playingPileGUI[i].getSize();
		checkSum += size * multiplier++;
		for (local j = 0; j < size; ++j) {
			checkSum += SuitsEnum[game.gameGUI.playingPileGUI[i].getCardCollectionCard(j).Suit] * multiplier++;
			checkSum += RanksEnum[game.gameGUI.playingPileGUI[i].getCardCollectionCard(j).Rank] * multiplier++;
		}
	}
	for (local i = 0; i < 4; ++i) {
		local size = game.gameGUI.freeCellGUI[i].getSize();
		checkSum += size * multiplier++;
		for (local j = 0; j < size; ++j) {
			checkSum += SuitsEnum[game.gameGUI.freeCellGUI[i].getCardCollectionCard(j).Suit] * multiplier++;
			checkSum += RanksEnum[game.gameGUI.freeCellGUI[i].getCardCollectionCard(j).Rank] * multiplier++;
		}
	}
	for (local i = 0; i < 4; ++i) {
		local size = game.gameGUI.foundationGUI[i].getSize();
		checkSum += size * multiplier++;
		for (local j = 0; j < size; ++j) {
			checkSum += SuitsEnum[game.gameGUI.foundationGUI[i].getCardCollectionCard(j).Suit] * multiplier++;
			checkSum += RanksEnum[game.gameGUI.foundationGUI[i].getCardCollectionCard(j).Rank] * multiplier++;
		}
	}
	local ClassEnum = [{"playing":1}, {"foundation":2}, {"waste":3}];
	local undoPile = FreeCellCore::getUndoPile();
	checkSum += undoPile.total() * multiplier++;
	foreach (local i, undoPile) {
		checkSum += i.numberOfCards * multiplier++;
		checkSum += ClassEnum[i.source.class] * multiplier++;
		checkSum += i.source.collectionNo * multiplier++;
		checkSum += ClassEnum[i.destination.class] * multiplier++;
		checkSum += i.destination.collectionNo * multiplier++;
	}
	local redoPile = FreeCellCore::getRedoPile();
	checkSum += redoPile.total() * multiplier++;
	foreach (local i, redoPile) {
		checkSum += i.numberOfCards * multiplier++;
		checkSum += ClassEnum[i.source.class] * multiplier++;
		checkSum += i.source.collectionNo * multiplier++;
		checkSum += ClassEnum[i.destination.class] * multiplier++;
		checkSum += i.destination.collectionNo * multiplier++;
	}
	std::print("CheckSum value is:", checkSum, ", and multiplier value is:", multiplier, "\n");
	return checkSum;
}

function loadgame(frame)
{
	xmlfile = xml::load("xmlfile.xml");
	if (xmlfile == nil) {
		frame.gamesWon = 0;
		frame.gamesLost = 0;
		frame.gamesStreakWon = 0;
		frame.gamesStreakLost = 0;
		return nil;
	}
	if (xmlfile.Statistics != nil and xmlfile.Statistics[0] != nil) {
		if (xmlfile.Statistics[0].Total != nil and xmlfile.Statistics[0].Total[0] != nil) {
			if (xmlfile.Statistics[0].Total[0]["$Attributes"].Won != nil)
				frame.gamesWon = std::strtonum(xmlfile.Statistics[0].Total[0]["$Attributes"].Won);
			else
				frame.gamesWon = 0;
			if (xmlfile.Statistics[0].Total[0]["$Attributes"].Lost != nil)
				frame.gamesLost = std::strtonum(xmlfile.Statistics[0].Total[0]["$Attributes"].Lost);
			else
				frame.gamesLost = 0;
		}
		if (xmlfile.Statistics[0].Streaks != nil and xmlfile.Statistics[0].Streaks[0] != nil) {
			if (xmlfile.Statistics[0].Streaks[0]["$Attributes"].Won != nil)
				frame.gamesStreakWon = std::strtonum(xmlfile.Statistics[0].Streaks[0]["$Attributes"].Won);
			else
				frame.gamesStreakWon = 0;
			if (xmlfile.Statistics[0].Streaks[0]["$Attributes"].Lost)
				frame.gamesStreakLost = std::strtonum(xmlfile.Statistics[0].Streaks[0]["$Attributes"].Lost);
			else
				frame.gamesStreakLost = 0;
		}
	}
	if (xmlfile.SaveGame != nil and xmlfile.SaveGame[0] != nil) {
		for (local i = 0; i < 8; ++i) {
			frame.gameGUI.playingPileGUI[i].split(frame.gameGUI.playingPileGUI[i].getCardCollectionCard(0));
			if (xmlfile.SaveGame[0].PlayingPile[i] != nil and
				xmlfile.SaveGame[0].PlayingPile[i].cards != nil) {
				for (local j = 0; j < std::tablength(xmlfile.SaveGame[0].PlayingPile[i].cards); ++j) {
					frame.gameGUI.playingPileGUI[i].concatenate(Card::card(
						xmlfile.SaveGame[0].PlayingPile[i].cards[j]["$Attributes"].Rank,
						xmlfile.SaveGame[0].PlayingPile[i].cards[j]["$Attributes"].Suit)
					);
				}
			}
		}
		for (local i = 0; i < 4; ++i) {
			if (xmlfile.SaveGame[0].FreeCell[i] != nil and
				xmlfile.SaveGame[0].FreeCell[i].cards != nil and
				xmlfile.SaveGame[0].FreeCell[i].cards[0] != nil) {
				if (xmlfile.SaveGame[0].FreeCell[i].cards[0]["$Attributes"].Rank != nil and
					xmlfile.SaveGame[0].FreeCell[i].cards[0]["$Attributes"].Suit != nil) {
					frame.gameGUI.freeCellGUI[i].concatenate(Card::card(
						xmlfile.SaveGame[0].FreeCell[i].cards[0]["$Attributes"].Rank,
						xmlfile.SaveGame[0].FreeCell[i].cards[0]["$Attributes"].Suit));
				}
			}
		}
		for (local i = 0; i < 4; ++i) {
			if (xmlfile.SaveGame[0].Foundation[i] != nil and
				xmlfile.SaveGame[0].Foundation[i].cards != nil) {
				for (local j = 0; j < std::tablength(xmlfile.SaveGame[0].Foundation[i].cards); ++j) {
					frame.gameGUI.foundationGUI[i].concatenate(Card::card(
						xmlfile.SaveGame[0].Foundation[i].cards[j]["$Attributes"].Rank,
						xmlfile.SaveGame[0].Foundation[i].cards[j]["$Attributes"].Suit)
					);
				}
			}
		}
		if (xmlfile.SaveGame[0].UndoPile != nil) {
			for (local i = 0; i < std::tablength(xmlfile.SaveGame[0].UndoPile); ++i) {
				if (xmlfile.SaveGame[0].UndoPile[0] != nil and
					xmlfile.SaveGame[0].UndoPile[i].Source != nil and
					xmlfile.SaveGame[0].UndoPile[i].Source[0] != nil and
					xmlfile.SaveGame[0].UndoPile[i].Destination != nil and
					xmlfile.SaveGame[0].UndoPile[i].Destination[0] != nil) {

					local source = identifyCollection(xmlfile.SaveGame[0].UndoPile[i].Source, frame.gameGUI);
					local destination = identifyCollection(xmlfile.SaveGame[0].UndoPile[i].Destination, frame.gameGUI);
					local numberOfCards = std::strtonum(xmlfile.SaveGame[0].UndoPile[i]["$Attributes"].NumberOfCards);
					FreeCellCore::addUndoAction(numberOfCards, source, destination);
				}
			}
		}
		if (xmlfile.SaveGame[0].RedoPile != nil and xmlfile.SaveGame[0].RedoPile[0] != nil) {
			for (local i = 0; i < std::tablength(xmlfile.SaveGame[0].RedoPile); ++i) {
				if (xmlfile.SaveGame[0].RedoPile[i] != nil and
					xmlfile.SaveGame[0].RedoPile[i].Source != nil and
					xmlfile.SaveGame[0].RedoPile[i].Source[0] != nil and
					xmlfile.SaveGame[0].RedoPile[i].Destination != nil and
					xmlfile.SaveGame[0].RedoPile[i].Destination[0] != nil) {

					local source = identifyCollection(xmlfile.SaveGame[0].RedoPile[i].Source, frame.gameGUI);
					local destination = identifyCollection(xmlfile.SaveGame[0].RedoPile[i].Destination, frame.gameGUI);
					local numberOfCards = std::strtonum(xmlfile.SaveGame[0].RedoPile[i]["$Attributes"].NumberOfCards);
					FreeCellCore::addRedoAction(numberOfCards, source, destination);
				}
			}
		}
	}
	local checkSum = getCheckSum(frame);
	if (checkSum != std::strtonum(xmlfile["$Attributes"].CheckSum)) {
		std::print("An error occured while loading file.\n");
		return nil;
	}
	return xmlfile;
}

function savegame(frame)
{
	local checkSum = getCheckSum(frame);
	t = [{"$Name": "freecell"}];
	t["$Attributes"] = [];
	t["$Attributes"].CheckSum = checkSum;
	t.Statistics = [];
	t.Statistics[0] = [];
	t.Statistics[0].Total = [];
	t.Statistics[0].Total[0] = [];
	t.Statistics[0].Total[0]["$Attributes"] = [];
	t.Statistics[0].Total[0]["$Attributes"].Won = frame.gamesWon;
	t.Statistics[0].Total[0]["$Attributes"].Lost = frame.gamesLost;
	t.Statistics[0].Streaks = [];
	t.Statistics[0].Streaks[0] = [];
	t.Statistics[0].Streaks[0]["$Attributes"] = [];
	t.Statistics[0].Streaks[0]["$Attributes"].Won = frame.gamesStreakWon;
	t.Statistics[0].Streaks[0]["$Attributes"].Lost = frame.gamesStreakLost;
	t.Settings = [];
	t.Settings[0] = [];
	t.Settings[0].Position = [];
	t.Settings[0].Position[0] = [];
	t.Settings[0].Position[0]["$Attributes"] = [];
	t.Settings[0].Position[0]["$Attributes"].x = frame.getposition().getx();
	t.Settings[0].Position[0]["$Attributes"].y = frame.getposition().gety();
	t.Settings[0].Size = [];
	t.Settings[0].Size[0] = [];
	t.Settings[0].Size[0]["$Attributes"] = [];
	t.Settings[0].Size[0]["$Attributes"].width = frame.getsize().getwidth();
	t.Settings[0].Size[0]["$Attributes"].height = frame.getsize().getheight();
	t.Settings[0]["$Attributes"] = [];
	if (frame.ismaximized())
		t.Settings[0]["$Attributes"].Maximized = "true";
	else
		t.Settings[0]["$Attributes"].Maximized = "false";
	t.SaveGame = [];
	t.SaveGame[0] = [];
	t.SaveGame[0].Foundation = [];
	for (local i = 0; i < 4; ++i) {
		t.SaveGame[0].Foundation[i] = [];
		local cardSize = frame.gameGUI.foundationGUI[i].getSize();
		if (cardSize > 0)
			t.SaveGame[0].Foundation[i].cards = [];
		for (local j = 0; j < cardSize; ++j) {
			t.SaveGame[0].Foundation[i].cards[j] = [];
			t.SaveGame[0].Foundation[i].cards[j]["$Attributes"] = [];
			t.SaveGame[0].Foundation[i].cards[j]["$Attributes"].Suit = frame.gameGUI.foundationGUI[i].getCardCollectionCard(j).Suit;
			t.SaveGame[0].Foundation[i].cards[j]["$Attributes"].Rank = frame.gameGUI.foundationGUI[i].getCardCollectionCard(j).Rank;
		}
	}
	t.SaveGame[0].FreeCell = [];
	for (local i = 0; i < 4; ++i) {
		t.SaveGame[0].FreeCell[i] = [];
		local cardSize = frame.gameGUI.freeCellGUI[i].getSize();
		if (cardSize > 0)
			t.SaveGame[0].FreeCell[i].cards = [];
		for (local j = 0; j < cardSize; ++j) {
			t.SaveGame[0].FreeCell[i].cards[j] = [];
			t.SaveGame[0].FreeCell[i].cards[j]["$Attributes"] = [];
			t.SaveGame[0].FreeCell[i].cards[j]["$Attributes"].Suit = frame.gameGUI.freeCellGUI[i].getCardCollectionCard(j).Suit;
			t.SaveGame[0].FreeCell[i].cards[j]["$Attributes"].Rank = frame.gameGUI.freeCellGUI[i].getCardCollectionCard(j).Rank;
		}
	}
	t.SaveGame[0].PlayingPile = [];
	for (local i = 0; i < 8; ++i) {
		t.SaveGame[0].PlayingPile[i] = [];
		local cardSize = frame.gameGUI.playingPileGUI[i].getSize();
		if (cardSize > 0)
			t.SaveGame[0].PlayingPile[i].cards = [];
		for (local j = 0; j < cardSize; ++j) {
			t.SaveGame[0].PlayingPile[i].cards[j] = [];
			t.SaveGame[0].PlayingPile[i].cards[j]["$Attributes"] = [];
			t.SaveGame[0].PlayingPile[i].cards[j]["$Attributes"].Suit = frame.gameGUI.playingPileGUI[i].getCardCollectionCard(j).Suit;
			t.SaveGame[0].PlayingPile[i].cards[j]["$Attributes"].Rank = frame.gameGUI.playingPileGUI[i].getCardCollectionCard(j).Rank;
		}
	}
	t.SaveGame[0].UndoPile = [];
	undoPile = FreeCellCore::getUndoPile();
	local j = 0;
	foreach (local i, undoPile) {
		t.SaveGame[0].UndoPile[j] = [];
		t.SaveGame[0].UndoPile[j]["$Attributes"] = [];
		t.SaveGame[0].UndoPile[j]["$Attributes"].NumberOfCards = i.numberOfCards;
		t.SaveGame[0].UndoPile[j].Source = [];
		t.SaveGame[0].UndoPile[j].Source[0] = [];
		t.SaveGame[0].UndoPile[j].Source[0]["$Attributes"] = [];
		t.SaveGame[0].UndoPile[j].Source[0]["$Attributes"].Class = i.source.class;
		t.SaveGame[0].UndoPile[j].Source[0]["$Attributes"].CollectionNo = i.source.collectionNo;
		t.SaveGame[0].UndoPile[j].Destination = [];
		t.SaveGame[0].UndoPile[j].Destination[0] = [];
		t.SaveGame[0].UndoPile[j].Destination[0]["$Attributes"] = [];
		t.SaveGame[0].UndoPile[j].Destination[0]["$Attributes"].Class = i.destination.class;
		t.SaveGame[0].UndoPile[j].Destination[0]["$Attributes"].CollectionNo = i.destination.collectionNo;
		++j;
	}
	t.SaveGame[0].RedoPile = [];
	redoPile = FreeCellCore::getRedoPile();
	j = 0;
	foreach (local i, redoPile) {
		t.SaveGame[0].RedoPile[j] = [];
		t.SaveGame[0].RedoPile[j]["$Attributes"] = [];
		t.SaveGame[0].RedoPile[j]["$Attributes"].NumberOfCards = i.numberOfCards;
		t.SaveGame[0].RedoPile[j].Source = [];
		t.SaveGame[0].RedoPile[j].Source[0] = [];
		t.SaveGame[0].RedoPile[j].Source[0]["$Attributes"] = [];
		t.SaveGame[0].RedoPile[j].Source[0]["$Attributes"].Class = i.source.class;
		t.SaveGame[0].RedoPile[j].Source[0]["$Attributes"].CollectionNo = i.source.collectionNo;
		t.SaveGame[0].RedoPile[j].Destination = [];
		t.SaveGame[0].RedoPile[j].Destination[0] = [];
		t.SaveGame[0].RedoPile[j].Destination[0]["$Attributes"] = [];
		t.SaveGame[0].RedoPile[j].Destination[0]["$Attributes"].Class = i.destination.class;
		t.SaveGame[0].RedoPile[j].Destination[0]["$Attributes"].CollectionNo = i.destination.collectionNo;
		++j;
	}
	xml::store(t, "xmlfile.xml");
}

function cardCollectionGUI_new(collection)
{
	std::inherit(cardCollectionGUI = [
		@cardCollectionRect : 0,
		method drawCard(dc, card, x, y) {
			if (card.hidden){
				dc.drawbitmap(bitmaps["HIDDEN"], x, y);
			} else {
				dc.drawbitmap(bitmaps[card.Suit][card.Rank], x, y);
			}
		},
		method getX() { return self.cardCollectionRect.getx(); },
		method getY() { return self.cardCollectionRect.gety(); },
		method getSize() { return self.size; },
		method getCardCollectionCard(i) { return self.cards[i]; },
		method draw() {},
		method recalculateHeight() {}
	], collection);
	return cardCollectionGUI;
}

function playingPileGUI(playingPile, x, y)
{
	local playingPileGUI = [];
	local cardCollectionGUI = cardCollectionGUI_new(playingPile);
	std::inherit(playingPileGUI, cardCollectionGUI);
	playingPileGUI.heightIncrement = 24;
	playingPileGUI.cardCollectionRect = rect_construct(x, y, bitmaps.width,
		bitmaps.height + ((playingPileGUI.getSize() - 1) * playingPileGUI.heightIncrement));
	
	tmp = [
		method(dc) {
			dc.drawbitmap(bitmaps["EMPTY_PLAYINGPILE"], self.getX(), self.getY());
			i = 0;
			for (local i = 0; i < self.getSize(); ++i) {
				self.drawCard(dc, self.cards[i], self.getX(), self.getY() + (i * self.heightIncrement));
			}
		},
		method() {
			self.heightIncrement = 24;
			if (self.getSize() > 0) {
				local height = bitmaps.height + ((self.getSize() - 1) * self.heightIncrement);
				while (height + self.getY() > 450) {
					self.heightIncrement--;
					height = bitmaps.height + ((self.getSize() - 1) * self.heightIncrement);
				}
				self.cardCollectionRect.setheight(height);
			}
		},
		method(x, y) {
			for (local i = 0; i < self.getSize() - 1; ++i) {
				if (y > self.getY() + i * self.heightIncrement and
					y < self.getY() + (i + 1) * self.heightIncrement) {
					return i;
				}
			}
			return self.getSize() - 1;
		},
		method(other) {
			if (self.size != other.size) {
				return false;
			}
			for (local i = 0; i < self.size; ++i) {
				local selfCard = self.getCardCollectionCard(i);
				local otherCard = other.getCardCollectionCard(i);
				if (selfCard.Suit != otherCard.Suit or 
					selfCard.Rank != otherCard.Rank) {
					return false;
				}
			}
			return true;
		}
	];
	std::tabsetmethod(playingPileGUI, tmp, "draw", 0);
	std::tabsetmethod(playingPileGUI, tmp, "recalculateHeight", 1);
	std::tabsetmethod(playingPileGUI, tmp, "getPointingCard", 2);
	std::tabsetmethod(playingPileGUI, tmp, "equal", 3);
	
	return playingPileGUI;
}

function foundationGUI(foundation, x, y)
{
	local foundationGUI = [];
	local cardCollectionGUI = cardCollectionGUI_new(foundation);
	std::inherit(foundationGUI, cardCollectionGUI);
	foundationGUI.cardCollectionRect = rect_construct(x + 3, y + 3, bitmaps.width, bitmaps.height);
	tmp = [(method(dc) {
		dc.drawbitmap(bitmaps["EMPTY_PILE"], self.getX() - 3, self.getY() - 3);
		if (self.getSize() > 0) {
			self.drawCard(dc, self.cards[self.getSize() - 1], self.getX(), self.getY());
		}
	})];
	std::tabsetmethod(foundationGUI, tmp, "draw", 0);
	return foundationGUI;
}

function freeCellGUI(freecell, x, y)
{
	local freeCellGUI = [];
	local cardCollectionGUI = cardCollectionGUI_new(freecell);
	std::inherit(freeCellGUI, cardCollectionGUI);
	freeCellGUI.cardCollectionRect = rect_construct(x + 3, y + 3, bitmaps.width, bitmaps.height);
	tmp = [(method(dc) {
		dc.drawbitmap(bitmaps["EMPTY_PILE"], self.getX() - 3, self.getY() - 3);
		if (self.getSize() > 0) {
			self.drawCard(dc, self.cards[self.getSize() - 1], self.getX(), self.getY());
		}
	})];
	std::tabsetmethod(freeCellGUI, tmp, "draw", 0);
	return freeCellGUI;
}

function freeCellGameGUI_new()
{
	freeCellGameGUI = [];
	freeCellGameGUI.freeCell = FreeCellCore::FreeCell().new();
	freeCellGameGUI.freeCellGUI = [];
	for (i = 0; i < 4; ++i)
		freeCellGameGUI.freeCellGUI[i] = freeCellGUI(freeCellGameGUI.freeCell.waste[i], 17 + i * 91, 20);
	freeCellGameGUI.foundationGUI = [];
	for (i = 0; i < 4; ++i)
		freeCellGameGUI.foundationGUI[i] = foundationGUI(freeCellGameGUI.freeCell.foundation[i], 435 + i * 91, 20);
	freeCellGameGUI.playingPileGUI = [];
	for (i = 0; i < 8; ++i)
		freeCellGameGUI.playingPileGUI[i] = playingPileGUI(freeCellGameGUI.freeCell.playing[i], 64 + i * 88, 148);
	tmp = [
		method(cardCollection) {
			local collisionArea = 0;
			local collisionCollection = nil;
			local movingRect = cardCollection.cardCollectionRect;
			for (local i = 0; i < 4; ++i) {
				local collection = self.freeCellGUI[i];
				if (collection.cardCollectionRect.intersects(movingRect)) {
					local rect = rect_construct(collection.cardCollectionRect.getposition(),
												collection.cardCollectionRect.getsize());
					rect.intersect(movingRect);
					local area = rect.getwidth() * rect.getheight();
					if (area > collisionArea) {
						collisionArea = area;
						collisionCollection = collection;
					}
				}
			}
			
			for (i = 0; i < 4; ++i) {
				local collection = self.foundationGUI[i];
				if (collection.cardCollectionRect.intersects(movingRect)) {
					local rect = rect_construct(collection.cardCollectionRect.getposition(),
												collection.cardCollectionRect.getsize());
					rect.intersect(movingRect);
					local area = rect.getwidth() * rect.getheight();
					if (area > collisionArea) {
						collisionArea = area;
						collisionCollection = collection;
					}
				}
			}
			
			for (i = 0; i < 8; ++i) {
				local collection = self.playingPileGUI[i];
				if (collection.cardCollectionRect.intersects(movingRect)) {
					local rect = rect_construct(collection.cardCollectionRect.getposition(),
												collection.cardCollectionRect.getsize());
					rect.intersect(movingRect);
					local area = rect.getwidth() * rect.getheight();
					if (area > collisionArea) {
						collisionArea = area;
						collisionCollection = collection;
					}
				}
			}
			if (collisionArea > 0) {
				return collisionCollection;
			} else {
				return nil;
			}
		},
		method(frame) {
			frame.dc.getuserscale(t = [], []);
			local userScale = t[0];
			
			//Draw everything to a buffer and copy it to the dc when the drawing phase finish.
			//This way there is no flickering.
			buffer = buffereddc_construct(frame.dc, size_construct(frame.getsize().getwidth()/userScale, frame.getsize().getheight()/userScale));
			
			//Draw background
			local brush = brush_construct(colour_construct(20,100,20));
			buffer.setbrush(brush);
			buffer.drawrectangle(0, 0, frame.getsize().getwidth()/userScale, frame.getsize().getheight()/userScale);
			
			//Draw the playing piles
			for (local i = 0; i < std::tablength(self.playingPileGUI); ++i) {
				self.playingPileGUI[i].draw(buffer);
			}
			
			//Draw the foundations
			for (local i = 0; i < std::tablength(self.foundationGUI); ++i) {
				self.foundationGUI[i].draw(buffer);
			}
			
			//Draw the playing piles
			for (local i = 0; i < std::tablength(self.freeCellGUI); ++i) {
				self.freeCellGUI[i].draw(buffer);
			}
			
			if (self.movingPile != nil) {
				self.movingPile.draw(buffer);
			}
		},
		method (x, y) {
			for (local i = 0; i < 8; ++i) {
				if (self.playingPileGUI[i].cardCollectionRect.contains(x, y)) {
					return self.playingPileGUI[i];
				}
			}
			for (local i = 0; i < 4; ++i) {
				if (self.foundationGUI[i].cardCollectionRect.contains(x, y)) {
					return self.foundationGUI[i];
				}
			}
			for (local i = 0; i < 4; ++i) {
				if (self.freeCellGUI[i].cardCollectionRect.contains(x, y)) {
					return self.freeCellGUI[i];
				}
			}
			return nil;
		}
	];
	std::tabsetmethod(freeCellGameGUI, tmp, "detectCollision", 0);
	std::tabsetmethod(freeCellGameGUI, tmp, "redraw", 1);
	std::tabsetmethod(freeCellGameGUI, tmp, "getPointingCollection", 2);
	return freeCellGameGUI;
}

function onMoveEvent(frame, ev)
{
	frame.gameGUI.redraw(frame);
}

function onSizeEvent(frame, ev)
{
	userScaleX = frame.getsize().getwidth()/815;
	userScaleY = frame.getsize().getheight()/496;
	if (userScaleX < 1) {
		if (userScaleY < userScaleX) {
			userScaleX = userScaleY;
		}
	} else {
		if (userScaleY < userScaleX) {
			userScaleX = userScaleY;
		}
	}
	frame.dc.setuserscale(userScaleX, userScaleX);

	frame.gameGUI.redraw(frame);
}

function onAbout(frame, ev) {
	info = aboutdialoginfo_construct();
	info.setname("FreeCell");
	info.setversion("1.0");
	info.setdevelopers(["Andreas Maragoudakis", "Christos Despotakis"]);
	info.setdescription("This is a test project for wxWidgets library for DELTA.");
	aboutbox(info);
}

function onNewGame(frame, ev) {
	//frame.gameGUI = nil;
	frame.gameGUI = freeCellGameGUI_new();
	frame.gameGUI.redraw(frame);
	if (frame.gamesLost != nil)
		frame.gamesLost++;
	if (frame.gamesStreakLost != nil) {
		frame.gamesStreakLost++;
		frame.gamesStreakWon = 0;
	}
	std::print("New Game\n");
}

function onUndo(frame, ev) {
	retval = FreeCellCore::undo();
	if (retval != nil) {
		retval.destination.recalculateHeight();
		retval.source.recalculateHeight();
	}
	frame.gameGUI.redraw(frame);
	std::print("Undo\n");
}

function onRedo(frame, ev) {
	retval = FreeCellCore::redo();
	if (retval != nil) {
		retval.destination.recalculateHeight();
		retval.source.recalculateHeight();
	}
	frame.gameGUI.redraw(frame);
	std::print("Redo\n");
}

function onStatistics(frame, ev) {
	dialog = dialog_construct(frame, "ID_ANY", "FreeCell Statistics");
	sizer = boxsizer_construct("VERTICAL");
	dialog.setsizer(sizer);
	gamesWonCtrl = statictext_construct(dialog, "ID_ANY", "Games Won: " + frame.gamesWon);
	sizer.add(gamesWonCtrl, 0, flags("CENTER", "ALL"), 5);
	gamesLostCtrl = statictext_construct(dialog, "ID_ANY", "Games Lost: " + frame.gamesLost);
	sizer.add(gamesLostCtrl, 0, flags("CENTER", "ALL"), 5);
	local gamesPlayed = frame.gamesWon + frame.gamesLost;
	gamesPlayedCtrl = statictext_construct(dialog, "ID_ANY", "Games Played: " + gamesPlayed);
	sizer.add(gamesPlayedCtrl, 0, flags("CENTER", "ALL"), 5);
	winPercentageCtrl = statictext_construct();
	if (gamesPlayed != 0)
		winPercentageCtrl.create(dialog, "ID_ANY", "Win percentage: " + frame.gamesWon / gamesPlayed * 100 + "%");
	else
		winPercentageCtrl.create(dialog, "ID_ANY", "Win percentage: 0%");
	sizer.add(winPercentageCtrl, 0, flags("CENTER", "ALL"), 5);
	sizer.addspacer(20);
	gamesWonStreakCtrl = statictext_construct(dialog, "ID_ANY", "Winning streak: " + frame.gamesStreakWon);
	sizer.add(gamesWonStreakCtrl, 0, flags("CENTER", "ALL"), 5);
	gamesLostStreakCtrl = statictext_construct(dialog, "ID_ANY", "Losing streak: " + frame.gamesStreakLost);
	sizer.add(gamesLostStreakCtrl, 0, flags("CENTER", "ALL"), 5);
	buttonSizer = dialog.createbuttonsizer("OK");
	sizer.add(buttonSizer, 0, flags("CENTER", "ALL"), 5);
	dialog.showmodal();
	dialog.destroy();
}

function onExit(frame, ev) {
//Save changes
	savegame(frame);
//Destruction
	frame.destroy();
}

function onLeftDownEvent(frame, ev) {
	std::print("Left Mouse Button Down (", ev.getx(), ",", ev.gety(), ")\n");
	local logicalX = frame.dc.devicetologicalx(ev.getx());
	local logicalY = frame.dc.devicetologicaly(ev.gety());
	if (frame.gameGUI.movingPile != nil) {
		frame.gameGUI.freeCell.move(frame.gameGUI.movingPile, frame.gameGUI.sourcePile, frame.gameGUI.sourcePile);
		frame.gameGUI.movingPile = nil;
		frame.gameGUI.sourcePile = nil;
	}
	local pointingCollection = frame.gameGUI.getPointingCollection(logicalX, logicalY);
	if (pointingCollection != nil) {
		if (pointingCollection.class == "foundation") {
			std::print("Pressed foundation\n");
		}
		if (pointingCollection.class == "playing") {
			cardNo = pointingCollection.getPointingCard(logicalX, logicalY);
			if (cardNo == -1)
				return;
			card = pointingCollection.getCardCollectionCard(cardNo);
			if (pointingCollection.selectRules(card)) {
				frame.gameGUI.movingPile = playingPileGUI(
					pointingCollection.split(card),
					pointingCollection.getX(),
					pointingCollection.getY() + cardNo * pointingCollection.heightIncrement
				);
				frame.gameGUI.sourcePile = pointingCollection;
				frame.gameGUI.movingPile.relativeX = logicalX - frame.gameGUI.movingPile.getX();
				frame.gameGUI.movingPile.relativeY = logicalY - frame.gameGUI.movingPile.getY();
			}
			std::print("Pressed playing pile\n");
			std::print("Card:", card.Suit, ", ", card.Rank, "\n");
		}
		if (pointingCollection.class == "waste") {
			std::print("Pressed freecell\n");
			if (pointingCollection.getSize() == 1) {
				card = pointingCollection.getCardCollectionCard(0);
				frame.gameGUI.movingPile = playingPileGUI(
					pointingCollection.split(card),
					pointingCollection.getX(),
					pointingCollection.getY()
				);
				frame.gameGUI.sourcePile = pointingCollection;
				frame.gameGUI.movingPile.relativeX = logicalX - pointingCollection.getX();
				frame.gameGUI.movingPile.relativeY = logicalY - pointingCollection.getY();
			}
		}
	}
}

function onLeftUpEvent(frame, ev) {
	std::print("Left Mouse Button Up\n");
	if (frame.gameGUI.movingPile != nil) {
		destinationPile = frame.gameGUI.detectCollision(frame.gameGUI.movingPile);
		if (destinationPile != nil and destinationPile != frame.gameGUI.sourcePile) {
			frame.gameGUI.freeCell.move(frame.gameGUI.movingPile, frame.gameGUI.sourcePile, destinationPile);
			if (destinationPile.class == "playing") {
				destinationPile.recalculateHeight();
			}
			if (frame.gameGUI.sourcePile.class == "playing") {
				frame.gameGUI.sourcePile.recalculateHeight();
			}
			frame.checkWinConditions();
		} else if (destinationPile == frame.gameGUI.sourcePile) {
			frame.gameGUI.sourcePile.concatenate(frame.gameGUI.movingPile);
		} else {
			frame.gameGUI.sourcePile.concatenate(frame.gameGUI.movingPile);
		}
		frame.gameGUI.movingPile = nil;
		frame.gameGUI.sourcePile = nil;
		frame.gameGUI.redraw(frame);
	}
}

function onRightDownEvent(frame, ev) {
	std::print("Right Mouse Button Down (", ev.getx(), ",", ev.gety(), ")\n");
	local logicalX = frame.dc.devicetologicalx(ev.getx());
	local logicalY = frame.dc.devicetologicaly(ev.gety());
	if (frame.gameGUI.movingPile != nil) {
		frame.gameGUI.freeCell.move(frame.gameGUI.movingPile, frame.gameGUI.sourcePile, frame.gameGUI.sourcePile);
		frame.gameGUI.movingPile = nil;
		frame.gameGUI.sourcePile = nil;
	}
	local pointingCollection = frame.gameGUI.getPointingCollection(logicalX, logicalY);
	if (pointingCollection != nil) {
		if (pointingCollection.class == "foundation") {
			std::print("Pressed foundation\n");
		}
		if (pointingCollection.class == "playing") {
			cardNo = pointingCollection.getPointingCard(logicalX, logicalY);
			if (cardNo == -1)
				return;
			card = pointingCollection.getCardCollectionCard(cardNo);
			local tmpPile = pointingCollection.split(card);
			local finished = false;

			for (local i = 0; i < 4; ++i) {	//Check if the card can move to foundation
				if (pointingCollection.selectRules(card)) {
					if (frame.gameGUI.foundationGUI[i].moveRules(tmpPile)) {
						frame.gameGUI.freeCell.move(
							tmpPile, pointingCollection, frame.gameGUI.foundationGUI[i]
						);
						frame.checkWinConditions();
						finished = true;
					}
				}
			}
			if (finished == true) {
				pointingCollection.recalculateHeight();
				frame.gameGUI.redraw(frame);
			} else {
				pointingCollection.concatenate(tmpPile);
			}
			std::print("Pressed playing pile\n");
			std::print("Card:", card.Suit, ", ", card.Rank, "\n");
		}
		if (pointingCollection.class == "waste") {
			local finished = false;
			for (local i = 0; i < 4; ++i) {	//Check if the card can move to foundation
				if (frame.gameGUI.foundationGUI[i].moveRules(pointingCollection)) {
					frame.gameGUI.freeCell.move(
						pointingCollection, pointingCollection, frame.gameGUI.foundationGUI[i]
					);
					frame.checkWinConditions();
					finished = true;
				}
			}
			if (finished == true) {
				frame.gameGUI.redraw(frame);
			}
			std::print("Pressed freecell\n");
		}
	} else {
		std::print("Pressed empty space\n");
		frame.gameGUI.freeCell.moveToFoundation();
		frame.checkWinConditions();
		for (local i = 0; i < 8; ++i) {
			frame.gameGUI.playingPileGUI[i].recalculateHeight();
		}
		frame.gameGUI.redraw(frame);
	}
}

function onMotionEvent(frame, ev) {
	if (ev.leftisdown()) {
		local logicalX = frame.dc.devicetologicalx(ev.getx());
		local logicalY = frame.dc.devicetologicaly(ev.gety());		
		std::print("Mouse Dragged to (" + ev.getx() + "," + ev.gety() + ")\n");
		if (frame.gameGUI.movingPile != nil) {
			frame.gameGUI.movingPile.cardCollectionRect.setx(logicalX - frame.gameGUI.movingPile.relativeX);
			frame.gameGUI.movingPile.cardCollectionRect.sety(logicalY - frame.gameGUI.movingPile.relativeY);
		}
		frame.gameGUI.redraw(frame);
	}
}

function onLeftDoubleClickEvent(frame, ev) {
	std::print("Left Mouse Button Doubleclicked\n");
	local logicalX = frame.dc.devicetologicalx(ev.getx());
	local logicalY = frame.dc.devicetologicaly(ev.gety());
	if (frame.gameGUI.movingPile != nil) {
		frame.gameGUI.freeCell.move(frame.gameGUI.movingPile, frame.gameGUI.sourcePile, frame.gameGUI.sourcePile);
		frame.gameGUI.movingPile = nil;
		frame.gameGUI.sourcePile = nil;
	}
	local pointingCollection = frame.gameGUI.getPointingCollection(logicalX, logicalY);
	if (pointingCollection != nil) {
		if (pointingCollection.class == "foundation") {
			std::print("Doubleclicked foundation\n");
		}
		if (pointingCollection.class == "waste") {
			if (pointingCollection.getSize() == 1) {
				local finished = false;
				for (local i = 0; i < 8; ++i) {	//Check all non-empty playing piles for legal moves
					if (finished == false and frame.gameGUI.playingPileGUI[i].getSize() != 0 and
						frame.gameGUI.playingPileGUI[i].moveRules(pointingCollection)) {
						frame.gameGUI.freeCell.move(
							pointingCollection, pointingCollection, frame.gameGUI.playingPileGUI[i]
						);
						frame.gameGUI.playingPileGUI[i].recalculateHeight();
						finished = true;
					}
				}
				for (local i = 0; i < 8; ++i) {	//Check if there are any empty playing piles
					if (finished == false and frame.gameGUI.playingPileGUI[i].getSize() == 0) {
						frame.gameGUI.freeCell.move(
							pointingCollection, pointingCollection, frame.gameGUI.playingPileGUI[i]
						);
						frame.gameGUI.playingPileGUI[i].recalculateHeight();
						finished = true;
					}
				}
				for (local i = 0; i < 4; ++i) {	//Check if the card can be added to a foundation pile
					if (finished == false and
						frame.gameGUI.foundationGUI[i].moveRules(pointingCollection)) {
						frame.gameGUI.freeCell.move(
							pointingCollection, pointingCollection, frame.gameGUI.foundationGUI[i]
						);
						frame.checkWinConditions();
						finished = true;
					}
				}
				if (finished == true) {
					frame.gameGUI.redraw(frame);
				}
			}
			std::print("Doubleclicked freecell\n");
		}
		if (pointingCollection.class == "playing") {
			cardNo = pointingCollection.getPointingCard(logicalX, logicalY);
			if (cardNo == -1)
				return;
			card = pointingCollection.getCardCollectionCard(cardNo);
			local tmpPile = pointingCollection.split(card);
			local finished = false;
			
			for (local i = 0; i < 8; ++i) {	//Check if the pile can move to another non-empty playing pile
				if (pointingCollection.equal(frame.gameGUI.playingPileGUI[i]))
					continue;
				if (finished == false and frame.gameGUI.playingPileGUI[i].getSize() != 0 and
					pointingCollection.selectRules(card)) {
					if (frame.gameGUI.playingPileGUI[i].moveRules(tmpPile)) {
						frame.gameGUI.freeCell.move(
							tmpPile, pointingCollection, frame.gameGUI.playingPileGUI[i]
						);
						frame.gameGUI.playingPileGUI[i].recalculateHeight();
						finished = true;
					}
				}
			}
			
			for (local i = 0; i < 8; ++i) {	//Check if there is another empty playing pile
				if (pointingCollection.equal(frame.gameGUI.playingPileGUI[i]))
					continue;
				if (finished == false and frame.gameGUI.playingPileGUI[i].getSize() == 0 and
					pointingCollection.selectRules(card)) {
					if (frame.gameGUI.playingPileGUI[i].moveRules(tmpPile)) {
						frame.gameGUI.freeCell.move(
							tmpPile, pointingCollection, frame.gameGUI.playingPileGUI[i]
						);
						frame.gameGUI.playingPileGUI[i].recalculateHeight();
						finished = true;
					}
				}
			}
			
			for (local i = 0; i < 4; ++i) {	//Check if there is an empty freecell
				if (finished == false and
					pointingCollection.selectRules(card)) {
					if (frame.gameGUI.freeCellGUI[i].moveRules(tmpPile)) {
						frame.gameGUI.freeCell.move(
							tmpPile, pointingCollection, frame.gameGUI.freeCellGUI[i]
						);
						finished = true;
					}
				}
			}
			
			for (local i = 0; i < 4; ++i) {	//Check if the card can move to foundation
				if (finished == false and
					pointingCollection.selectRules(card)) {
					if (frame.gameGUI.foundationGUI[i].moveRules(tmpPile)) {
						frame.gameGUI.freeCell.move(
							tmpPile, pointingCollection, frame.gameGUI.foundationGUI[i]
						);
						frame.checkWinConditions();
						finished = true;
					}
				}
			}
			
			if (finished == true) {
				pointingCollection.recalculateHeight();
				frame.gameGUI.redraw(frame);
			} else {
				pointingCollection.concatenate(tmpPile);
			}
			std::print("Doubleclicked playing pile\n");
		}
	}
}

function onHelp(frame, ev)
{
	messagebox("Try to put all cards to the foundations (top right piles) to win the game.\n" +
			   "Controls\n" +
			   "Drag & Drop: Click and drag a card or a collection of cards.\n" +
			   "If the collection doesn't move, then the collection dragged is not valid.\n" +
			   "Doubleclick: Instead of drag & drop.\n" +
			   "Right click: Send the pointing card to the foundation pile\n" +
			   "If you right click in empty space, all the cards that can be moved\n" +
			   "to the foundation piles with a legal move, will move there.",
			   "Help about FreeCell",
			   flags(wx::OK, wx::ICON_INFORMATION));
}

function oninitfunc()
{
	frame = frame_construct(nil, "ID_ANY", "FreeCell");
	//Add methods to wxframe.
	tmp = [method ()
		{
			local foundationSize = std::tablength(self.gameGUI.foundationGUI);
			for (local i = 0; i < foundationSize; ++i) {
				if (self.gameGUI.foundationGUI[i].getSize() != 13) {
					return false;
				}
			}
			if (self.gamesWon != nil)
				self.gamesWon++;
			if (self.gamesStreakWon != nil) {
				self.gamesStreakWon++;
				self.gamesStreakLost = 0;
			}
			messagebox("You won the game", "Congratulation");
			//self.gameGUI = nil;
			self.gameGUI = freeCellGameGUI_new();
			self.gameGUI.redraw(self);
			return true;
		},
		method (xmlfile)
		{
			if (xmlfile != nil) {
				self.setsize(std::strtonum(xmlfile.Settings[0].Position[0]["$Attributes"].x), std::strtonum(xmlfile.Settings[0].Position[0]["$Attributes"].y),
							 std::strtonum(xmlfile.Settings[0].Size[0]["$Attributes"].width), std::strtonum(xmlfile.Settings[0].Size[0]["$Attributes"].height));
				if (xmlfile.Settings[0]["$Attributes"].Maximized == "true")
					self.maximize(true);
			}
		}
	];
	std::tabsetmethod(std::externiduserdata(frame), tmp, "checkWinConditions", 0);
	std::tabsetmethod(std::externiduserdata(frame), tmp, "loadGameSettings", 1);
	frame.show(true);
	
	//CREATE MENUS
	
	gameMenu = menu_construct();
	helpMenu = menu_construct();
	helpMenu.append(11, "Help\tF1", "Help about FreeCell");
	helpMenu.append(1, "About", "About FreeCell");
	gameMenu.append(2, "&New Game\tAlt-N", "Begins a new game");
	gameMenu.append(5, "Statistics\tCtrl-S", "Show game statistics");
	gameMenu.appendseparator();
	gameMenu.append(3, "Undo\tCtrl-Z", "Undo a move");
	gameMenu.append(4, "Redo\tCtrl-Y", "Redo a move");
	gameMenu.appendseparator();
	gameMenu.append(6, "E&xit\tAlt-F4", "Quit the game");
	menus = [gameMenu, helpMenu];
	titles = ["&Game", "&Help"];
	menuBar = menubar_construct(2, menus, titles);
	frame.setmenubar(menuBar);
	menuBar.enabletop(1, true);

//Start New Game at startup.
	frame.dc = clientdc_construct(frame);
	frame.setsize(815, 496);
	
	//Load from xml file
	frame.gameGUI = freeCellGameGUI_new();
	xmlfile = loadgame(frame);
	if (xmlfile == nil) {
		frame.gameGUI = freeCellGameGUI_new();
	}

	//All the events connect at start-up
	frame.connect("EVT_SIZE", onSizeEvent);
	frame.connect("EVT_MOVE", onMoveEvent);
	frame.connect(1, "EVT_COMMAND_MENU_SELECTED", onAbout);
	frame.connect(2, "EVT_COMMAND_MENU_SELECTED", onNewGame);
	frame.connect(3, "EVT_COMMAND_MENU_SELECTED", onUndo);
	frame.connect(4, "EVT_COMMAND_MENU_SELECTED", onRedo);
	frame.connect(5, "EVT_COMMAND_MENU_SELECTED", onStatistics);
	frame.connect(6, "EVT_COMMAND_MENU_SELECTED", onExit);
	frame.connect(11, "EVT_COMMAND_MENU_SELECTED", onHelp);
	frame.connect("EVT_CLOSE", onExit);
	frame.connect("EVT_LEFT_DOWN", onLeftDownEvent);
	frame.connect("EVT_LEFT_UP", onLeftUpEvent);
	frame.connect("EVT_RIGHT_DOWN", onRightDownEvent);
	frame.connect("EVT_MOTION", onMotionEvent);
	frame.connect("EVT_LEFT_DCLICK", onLeftDoubleClickEvent);
	
	//Change the size & position of the window using the xml file to get all
	//the information from the previous game played.
	frame.loadGameSettings(xmlfile);
	frame.gameGUI.redraw(frame);
}

app = app_construct();

app.oninitadd(oninitfunc);
app.start();

std::dllunimportdeltalib(dll);
std::dllunimportdeltalib(dll2);