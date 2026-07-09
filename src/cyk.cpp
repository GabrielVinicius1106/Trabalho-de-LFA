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

    // Criar Matriz Triangular
    std::vector<std::vector<std::set<Simbolo>>> tabela(n + 1, std::vector<std::set<Simbolo>>(n));

    // Inicializar a Matriz com Casos Base (substrings de tamanho 1)
    for (int i = 0; i < n; ++i) {
        for (const auto& regra : g.regras) {
            if (regra.dir.size() == 1 && regra.dir[0] == palavra[i]) {
                tabela[1][i].insert(regra.esq);
            }
        }
    }

    // n = Tamanho da Palavra

    // Combinar Substrings Menores em Maiores para Gerar os Conjuntos de Geradores
    for(int L = 2; L <= n; ++L){                              // L = Comprimento de 2..N
        for(int S = 0; S <= n - L; ++S){                      // S = Posição Inicial 0..(n - L)
            for(int P = 1; P < L; ++P){

                // Seleciona os Conjuntos em Roldana
                const auto& conjuntoEsq = tabela[P][S];
                const auto& conjuntoDir = tabela[L - P][S + P];

                for(const auto& regra : g.regras){
                    // Filtra Pelas Regras que produzem 2(DUAS) VARIÁVEIS
                    if(regra.dir.size() == 2){
                        // Busca por Produções
                        if(conjuntoEsq.find(regra.dir[0]) != conjuntoEsq.end() &&
                            conjuntoDir.find(regra.dir[1]) != conjuntoDir.end()){
                            tabela[L][S].insert(regra.esq);
                        }
                    }
                }
            }
        }
    }

    std::cout << "\n================ TABELA CYK ================\n";

    std::set<Simbolo> subpalavrasGeradas;

    std::cout << "Tamanho da Palavra: " << n << "\n";

    // Imprime a Matriz Triangular
    for(int L = n; L >= 1; --L){
        std::cout << "L=" << L << "\t| ";
        for(int S = 0; S <= n - L; ++S){
            std::cout << "{";
            if(tabela[L][S].empty()){
                std::cout << "-";
            } else {
                auto it = tabela[L][S].begin();
                std::cout << *it;

                // Define uma Subpalavra. Adicionar a Lista de Subpalavras
                if(tabela[L][S].find(g.simboloInicial) != tabela[L][S].end()){

                    std::string subpalavra = "";

                    for(int i = S; i < S + L; i++){
                        subpalavra += palavra[i];
                    }

                    subpalavrasGeradas.insert(subpalavra);


                }

                ++it;
                while(it != tabela[L][S].end()){
                    std::cout << ", " << *it;
                    ++it;

                    // Define uma Subpalavra. Adicionar a Lista de Subpalavras
                    if(tabela[L][S].find(g.simboloInicial) != tabela[L][S].end()){

                        std::string subpalavra = "";

                        for(int i = S; i < S + L; i++){
                            subpalavra += palavra[i];
                        }

                        subpalavrasGeradas.insert(subpalavra);
                    }
                }
            }
            std::cout << "}\t";
        }
        std::cout << "\n";
    }

    // Exibe a Cadeia de Caracteres
    std::cout << "\nPALAVRA: ";
    for(int i = 0; i < n; i++){
        std::cout << "{ " << palavra[i] << " } \t\t";
    }

    std::cout << "\n============================================\n";

    // Quantidade de Subpalavras
    std::cout << "Qntd de Subpalavras Geradas: " << subpalavrasGeradas.size() << std::endl;

    std::cout << "Subpalavras Geradas: \n";

    for(auto x : subpalavrasGeradas){
        std::cout << " - " << x << "\n";
    }

    std::cout << "\n";

    // Verificar se Símbolo Inicial está na RAIZ
    bool aceita = tabela[n][0].find(g.simboloInicial) != tabela[n][0].end();
    if (aceita) {
        std::cout << "\n[RESULTADO] A palavra PERTENCE a linguagem gerada pela gramatica.\n";
    } else {
        std::cout << "\n[RESULTADO] A palavra NAO PERTENCE a linguagem gerada pela gramatica.\n";
    }
}