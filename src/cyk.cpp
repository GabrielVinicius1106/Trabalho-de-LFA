#include "gramatica.hpp"
#include <iostream>
#include <vector>
#include <set>

void executarCYK(const Gramatica &g, const std::vector<Simbolo> &palavra) {
    
    int n = palavra.size();
    
    // Tratamento Base para Palavra Vazia
    if (n == 0) {
        bool aceitaVazio = false;
        for (const auto& regra : g.regras) {
            if (regra.esq == g.simboloInicial && (regra.dir.empty() || regra.dir[0] == SIMBOLO_VAZIO)) {
                aceitaVazio = true;
                break;
            }
        }
        if (aceitaVazio) std::cout << "A palavra (vazia) PERTENCE a linguagem.\n";
        else std::cout << "A palavra (vazia) NAO PERTENCE a linguagem.\n";
        return;
    }

    std::vector<std::vector<std::set<Simbolo>>> tabela(n + 1, std::vector<std::set<Simbolo>>(n));

    for (int i = 0; i < n; ++i) {
        for (const auto& regra : g.regras) {
            if (regra.dir.size() == 1 && regra.dir[0] == palavra[i]) {
                tabela[1][i].insert(regra.esq);
            }
        }
    }

    for (int L = 2; L <= n; ++L) {
        for (int S = 0; S <= n - L; ++S) {
            for (int P = 1; P < L; ++P) {
                const auto& conjuntoEsq = tabela[P][S];
                const auto& conjuntoDir = tabela[L - P][S + P];

                for (const auto& regra : g.regras) {
                    if (regra.dir.size() == 2) {
                        if (conjuntoEsq.find(regra.dir[0]) != conjuntoEsq.end() &&
                            conjuntoDir.find(regra.dir[1]) != conjuntoDir.end()) {
                            tabela[L][S].insert(regra.esq);
                        }
                    }
                }
            }
        }
    }

    std::cout << "\n================ TABELA CYK ================\n";
    
    for (int L = n; L >= 1; --L) {
        std::cout << "L=" << L << "\t| ";
        for (int S = 0; S <= n - L; ++S) {
            std::cout << "{";
            if (tabela[L][S].empty()) {
                std::cout << "-";
            } else {
                auto it = tabela[L][S].begin();
                std::cout << *it;
                ++it;
                while (it != tabela[L][S].end()) {
                    std::cout << ", " << *it;
                    ++it;
                }
            }
            std::cout << "}\t";
        }
        std::cout << "\n";
    }
    std::cout << "============================================\n";

    bool aceita = tabela[n][0].find(g.simboloInicial) != tabela[n][0].end();
    if (aceita) {
        std::cout << "\n[RESULTADO] A palavra PERTENCE a linguagem gerada pela gramatica.\n";
    } else {
        std::cout << "\n[RESULTADO] A palavra NAO PERTENCE a linguagem gerada pela gramatica.\n";
    }
}