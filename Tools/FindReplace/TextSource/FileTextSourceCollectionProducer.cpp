/*
 * FileTextSourceCollectionProducer.cpp
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@csd.uoc.gr	
 */

#include "FileTextSourceCollectionProducer.h"
#include "FileTextSource.h"


using namespace frep;

std::list<TextSource *> & FileTextSourceCollectionProducer::GetTextSourceList (LookIn) {
	
	streamList.clear();		//sos auto prepei na to prosexw

	FileTextSource * file1 = new FileTextSource();
	FileTextSource * file2 = new FileTextSource();
	FileTextSource * file3 = new FileTextSource();
	FileTextSource * file4 = new FileTextSource();
	FileTextSource * file5 = new FileTextSource();
	FileTextSource * file6 = new FileTextSource();
	FileTextSource * file7 = new FileTextSource();
	FileTextSource * file8 = new FileTextSource();
	FileTextSource * file9 = new FileTextSource();
	FileTextSource * file10 = new FileTextSource();
	FileTextSource * file11 = new FileTextSource();
	FileTextSource * file12 = new FileTextSource();
	FileTextSource * file13 = new FileTextSource();
	FileTextSource * file14 = new FileTextSource();
	FileTextSource * file15 = new FileTextSource();
	FileTextSource * file16 = new FileTextSource();
	FileTextSource * file17 = new FileTextSource();
	FileTextSource * file18 = new FileTextSource();
	FileTextSource * file19 = new FileTextSource();
	FileTextSource * file20 = new FileTextSource();
	FileTextSource * file21 = new FileTextSource();
	FileTextSource * file22 = new FileTextSource();
	FileTextSource * file23 = new FileTextSource();
	FileTextSource * file24 = new FileTextSource();
	FileTextSource * file25 = new FileTextSource();
	FileTextSource * file26 = new FileTextSource();
	FileTextSource * file27 = new FileTextSource();
	FileTextSource * file28 = new FileTextSource();
	FileTextSource * file29 = new FileTextSource();
	FileTextSource * file30 = new FileTextSource();
 

	file1->SetPath("test.txt");
	file2->SetPath("test2.txt");
	file3->SetPath("array1.dsc");
	file4->SetPath("array2.dsc");
	file5->SetPath("fibo.dsc");
	file6->SetPath("functionCall.dsc");
	file7->SetPath("queens.dsc");
	file8->SetPath("test_adim.dsc");
	file9->SetPath("test_args.dsc");
	file10->SetPath("test_argument.dsc");
	file11->SetPath("test_arguments.dsc");
	file12->SetPath("test_array.dsc");
	file13->SetPath("test_array2.dsc");
	file14->SetPath("test_bool.dsc");
	file15->SetPath("test_dest_array.dsc");
	file16->SetPath("test_visitor.dsc");
	file17->SetPath("test_indexed.dsc");
	file18->SetPath("test_indices.dsc");
	file19->SetPath("test_input.dsc");
	file20->SetPath("test_multifunc.dsc");
	file21->SetPath("test_multindexed.dsc");
	file22->SetPath("test_nil_table.dsc");
	file23->SetPath("test_nokena.dsc");
	file24->SetPath("test_rel.dsc");
	file25->SetPath("test_returnindices.dsc");
	file26->SetPath("test_scope.dsc");
	file27->SetPath("test_simple_tablecopy.dsc");
	file28->SetPath("test_str_array.dsc");
	file29->SetPath("test_tablecopy.dsc");
	file30->SetPath("test_typeof.dsc");
	
	//streamList.push_back(file1);
	streamList.push_back(file2);
	/*streamList.push_back(file3);
	streamList.push_back(file4);
	streamList.push_back(file5);
	streamList.push_back(file6);
	streamList.push_back(file7);
	streamList.push_back(file8);
	streamList.push_back(file9);
	streamList.push_back(file10);
	streamList.push_back(file11);
	streamList.push_back(file12);
	streamList.push_back(file13);
	streamList.push_back(file14);
	streamList.push_back(file15);
	streamList.push_back(file16);
	streamList.push_back(file17);
	streamList.push_back(file18);
	streamList.push_back(file19);
	streamList.push_back(file20);
	streamList.push_back(file21);
	streamList.push_back(file22);
	streamList.push_back(file23);
	streamList.push_back(file24);
	streamList.push_back(file25);
	streamList.push_back(file26);
	streamList.push_back(file27);
	streamList.push_back(file28);
	streamList.push_back(file29);
	streamList.push_back(file30);*/
	
	return streamList;
}

