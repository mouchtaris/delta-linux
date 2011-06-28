del "Src\DecisionMakerScanner.cpp"
del "Src\DecisionMakerBisonParser.cpp"
del "Include\DecisionMakerBisonParser.h"

bison -d "Src/DecisionMakerParser.y"
flex "Src/DecisionMakerScanner.l"

move "Src\DecisionMakerBisonParser.hpp" "Include\DecisionMakerBisonParser.h"