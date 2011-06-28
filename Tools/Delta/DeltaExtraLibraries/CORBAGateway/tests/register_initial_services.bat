call "%VS80COMNTOOLS%\vsvars32.bat"
"%TAO_ROOT%\bin\tao_ifr.exe" -ORBInitRef InterfaceRepository=file://if_repo.ior orb.idl
"%TAO_ROOT%\bin\tao_ifr.exe" -ORBInitRef InterfaceRepository=file://if_repo.ior "%TAO_ROOT%\idl\CosNaming.idl"