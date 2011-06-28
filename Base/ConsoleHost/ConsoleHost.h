/**
 *	ConsoleHost.h
 *
 *	-- Console Host --
 *
 *	Class that execute another console process
 *	redirecting its output input streams.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#ifndef CONSOLE_HOST_H_
#define CONSOLE_HOST_H_

#include "Common.h"
#include "LibraryAPIDecls.h"
#include <boost/function.hpp>

namespace util
{
	//----------------------------
	//-- class ConsoleHost

	class _BASE_API ConsoleHost
	{
	public:
		///--- type definitions
		typedef boost::function<void (const Buffer& buffer)> OnOutput;
		typedef boost::function<void (bool success)> OnFinish;

		///--- constructors / destructor
		ConsoleHost(bool single_thread=true)
			: single_thread(single_thread), onOutput((OnOutput) 0), onFinish((OnFinish) 0) {}
		~ConsoleHost(void) {}

		///--- public API
		static void Initialize (void);
		static void CleanUp (void);	

		unsigned long Execute(
			const std::string& application,
			const std::string& currentDirectory = "",
			OnOutput onOutput = (OnOutput) 0,
			OnFinish onFinish = (OnFinish) 0
		);
		void ExecuteShell(
			const std::string& file,
			const std::string& parameters = "",
			const std::string& currentDirectory = ""
		);

		bool TerminateProcess(unsigned long pid);

		void SendToChild(const Buffer& buffer);
		void ReadFromChild(void);

		///--- overloaded operators
		unsigned long operator ()(
			const std::string& application,
			const std::string& currentDirectory = "",
			OnOutput onOutput = (OnOutput) 0,
			OnFinish onFinish = (OnFinish) 0
		)
			{ return Execute(application, currentDirectory, onOutput, onFinish); }

		ConsoleHost& operator <<(const Buffer& buffer)
			{ SendToChild(buffer); return *this; }

		void DisplayExecutionError (char* errorId);

	private:	
		///--- private API

		///--- private members
		bool single_thread;
		OnOutput onOutput;		///< on child process output callback
		OnFinish onFinish;		///< on child process finish callback

		void* hInputWrite;		///< pipe sending input to child
		void* hOutputRead;		///< pipe receiving output from child
	};
}

#endif	//CONSOLE_HOST_
