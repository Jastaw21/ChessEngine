#include "../../../include/EngineShared/UCIParsing/UciParser.h"

#include <iostream>
#include <string>

#include "EngineShared/UCIParsing/Tokeniser.h"

void UciParser::parse(const std::string& inString){
    currentToken = 0;
    auto tokens = Tokeniser(inString);

    while (currentToken < tokens.size()){


        currentToken++; // advance
    }
    
    return;
};

Token& UciParser::peek(){
    if (currentToken+1 < tokens.size()){
        return tokens.at(currentToken);
    }
    else{
        return Token{.type = EOF_TOKEN,.value = ""};
    }
}

void UciParser::parseMoveList(){
    
}
