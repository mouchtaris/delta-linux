/*
 * ApplicationCommunicator.h
 * Abstract Class
 * author: Koutsopoulos Nikolaos
 * mail	 : koutsop@ics.forth.gr	
 */


#ifndef APPLICATIONCOMMUNICATOR_H
#define APPLICATIONCOMMUNICATOR_H

#include <string>

namespace frep {

	class ApplicationCommunicator {
	public:
		virtual ~ApplicationCommunicator(void) {}
		ApplicationCommunicator(void) : editorChanged(false), editorPath("") {}

		virtual bool		CanDoUndoAction (void) const = 0;
		virtual void		BeginUndoAction (void) = 0;
		virtual void		EndUndoAction (void) = 0;
		virtual bool		CanWriteToOutput (void) const = 0;
		virtual void		WriteToOutput (const std::string &) const = 0;

		virtual bool		HasEditorChanged(void) { return editorChanged; }
		virtual void		ChangeEditor(const bool b) { editorChanged = true; }
		virtual void		SetEditorPath(const std::string & path) { editorPath = path; }
		virtual std::string	GetEditorPath(void) { return editorPath; }


	private:
		bool		editorChanged;
		std::string	editorPath;
	};

}	//namespace frep;

#endif