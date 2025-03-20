#include "../include/Leopard.hpp"
#include <cctype>
#include <cstdio>
#include <iostream>

static int gettok() {
    static int lastChar = ' ';

    while(std::isspace(lastChar))
        lastChar = std::getchar();

    if(std::isalpha(lastChar))
    {
        IdentifierStr = lastChar;
        while(std::isalnum(lastChar = std::getchar()))
            IdentifierStr += lastChar;
        if(IdentifierStr == "def")
            return tok_def;
        if(IdentifierStr == "extern")
            return tok_extern;
        return tok_identifier;
    }

    if(std::isdigit(lastChar) || lastChar == '.')
    {
        std::string NumStr;
        int dot_found = 0;
        do{
            NumStr += lastChar;
            lastChar = std::getchar();
            if(lastChar == '.')
                dot_found++;
        }while(std::isdigit(lastChar) || lastChar == '.');
        if(dot_found > 1)
        {
            std::cerr << "Invalid number: " << NumStr << std::endl;
            exit(1);
        }
        NumVal = std::strtod(NumStr.c_str(),0);
        return tok_number;
    }

    if(lastChar == '#')
    {
        do
            lastChar = std::getchar();
        while(lastChar != '\n' && lastChar != '\r' && lastChar != EOF);
        if(lastChar != EOF)
            return gettok();
    }

    if(lastChar == EOF)
        return tok_eof;
    
    int thisChar = lastChar;
    lastChar = std::getchar();
    return thisChar;
}

int main() {
    int tok;
    do{
        tok = gettok();
        if(tok == tok_identifier)
            std::cout << "Identifier: " << IdentifierStr << std::endl;
        else if(tok == tok_extern)
            std::cout << "Extern: " << IdentifierStr << std::endl;
        else if(tok == tok_def)
            std::cout << "Def: " << IdentifierStr << std::endl;
        else if(tok == tok_number)
            std::cout << "Number: " << NumVal << std::endl;
        else if(tok == tok_eof)
            std::cout << "End of file" << std::endl;
        else
            std::cout << "Unknown token: " << tok << std::endl;
    }while(tok != tok_eof);
}
