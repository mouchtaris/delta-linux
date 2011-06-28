del "Src\ProfileScanner.cpp"
del "Src\ProfileBisonParser.cpp"
del "Include\ProfileBisonParser.h"

bison -d "Src/ProfileParser.y"
flex "Src/ProfileScanner.l"

move "Src\ProfileBisonParser.hpp" "Include\ProfileBisonParser.h"