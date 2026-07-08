#ifndef GRAMATICA_HPP
#define GRAMATICA_HPP

#include <string>
#include <vector>

using namespace std;

using Simbolo = string;

using Producao = vector<Simbolo>;

inline const string SIMBOLO_VAZIO = "&";

struct Regra {
    Simbolo  ler;   // Lado Esquerdo da Regra - Símbolo
    Producao ldr;   // Lado Direito da Regra  - Produção
};

struct Gramatica {
    
    vector<Simbolo> terminais;
    vector<Simbolo> naoTerminais;
    
    Simbolo         simboloInicial;
    vector<Regra>   regras;

    bool ehTerminal(const Simbolo& s) const;
    bool ehNaoTerminal(const Simbolo& s) const;
};

#endif
