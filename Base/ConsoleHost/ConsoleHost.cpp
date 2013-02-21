/**
 *	ConsoleHost.cpp
 *
 *	-- Console Host --
 *
 *	Class that execute another console process
 *	redirecting its output input streams.
 *
 *	Themistoklis Bourdenas <themis@ics.forth.gr>
 *	June 2007
 */
#include "ConsoleHost.h"
#include "GenericDialogs.h"

#include <windows.h>
#include <shellapi.h>
#include <AtlBase.h>
#include <AtlConv.h>

#pragma warning(disable:4311)
#pragma comment(lib, "User32.lib")

#define EXECUTION_ERROR(error, retval)	do { DisplayExecutionError(error); return retval; } while(false)
#define	RETURN_EMPTY

namespace util
{
	//-------------------------------------------------------//
	//---- Free Functions Declarations ----------------------//

	static HANDLE _LaunchChild(ConsoleHost* host, const std::string& application, const std::string& currentDirectory,
		HANDLE hChildStdOut, HANDLE hChildStdIn, HANDLE hChildStdErr);
	static DWORD WINAPI _gReadFromChild(LPVOID lpvThreadParam);
	
	static TCHAR*	strduplicate(const std::string& str);
	static void		strfree		(TCHAR* str);

	//-------------------------------------------------------//
	//---- class ConsoleHost --------------------------------//

	std::list<ConsoleHost*> ConsoleHost::hosts;

	//-----------------------------------------------------------------------

	ConsoleHost::ConsoleHost(bool single_thread) 
		: single_thread(single_thread), forceFinish(false), pid(0), onOutput((OnOutput) 0), onFinish((OnFinish) 0)
	{
		hosts.push_back(this);
	}
	ConsoleHost::~ConsoleHost(void) { hosts.remove(this); }

	//-----------------------------------------------------------------------

	void ConsoleHost::Initialize (void)
	{
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	}

	//-----------------------------------------------------------------------

	void ConsoleHost::CleanUp (void)
	{
	}

	//-----------------------------------------------------------------------

