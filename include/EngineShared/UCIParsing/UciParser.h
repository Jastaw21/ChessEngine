//
// Created by jacks on 15/07/2025.
//

#ifndef UCIPARSER_H
#define UCIPARSER_H
#include <string>
#include <vector>


class Token;


class UCIParser {
public:

    std::vector<Token> parse(const std::string& parseTarget){}

private:

    std::vector<Token> tokens;
    int currentToken;

    Token& peek();
    Token& consume();
};


#endif //UCIPARSER_H
