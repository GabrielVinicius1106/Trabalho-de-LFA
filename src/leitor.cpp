#include "leitor.hpp"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>

namespace {

    string aparar(const string& linha){

        size_t inicio = linha.find_first_not_of(" \t\r\n");
        
        if (inicio == string::npos) return "";
        
        size_t fim = linha.find_last_not_of(" \t\r\n");
        
        return linha.substr(inicio, fim - inicio + 1);
    }

    string lerLinha(ifstream& arquivo, const string& contexto){
        
        string linha;
        
        if(!getline(arquivo, linha)){
            throw ErroLeituraGLC("Fim de arquivo inesperado ao ler: " + contexto);
        }

        return aparar(linha);
    }

    int lerInteiroPositivo(ifstream& arquivo, const string& contexto){
        
        string linha = lerLinha(arquivo, contexto);
        
        try {

            size_t pos;
            int valor = stoi(linha, &pos);
            
            if(pos != linha.size() || valor <= 0){
                throw invalid_argument("valor não positivo ou lixo na linha");
            }

            return valor;

        } catch (const exception&){
            throw ErroLeituraGLC("Esperava um inteiro positivo em '" + contexto + "', encontrei: \"" + linha + "\"");
        }
    }

    vector<Simbolo> lerListaDeSimbolos(ifstream& arquivo, int quantidade, const string& contexto){

        vector<Simbolo> simbolos;
        simbolos.reserve(quantidade);

        for(int i = 0; i < quantidade; ++i){

            string simbolo = lerLinha(arquivo, contexto + " (" + to_string(i + 1) + "/" + to_string(quantidade) + ")");
            
            if(simbolo.empty()){
                throw ErroLeituraGLC("Símbolo vazio encontrado em: " + contexto);
            }

            simbolos.push_back(simbolo);
        }

        return simbolos;
    }

    Regra lerRegra(const string& linhaOriginal, const string& contexto){
        
        string linha = aparar(linhaOriginal);
        
        if(linha.empty()){
            throw ErroLeituraGLC("Linha de regra vazia em: " + contexto);
        }

        istringstream stream(linha);
        Regra regra;
        stream >> regra.esq;

        vector<Simbolo> ldr;
        Simbolo token;

        while(stream >> token){
            ldr.push_back(token);
        }

        if(ldr.size() == 1 && ldr[0] == SIMBOLO_VAZIO){
            ldr.clear();
        }

        regra.dir = move(ldr);
        return regra;
    }

    void validarSimbolosDistintos(const vector<Simbolo>& terminais, const vector<Simbolo>& variaveis){

        unordered_set<Simbolo> vistos;

        for(const auto& t : terminais){
            
            if(!vistos.insert(t).second){
                throw ErroLeituraGLC("Símbolo terminal repetido: \"" + t + "\"");
            }

        }

        for(const auto& n : variaveis){
            
            if(vistos.count(n)){
                throw ErroLeituraGLC("Símbolo \"" + n + "\" aparece como terminal e não-terminal");
            }
            
            if(!vistos.insert(n).second){
                throw ErroLeituraGLC("Símbolo não-terminal repetido: \"" + n + "\"");
            }
        }
    }

    void validarSimboloInicial(const Gramatica& g){
        
        if(!g.ehVariavel(g.simboloInicial)){
            throw ErroLeituraGLC("Símbolo inicial \"" + g.simboloInicial + "\" não está entre os não-terminais declarados");
        }
    }

    void validarRegras(const Gramatica& g){

        for(const auto& regra : g.regras){
            
            if(!g.ehVariavel(regra.esq)){
                throw ErroLeituraGLC("LER da regra não é um não-terminal declarado: \"" + regra.esq + "\"");
            }

            for(const auto& simbolo : regra.dir){
                if(!g.ehTerminal(simbolo) && !g.ehVariavel(simbolo)){
                    throw ErroLeituraGLC("Símbolo \"" + simbolo +
                                        "\" usado na regra de \"" + regra.esq +
                                        "\" não foi declarado como terminal nem não-terminal");
                }
            }
        }
    }

}

std::vector<Simbolo> lerPalavra(){

    int n;

    std::cout << "\n Insira o Tamanho da Palavra: ";
    std::cin >> n;

    std::vector<Simbolo> s;

    std::cout << "\n Insira a Palavra: ";
    for(int i = 0; i < n; i++){
        std::cin >> s[i];
    }
    
    return s;
}

Gramatica lerArquivoGLC(const string& caminhoArquivo){
    
    ifstream arquivo(caminhoArquivo);
    
    if(!arquivo.is_open()){
        throw ErroLeituraGLC("Não foi possível abrir o arquivo: " + caminhoArquivo);
    }

    Gramatica g;

    int t = lerInteiroPositivo(arquivo, "quantidade de terminais (t)");
    g.terminais = lerListaDeSimbolos(arquivo, t, "símbolo terminal");

    int n = lerInteiroPositivo(arquivo, "quantidade de não-terminais (n)");
    g.variaveis = lerListaDeSimbolos(arquivo, n, "símbolo não-terminal");

    validarSimbolosDistintos(g.terminais, g.variaveis);

    g.simboloInicial = lerLinha(arquivo, "símbolo inicial");
    validarSimboloInicial(g);

    int r = lerInteiroPositivo(arquivo, "quantidade de regras (r)");

    g.regras.reserve(r);

    for(int i = 0; i < r; ++i){
        
        string linha;
        
        if(!getline(arquivo, linha)){
            throw ErroLeituraGLC("Fim de arquivo inesperado ao ler regra " + to_string(i + 1) + "/" + to_string(r));
        }

        g.regras.push_back(lerRegra(linha, "regra " + to_string(i + 1) + "/" + to_string(r)));
    }

    validarRegras(g);

    return g;
}
