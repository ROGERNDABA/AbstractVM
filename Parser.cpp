/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Roger Ndaba <rogerndaba@gmil.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/06/18 13:37:41 by Roger Ndaba       #+#    #+#             */
/*   Updated: 2019/06/23 12:52:34 by Roger Ndaba      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

Lexer Parser::_lexer;
std::vector<const IOperand*> Parser::_stack = {};
bool Parser::_exit = false;

Parser::Parser() {
    _lexFuncs[DUMP] = &Parser::pDump;
    _lexFuncs[PUSH] = &Parser::pPush;
    _lexFuncs[POP] = &Parser::pPop;
    _lexFuncs[ASSERT] = &Parser::pAssert;
    _lexFuncs[ADD] = &Parser::pAdd;
    _lexFuncs[SUB] = &Parser::pSub;
    _lexFuncs[MUL] = &Parser::pMul;
    _lexFuncs[DIV] = &Parser::pDiv;
    _lexFuncs[MOD] = &Parser::pMod;
    _lexFuncs[COMMENT] = &Parser::pComment;
    _lexFuncs[PRINT] = &Parser::pPrint;
    _lexFuncs[EXIT] = &Parser::pExit;
    _lexFuncs[EXEC] = &Parser::pExecute;
    this->_lexers = _lexer.getLexers();
}

Parser::Parser(std::string expr, short int exprType, int line) {
    this->_lexer.lexExpression(expr, exprType, line);
    this->_lexers = _lexer.getLexers();
}

Parser::ParserException::ParserException(std::string exc) {
    this->_exc = "\033[31m" + exc + "\033[0m";
}

Parser::ParserException::ParserException(Parser::ParserException const& copy) {
    *this = copy;
}

const char* Parser::ParserException::what() const throw() {
    // std::string s = this->_exc;
    return this->_exc.c_str();
}

Parser::ParserException& Parser::ParserException::operator=(Parser::ParserException const& rhs) {
    if (this != &rhs) {
        this->_exc = rhs._exc;
    }
    return *this;
}

Parser::~Parser() {
    for (std::vector<const IOperand*>::iterator i = _stack.end(); i-- != _stack.begin();) {
        delete *i;
    }
}

Parser::Parser(Parser const& copy) {
    *this = copy;
};

Parser& Parser::operator=(Parser const& rhs) {
    if (this != &rhs) {
    }
    return *this;
}

void Parser::pDump() {
    for (std::vector<const IOperand*>::iterator i = _stack.end(); i-- != _stack.begin();) {
        std::cout << (*i)->toString() << std::endl;
    }
}
void Parser::pPush() {
    const IOperand* tmpIop = _factory.createOperand(_currLex.type, _currLex.value);
    if (tmpIop != nullptr)
        this->_stack.push_back(tmpIop);
    delete tmpIop;
}
void Parser::pPop() {
    if (_stack.size() < 1)
        throw Parser::ParserException("Can't pop on an empty stack");
}
void Parser::pAssert() {
}
void Parser::pAdd() {
    if (_stack.size() > 1) {
        const IOperand* op1 = *(_stack.begin());
        _stack.erase(_stack.begin());
        const IOperand* op2 = *(_stack.begin());
        _stack.erase(_stack.begin());
        const IOperand* tmpIop = *op1 + *op2;
        if (tmpIop != nullptr)
            this->_stack.push_back(tmpIop);
        delete op1;
        delete op2;
    } else {
        throw Parser::ParserException("Stack must have at least two items to add");
    }
}
void Parser::pSub() {
    if (_stack.size() > 1) {
        const IOperand* op1 = *(_stack.begin());
        _stack.erase(_stack.begin());
        const IOperand* op2 = *(_stack.begin());
        _stack.erase(_stack.begin());
        _stack.push_back(*op1 - *op2);
        delete op1;
        delete op2;
    } else {
        throw Parser::ParserException("Stack must have at least two items to sub");
    }
}
void Parser::pMul() {
    if (_stack.size() > 1) {
        const IOperand* op1 = *(_stack.begin());
        _stack.erase(_stack.begin());
        const IOperand* op2 = *(_stack.begin());
        _stack.erase(_stack.begin());
        _stack.push_back(*op1 * *op2);
        delete op1;
        delete op2;
    } else {
        throw Parser::ParserException("Stack must have at least two items to mul");
    }
}
void Parser::pDiv() {
    if (_stack.size() > 1) {
        const IOperand* op1 = *(_stack.begin());
        _stack.erase(_stack.begin());
        const IOperand* op2 = *(_stack.begin());
        _stack.erase(_stack.begin());
        _stack.push_back(*op1 / *op2);
        delete op1;
        delete op2;
    } else {
        throw Parser::ParserException("Stack must have at least two items to div");
    }
}
void Parser::pMod() {
    if (_stack.size() > 1) {
        const IOperand* op1 = *(_stack.begin());
        _stack.erase(_stack.begin());
        const IOperand* op2 = *(_stack.begin());
        _stack.erase(_stack.begin());
        _stack.push_back(*op1 % *op2);
        delete op1;
        delete op2;
    } else {
        throw Parser::ParserException("Stack must have at least two items to mod");
    }
}
void Parser::pComment() {
    std::cout << "---> ;" << _currLex.value << std::endl;
}
void Parser::pPrint() {
}
void Parser::pExit() {
    this->_exit = true;
}
void Parser::pExecute() {
    this->eval();
}

bool Parser::getExit() {
    return this->_exit;
}

void Parser::eval() {
    try {
        this->_lexers = _lexer.getLexers();
        for (unsigned long i = 0; i < _lexers.size(); i++) {
            lexFunctions::iterator it;
            it = this->_lexFuncs.find(_lexers[i].lexE);
            if (it != _lexFuncs.end()) {
                _currLex = _lexers[i];
                (this->*it->second)();
            }
        }
        this->_lexer.clearLexers();
    } catch (Parser::ParserException& e) {
        std::cerr << e.what() << '\n';
        this->_lexer.clearLexers();
    }
}