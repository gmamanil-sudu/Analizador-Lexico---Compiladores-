#include "Token.h"

string formatearToken(const Token& token) {
    if (token.tipo == "ID") {
        return "<ID," + token.atributo + ">";
    }

    return "<" + token.tipo + ">";
}