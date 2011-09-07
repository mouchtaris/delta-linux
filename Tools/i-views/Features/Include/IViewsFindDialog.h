/**
 *	IViewsFindDialog.h
 *
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr>
 *	Octomber 2010
 */

#ifndef IVIEWS_FIND_FIALOG_H
#define IVIEWS_FIND_FIALOG_H

#include "FindDialog.h"

namespace iviews {
	
	class IViewsFindDialog : public frep::FindDialog {
	public:
		IViewsFindDialog (
			wxWindow*, 
			frep::TextSourceCollectionProducer *, 
			frep::ApplicationCommunicator *
		);

	protected:
		virtual void OnLookIn (wxCommandEvent& event);
	private:

		DECLARE_EVENT_TABLE()
	};
}	//namespace iviews



#endif //IVIEWS_FIND_FIALOG_H