rd /Q /S debug;
rd /Q /S release;
del /Q /S DMSL.ncb;
del /Q /S /AH DMSL.suo

rd /Q /S "DecisionMaker\Debug";
rd /Q /S "DecisionMaker\Release";
del /Q /S "DecisionMaker\*.user";

rd /Q /S "DMSL\Debug";
rd /Q /S "DMSL\Release";
del /Q /S "DMSL\*.user";

rd /Q /S "Profile\Debug";
rd /Q /S "Profile\Release";
del /Q /S "Profile\*.user";

rd /Q /S "Tester\Debug";
rd /Q /S "Tester\Release";
del /Q /S "Tester\*.user";

rd /Q /S "Utility\Debug";
rd /Q /S "Utility\Release";
del /Q /S "Utility\*.user";