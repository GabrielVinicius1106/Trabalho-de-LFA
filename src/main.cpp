#include "gramatica.hpp"
#include "leitor.hpp"

#include <iostream>

using namespace std;

// Imprime a gramática de forma legível, só para conferência manual
// durante o desenvolvimento (vai ser útil também mais pra frente,
// pra comparar a gramática ORIGINAL com a versão em CNF).

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

        cout << "  " << regra.ler << " -> ";
        
        if(regra.ldr.empty()){
            cout << SIMBOLO_VAZIO << " (epsilon)";
        } else {
            for(const auto& simbolo : regra.ldr) cout << simbolo << " ";
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
        
        imprimirGramatica(g);

        g.simplificar();

        imprimirGramatica(g);
    
    } catch (const ErroLeituraGLC& e){
        
        cerr << "Erro ao Ler Arquivo .glc: " << e.what() << "\n";
        
        return 1;
    }

    return 0;
}
