DeltaCORBA is a library that provides a CORBA gateway within the Delta language.

Delta is a dynamically typed language and the CORBA gateway is implemented
using its dynamic features, namely the Dynamic Invocation Interface (DII)
and the Dynamic Skeleton Interface (DSI).

DeltaCORBA is implemented in C++ and consists of 3 dynamically linked libraries
that can be loaded in any Delta program to provide a natural CORBA gateway.

The native C++ bindings are based on the TAO corba distribution
(http://www.cs.wustl.edu/~schmidt/TAO.html)

*** Installation Instructions ***
	- The only supported platform for now is Microsoft Windows (XP, Vista, 7)
	- Download and install TAO 
	- After the installation there must be 2 environmental variables
	  ACE_ROOT and TAO_ROOT pointing to the corresponding installation dirs.
	- Build DeltaCorba project from the sources and project files provided.

There are 3 Microsoft Visual studio projects provided:
For Visual Studio 2005
	- DeltaCorbaCommon/DeltaCorbaCommon.vcproj
	- DeltaCorbaClient/DeltaCorbaClient.vcproj
	- DeltaCorbaServer/DeltaCorbaServer.vcproj
For Visual Studio 2010
	- DeltaCorbaCommon/DeltaCorbaCommon.vcxproj
	- DeltaCorbaClient/DeltaCorbaClient.vcxproj
	- DeltaCorbaServer/DeltaCorbaServer.vcxproj

When built, each of these projects will create a dll containing the 
corresponding functionality.
After building the dlls run the copydlls.bat to copy the dll files in the
tests/bin folder so that they are present for the testfiles.
(All testfiles assume the DeltaCORBA dlls are located in that folder).
Finally, note that the TAO dlls have to be available in your path for the
DeltaCORBA dlls to load properly.

*** Testing ***
To test DeltaCORBA is successfully build, there are some tests included.
Each test consists of 5 files.
	- An idl specification file.
	- A Delta source file for the corba server.
	- A Delta source file for the corba client.
	- A Sparrow IDE project file to build the client and server applications.
	- A batch file to register the idl file to the CORBA Interface Repository.

Before running any test, one has first to start some basic CORBA services.
	- Run tests/naming_service.bat to start the CORBA Naming Service,
	- Run tests/ifr_service.bat to start the CORBA Interface Repository (IFR).
	- Optionally run the tests/register_initial_services.bat to register some
	  basic CORBA interfaces to the IFR. Without this step you will not be able
	  to directly use the NamingServise or InterfaceRepository within Delta.

After initializing the basic CORBA services, one may run a test provided:
	- First run the batch file to register the idl file to the IFR.
	- Run the server application using Sparrow IDE.
	- Run the client application using Sparrow IDE.

All batch files assume that the required corba executables (along with the dlls they need)
are located within %TAO_ROOT%\bin.

*** Contact Information ***
For any information, comments, bug reports, etc. about DeltaCORBA contact:
lilis@ics.forth.gr