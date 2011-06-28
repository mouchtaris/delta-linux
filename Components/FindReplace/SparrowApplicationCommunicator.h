/*
 * SparrowApplicationCommunicator.h 
 * inherit from ApplicationCommunicator
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef SPARROWAPPLICATIONCOMMUNICATOR_H
#define SPARROWAPPLICATIONCOMMUNICATOR_H


#include <string>
#include "ApplicationCommunicator.h"


namespace ide {

	class SparrowApplicationCommunicator : public frep::ApplicationCommunicator {
	private:


	public:
		virtual bool	CanDoUndoAction (void) const { return true; } 
		virtual void	BeginUndoAction (void);
		virtual void	EndUndoAction (void);

		virtual bool	CanWriteToOutput (void) const;
		virtual void	WriteToOutput (const std::string &) const;



		SparrowApplicationCommunicator(void) {}
		~SparrowApplicationCommunicator(void) {}
	};

}	// namespace ide

#endif	// SPARROWAPPLICATIONCOMMUNICATOR_H