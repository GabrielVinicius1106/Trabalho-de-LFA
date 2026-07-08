#include "gramatica.hpp"
#include <algorithm>

using namespace std;

bool Gramatica::ehTerminal(const Simbolo& s) const {
    return find(terminais.begin(), terminais.end(), s) != terminais.end();
}

bool Gramatica::ehNaoTerminal(const Simbolo& s) const {
    return find(naoTerminais.begin(), naoTerminais.end(), s) != naoTerminais.end();
}
