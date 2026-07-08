#ifndef GRAMATICA_HPP
#define GRAMATICA_HPP

#include <string>
#include <vector>

using Simbolo = std::string;

using Producao = std::vector<Simbolo>;

inline const std::string SIMBOLO_VAZIO = "&";

struct Regra {
    Simbolo  esq;   // Lado Esquerdo da Regra - Símbolo
    Producao dir;   // Lado Direito da Regra  - Produção
};

struct Gramatica {
    
    std::vector<Simbolo> terminais;
    std::vector<Simbolo> variaveis;
    
    Simbolo              simboloInicial;
    std::vector<Regra>   regras;

    bool ehTerminal(const Simbolo& s) const;
    bool ehVariavel(const Simbolo& s) const;

    void simplificar();

    void converterParaChomsky();

    private:
        void removerProducoesVazias();
        void removerProducoesUnitarias();
        void removerSimbolosInuteis();

};

#endif
