cd ./scripts/bin
del *.dbc *.dbi
cd ../../config_scripts
del *.dbc *.dbi
cd ../extension_scripts
del *.dbc *.dbi
cd ..

DeltaCompiler --debug --text --funcfile="./scripts/SparrowLibFuncs.txt" --output_path "./scripts/bin/" --symbolic_names "main.dsc" "./scripts/sparrowlib/main.dsc"
DeltaCompiler --debug --text --funcfile="./scripts/SparrowLibFuncs.txt" --output_path "./scripts/bin/" --symbolic_names "sparrowlib.dsc" "./scripts/sparrowlib/sparrowlib.dsc"

pause
