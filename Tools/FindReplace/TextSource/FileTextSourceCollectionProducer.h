#ifndef FILETEXTSOURCECOLLECTIONPRODUCER_H
#define FILETEXTSOURCECOLLECTIONPRODUCER_H


#include "TextSource.h"
#include "TextSourceCollectionProducer.h"



class FileTextSourceCollectionProducer : public frep::TextSourceCollectionProducer {
private:
	//Create list
	std::list<frep::TextSource *> streamList;


public:

	virtual std::list<frep::TextSource *> & GetTextSourceList (frep::LookIn);

	FileTextSourceCollectionProducer(void){}
	~FileTextSourceCollectionProducer(void){}
};


#endif