	unsigned long ConsoleHost::Execute(const std::string& application, const std::string& currentDirectory,
		OnOutput onOutput, OnFinish onFinish)
	{
		//HANDLE hChildProcess;

		if (onOutput) {
			this->onOutput = onOutput;
			this->onFinish = onFinish;

			HANDLE hOutputReadTmp, /*hOutputRead, */hOutputWrite;
			HANDLE hInputWriteTmp, hInputRead;//, hInputWrite;
			HANDLE hErrorWrite;
			SECURITY_ATTRIBUTES sa;

			//-- Set up the security attributes struct.
			sa.nLength= sizeof(SECURITY_ATTRIBUTES);
			sa.lpSecurityDescriptor = NULL;
			sa.bInheritHandle = TRUE;

			//-- Create the child output pipe.
			if (!CreatePipe(&hOutputReadTmp, &hOutputWrite, &sa, 0))
				EXECUTION_ERROR("CreatePipe", 0);

			//-- Create a duplicate of the output write handle for the std error
			//	 write handle. This is necessary in case the child application
			//	 closes one of its std output handles.
			if (!DuplicateHandle(GetCurrentProcess(), hOutputWrite,
								 GetCurrentProcess(), &hErrorWrite, 0,
								 TRUE,DUPLICATE_SAME_ACCESS)
			)
				EXECUTION_ERROR("DuplicateHandle", 0);

			//-- Create the child input pipe.
			if (!CreatePipe(&hInputRead, &hInputWriteTmp, &sa, 0))
				EXECUTION_ERROR("CreatePipe", 0);

			//-- Create new output read handle and the input write handles. Set
			//	 the Properties to FALSE. Otherwise, the child inherits the
			//	 properties and, as a result, non-closeable handles to the pipes
			//	 are created.
			if (!DuplicateHandle(GetCurrentProcess(), hOutputReadTmp,
								 GetCurrentProcess(), &hOutputRead,		///< Address of new handle
								 0, FALSE, DUPLICATE_SAME_ACCESS)		///< Make it uninheritable
			)
				EXECUTION_ERROR("DuplicateHandle", 0);

			if (!DuplicateHandle(GetCurrentProcess(), hInputWriteTmp,
								 GetCurrentProcess(), &hInputWrite,		///< Address of new handle
								 0, FALSE, DUPLICATE_SAME_ACCESS)		///< Make it uninheritable
			)
				EXECUTION_ERROR("DuplicateHandle", 0);

			//-- Close inheritable copies of the handles you do not want to be inherited.
			if (!CloseHandle(hOutputReadTmp))	EXECUTION_ERROR("CloseHandle", 0);
			if (!CloseHandle(hInputWriteTmp))	EXECUTION_ERROR("CloseHandle", 0);

			//-- Launch child process
			hChildProcess = _LaunchChild(this, application, currentDirectory, 
				hOutputWrite, hInputRead, hErrorWrite);

			if (!hChildProcess)
				return 0;

			//-- Close pipe handles (do not continue to modify the parent).
			//	 You need to make sure that no handles to the write end of the
			//	 output pipe are maintained in this process or else the pipe will
			//	 not close when the child process exits and the ReadFile will hang.
			if (!CloseHandle(hOutputWrite))	EXECUTION_ERROR("CloseHandle", 0);
			if (!CloseHandle(hInputRead))	EXECUTION_ERROR("CloseHandle", 0);
			if (!CloseHandle(hErrorWrite))	EXECUTION_ERROR("CloseHandle", 0);

			if (single_thread)
				ReadFromChild();
			else
			{
				//-- Launch the thread that reads from the child's output
				DWORD ThreadId;
				HANDLE hThread = CreateThread(NULL, 0, _gReadFromChild, (LPVOID)this, 0, &ThreadId);
				if (hThread == NULL)
					EXECUTION_ERROR("CreateThread", 0);

				//-- Wait for thread to complete
				//if (WaitForSingleObject(hThread, INFINITE) == WAIT_FAILED)
				//	EXECUTION_ERROR("WaitForSingleObject", 0);
			}
			return pid = GetProcessId(hChildProcess);
		}
		else
			if ((hChildProcess = _LaunchChild(this, application, currentDirectory, 0, 0, 0)) == NULL)
				return 0;
			else
				return pid = GetProcessId(hChildProcess);
	}

	//-----------------------------------------------------------------------

	void ConsoleHost::ExecuteShell(const std::string& file, const std::string& parameters, const std::string& currentDirectory)
	{
		TCHAR *params = parameters.empty() ? NULL : strduplicate(parameters);
		TCHAR *dir = currentDirectory.empty() ? NULL : strduplicate(currentDirectory);
		if ((int) ShellExecute(NULL, NULL, util::std2str(file).c_str(), params, dir, SW_NORMAL) <= 32)	//error
			DisplayExecutionError("ExecuteShell");
		strfree(params);
		strfree(dir);
	}

	//-----------------------------------------------------------------------

	bool ConsoleHost::TerminateProcess(unsigned long pid)
	{
		HANDLE hProc;

		// If we can't open the process with PROCESS_TERMINATE rights,
		// then we give up immediately.
		if ((hProc = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, FALSE, (DWORD) pid)) == NULL)
			return false;

		// If we launched the process notify that this is a forced finish
		for (std::list<ConsoleHost*>::iterator i = hosts.begin(); i != hosts.end(); ++i)
			if ((*i)->pid == pid) {
				(*i)->forceFinish = true;
				break;
			}

