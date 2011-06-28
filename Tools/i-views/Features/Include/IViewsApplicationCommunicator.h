#ifndef IVIEWS_APPLICATION_COMMUNICATOR_H
#define IVIEWS_APPLICATION_COMMUNICATOR_H

#include "ApplicationCommunicator.h"
namespace iviews {
	
	class IViewsApplicationCommunicator : public frep::ApplicationCommunicator {
	public:
		IViewsApplicationCommunicator(void);

		virtual bool CanDoUndoAction (void) const;
		virtual void BeginUndoAction (void);
		virtual void EndUndoAction (void);
		virtual bool CanWriteToOutput (void) const;
		virtual void WriteToOutput (const std::string &) const;

	private:
	};

}	//namespace iviews

#endif	//IVIEWS_APPLICATION_COMMUNICATOR_H