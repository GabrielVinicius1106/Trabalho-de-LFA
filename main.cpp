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
        
        
        vector<int> seq;
        
        string message = "\n";
        
        int opt = 0;
        
        do {
            
            system("clear");

            cout << endl;
                        
            if(seq.size() == 0){
                cout << "\n <--- GRAMATICA ATUAL ---> \n" << endl;
            } else if(seq[seq.size() - 1] == 1) {
                cout << "\n <--- GRAMATICA SIMPLIFICADA ---> \n" << endl;
            } else if(seq[seq.size() - 1] == 2) {
                cout << "\n <--- GRAMATICA NA FORMA NORMAL DE CHOMSKY ---> \n" << endl;
            }

            imprimirGramatica(g);

            cout << "\n<--- OPÇÕES --->\n" << endl;

            cout << "1 - SIMPLIFICAR GRAMÁTICA" << endl;
            cout << "2 - CONVERTER PARA CHOMSKY" << endl;
            cout << "3 - TESTAR PALAVRA" << endl;
            cout << endl;
            cout << "0 - SAIR" << endl;
            
            cout << message << endl;

            cout << "\nInsira uma Opção >> ";
            cin >> opt;

            switch(opt){

                case 1:
                    /* Simplificar Gramática */
                
                    if(seq.size() != 0){
                        message = "\nGramática já está Simplificada.";
                        break;
                    }

                    message = "\n";

                    g.simplificar();
                    seq.push_back(1);

                break;

                case 2:
                    /* Converter para Chomsky */

                    if(seq.size() != 0 && seq[seq.size() - 1] == 2){
                        message = "\nA Gramática já está na Forma Normal de Chomsky.";
                        break;
                    }

                    if(seq.size() == 0 || seq[seq.size() - 1] != 1){
                        message = "\nImpossível converter para Chomsky. Simplifique a Gramática Primeiro.";
                        break;
                    }

                    message = "\n";

                    g.converterParaChomsky();
                    seq.push_back(2);

                break;

                case 3: {
                    /* Testar Palavra com CYK */

                    if(seq.size() == 0 || seq[seq.size() - 1] != 2){
                        message = "\nImpossível Testar Palavra. Converta para Chomsky Primeiro.";
                        break;
                    }

                    message = "\n";

                    cout << "\nInsira o Tamanho da Palavra: ";
                    int n; cin >> n;

                    vector<Simbolo> simbols(n);

                    cout << "\nInsira a Palavra (com espaçamento entre simbolos): ";
                    
                    for(int i = 0; i < n; i++){
                        cin >> simbols[i];
                    }

                    executarCYK(g, simbols);

                    cout << "\nPressione 1 para CONTINUAR...";
                    int a; cin >> a;
                }
                break;

                case 0:
                    /* SAIR */
                    return 0;
                break;
            
                default:
                    message = "\nOpção Inválida.";
                break;
            }


        } while(opt != 0);
    
    } catch (const ErroLeituraGLC& e){
        
        cerr << "Erro ao Ler Arquivo .glc: " << e.what() << "\n";
        
        return 1;
    }

    return 0;
}