		// Kill the process
		bool retval = ::TerminateProcess(hProc, EXIT_FAILURE) ? true : false;
		CloseHandle(hProc);
		return retval;
	}
	
	//-----------------------------------------------------------------------

	void ConsoleHost::SendToChild(const Buffer& buffer)
	{
		DWORD nBytesWrote;

		//-- send input to child through the pipe
		if (!WriteFile(hInputWrite, buffer.c_str(), (DWORD) buffer.size(), &nBytesWrote, NULL))
			if (GetLastError() != ERROR_NO_DATA)	///< if ERROR_NO_DATA pipe was closed (normal exit path)
				EXECUTION_ERROR("WriteFile", RETURN_EMPTY);
	}

	//-----------------------------------------------------------------------

	void ConsoleHost::ReadFromChild(void)
	{
		CHAR  lpBuffer[1024];
		DWORD nBytesRead;

		while (true)
		{
			if (!ReadFile(hOutputRead, lpBuffer, sizeof(lpBuffer) - 1, &nBytesRead, NULL) || !nBytesRead)
			{
				bool success = GetLastError() == ERROR_BROKEN_PIPE;
				if (onFinish)
					onFinish(!forceFinish);
				if (success)
					break;
				else
					EXECUTION_ERROR("ReadFile", RETURN_EMPTY);
			}
			lpBuffer[nBytesRead] = '\0';

			//-- handle redirected output
			assert(onOutput);
			onOutput(lpBuffer);
		}

		if (!CloseHandle(hOutputRead))	EXECUTION_ERROR("CloseHandle", RETURN_EMPTY);
		if (!CloseHandle(hInputWrite))	EXECUTION_ERROR("CloseHandle", RETURN_EMPTY);
	}

	//-----------------------------------------------------------------------

	bool ConsoleHost::Succeeded(void) const
	{
		assert(hChildProcess);
		DWORD result;
		if (GetExitCodeProcess(hChildProcess, &result))
			return result == EXIT_SUCCESS;
		else
			EXECUTION_ERROR("GetExitCodeProcess", false);
	}

	//-------------------------------------------------------//
	//---- static free functions ----------------------------//

	void ConsoleHost::DisplayExecutionError (char *pszAPI)
	{
		TCHAR message[256];
		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			0,
			message,
			sizeof(message) / sizeof(TCHAR),
			NULL
		);
		gui::displayMessage(NULL, _T("Execution Error"), util::std2str(pszAPI) + _T(": ") + message);
	}

	//-----------------------------------------------------------------------

	HANDLE _LaunchChild(ConsoleHost* host, const std::string& application, const std::string& currentDirectory,
		HANDLE hChildStdOut, HANDLE hChildStdIn, HANDLE hChildStdErr)
	{
		PROCESS_INFORMATION pi;
		STARTUPINFO si;

		//-- Set up the start up info struct.
		ZeroMemory(&si,sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		if (hChildStdOut && hChildStdIn && hChildStdErr)
			si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
		si.hStdOutput = hChildStdOut;
		si.hStdInput  = hChildStdIn;
		si.hStdError  = hChildStdErr;
		si.wShowWindow = SW_HIDE;	//-- wShowWindow flags can only be used when dwFlags include STARTF_USESHOWWINDOW

		TCHAR* command = strduplicate(application);
		TCHAR* directory = currentDirectory.empty() ? NULL : strduplicate(currentDirectory);

		//-- Launch the process that you want to redirect
		//	 Make sure child is in the same directory as host
		//	 launch redirect from a command line to prevent location
		//	 confusion.
		HANDLE hChildProcess = (HANDLE) 0;
		if (!CreateProcess(NULL, command, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, directory, &si, &pi))
			host->DisplayExecutionError("CreateProcess");
		else {
			//-- Set global child process handle to cause threads to exit.
			hChildProcess = pi.hProcess;

			//-- Close any unnecessary handles.
			if (!CloseHandle(pi.hThread))
				host->DisplayExecutionError("CloseHandle");
		}
		strfree(command);
		strfree(directory);
		return hChildProcess;
	}

	//-----------------------------------------------------------------------

	DWORD WINAPI _gReadFromChild(LPVOID lpvThreadParam)
	{
		ConsoleHost* host = (ConsoleHost*)lpvThreadParam;
		host->ReadFromChild();
		delete host;		///< thread stopped running

		return 1;
	}

	//-----------------------------------------------------------------------
	
	TCHAR* strduplicate(const std::string& str) {
		TCHAR* ret = new TCHAR[str.size() + 1];
		mbstowcs(ret, str.c_str(), str.size());
		ret[str.size()] = '\0';
		return ret;
	}
	
	//-----------------------------------------------------------------------

	void strfree(TCHAR* str) { if (str) delete[] str; }

	//-----------------------------------------------------------------------
}
