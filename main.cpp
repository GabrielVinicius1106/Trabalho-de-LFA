#include "src/gramatica.hpp"
#include "src/leitor.hpp"
#include "src/cyk.hpp"

#include <iostream>

using namespace std;

// Imprime a Gramática
void imprimirGramatica(const Gramatica& g){
    
    cout << "Terminais (" << g.terminais.size() << "): ";
    for(const auto& t : g.terminais) cout << "\'" << t << "\' ";
    cout << "\n\n";

    cout << "Variaveis (" << g.variaveis.size() << "): ";
    for(const auto& n : g.variaveis) cout << "\'" << n << "\' ";
    cout << "\n\n";

    cout << "Simbolo Inicial: " << g.simboloInicial << "\n\n";

    cout << "Regras (" << g.regras.size() << "):\n";
    for(const auto& regra : g.regras){

        cout << "  " << regra.esq << " -> ";
        
        if(regra.dir.empty()){
            cout << SIMBOLO_VAZIO << " (epsilon)";
        } else {
            for(const auto& simbolo : regra.dir) cout << simbolo << " ";
        }
        
        cout << "\n";
    }

    cout << "\n======================================================== \n" << endl;
}

int main() {

    string caminho = "data/exemplo.glc";

    try {
        
        Gramatica g = lerArquivoGLC(caminho);
        
        cout << "Gramatica Lida com Sucesso de: " << caminho << "\n\n";
        
        cout << "GRAMATICA ORIGINAL \n\n";

        imprimirGramatica(g);

        g.simplificar();
        
        cout << "GRAMATICA SIMPLIFICADA \n\n";

        imprimirGramatica(g);

        g.converterParaChomsky();
        
        cout << "GRAMATICA NA FORMA NORMAL DE CHOMSKY \n\n";

        imprimirGramatica(g);

        cout << " Insira o Tamanho da Palavra: ";
        int n; cin >> n;

        std:vector<Simbolo> simbols(n);

        cout << " Insira a Palavra (com espaçamento entre simbolos): ";
        for(int i = 0; i < n; i++){
            cin >> simbols[i];
        }

        executarCYK(g, simbols);
    
    } catch (const ErroLeituraGLC& e){
        
        cerr << "Erro ao Ler Arquivo .glc: " << e.what() << "\n";
        
        return 1;
    }

    return 0;
}
