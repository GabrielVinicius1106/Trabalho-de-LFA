#include <algorithm>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <queue>

#include "gramatica.hpp"

using namespace std;

bool Gramatica::ehTerminal(const Simbolo& s) const {
    return find(terminais.begin(), terminais.end(), s) != terminais.end();
}

bool Gramatica::ehVariavel(const Simbolo& s) const {
    return find(variaveis.begin(), variaveis.end(), s) != variaveis.end();
}

void Gramatica::simplificar() {
    removerProducoesVazias();
    removerProducoesUnitarias();
    removerSimbolosInuteis();
}

void Gramatica::removerProducoesVazias() {

    unordered_set<Simbolo> anulaveis;
    bool mudou;
    
    for(const auto& regra : regras){
        if(regra.ldr.empty() || (regra.ldr.size() == 1 && regra.ldr[0] == SIMBOLO_VAZIO)){
            anulaveis.insert(regra.ler);
        }
    }
    
    do {
        mudou = false;
        for(const auto& regra : regras){
            if(anulaveis.find(regra.ler) == anulaveis.end()){
                
                bool todosAnulaveis = true;
                
                if(regra.ldr.empty() || (regra.ldr.size() == 1 && regra.ldr[0] == SIMBOLO_VAZIO)){
                    continue;
                }
                
                for(const auto& s : regra.ldr){
                    
                    if(anulaveis.find(s) == anulaveis.end()){
                        todosAnulaveis = false;
                        break;
                    }
                
                }
                
                if(todosAnulaveis){
                    anulaveis.insert(regra.ler);
                    mudou = true;
                }
            }
        }

    } while(mudou);

    vector<Regra> novasRegras;
    for(const auto& regra : regras){
        
        if(regra.ldr.empty() || (regra.ldr.size() == 1 && regra.ldr[0] == SIMBOLO_VAZIO)){
            continue; // Remove a produção ε original
        }
        
        vector<int> indicesAnulaveis;
        for(size_t i = 0; i < regra.ldr.size(); ++i){
            if(anulaveis.find(regra.ldr[i]) != anulaveis.end()){
                indicesAnulaveis.push_back(i);
            }
        }
        
        // Gera todas as combinações (2^n) de presença/ausência de símbolos anuláveis
        int numCombinacoes = 1 << indicesAnulaveis.size();
        
        for(int mask = 0; mask < numCombinacoes; ++mask){
            Regra novaRegra;
            novaRegra.ler = regra.ler;
            
            for(size_t i = 0; i < regra.ldr.size(); ++i){
                bool pular = false;
                for(size_t j = 0; j < indicesAnulaveis.size(); ++j){
                    if(i == (size_t)indicesAnulaveis[j]){
                        if((mask & (1 << j)) != 0){
                            pular = true;
                        }
                        break;
                    }
                }

                if(!pular){
                    novaRegra.ldr.push_back(regra.ldr[i]);
                }
            }
            
            // Adiciona a regra gerada, se não for vazia e não for repetida
            if(!novaRegra.ldr.empty()){
                bool existe = false;
                for(const auto& nr : novasRegras){
                    if(nr.ler == novaRegra.ler && nr.ldr == novaRegra.ldr){
                        existe = true;
                        break;
                    }
                }

                if(!existe){
                    novasRegras.push_back(novaRegra);
                }
            }
        }
    }
    
    regras = move(novasRegras);
}

void Gramatica::removerProducoesUnitarias() {
    // Implementação da substituição de cadeias de derivação A -> B
    unordered_map<Simbolo, unordered_set<Simbolo>> paresUnitarios;
    
    // Passo 1: Inicializar pares (A, A) para toda variável
    for (const auto& nt : variaveis) {
        paresUnitarios[nt].insert(nt);
    }
    
    // Extrair produções unitárias originais
    for (const auto& regra : regras) {
        if (regra.ldr.size() == 1 && ehVariavel(regra.ldr[0])) {
            paresUnitarios[regra.ler].insert(regra.ldr[0]);
        }
    }
    
    // Passo 2: Calcular o fecho transitivo (se A->B e B->C, então A->C)
    bool mudou;
    do {
        mudou = false;
        for (const auto& nt1 : variaveis) {
            auto alcancaveis = paresUnitarios[nt1];
            for (const auto& nt2 : alcancaveis) {
                for (const auto& nt3 : paresUnitarios[nt2]) {
                    if (paresUnitarios[nt1].find(nt3) == paresUnitarios[nt1].end()) {
                        paresUnitarios[nt1].insert(nt3);
                        mudou = true;
                    }
                }
            }
        }
    } while (mudou);
    
    // Passo 3: Criar novas regras ignorando as unitárias originais
    vector<Regra> novasRegras;
    for (const auto& nt : variaveis) {
        for (const auto& b : paresUnitarios[nt]) {
            for (const auto& regra : regras) {
                if (regra.ler == b) {
                    bool ehUnitaria = (regra.ldr.size() == 1 && ehVariavel(regra.ldr[0]));
                    
                    if (!ehUnitaria) {
                        Regra novaRegra;
                        novaRegra.ler = nt; // A regra original passa a apontar diretamente para a produção
                        novaRegra.ldr = regra.ldr;
                        
                        // Verifica duplicações
                        bool existe = false;
                        for (const auto& nr : novasRegras) {
                            if (nr.ler == novaRegra.ler && nr.ldr == novaRegra.ldr) {
                                existe = true;
                                break;
                            }
                        }
                        if (!existe) {
                            novasRegras.push_back(novaRegra);
                        }
                    }
                }
            }
        }
    }
    
    regras = move(novasRegras);
}

void Gramatica::removerSimbolosInuteis() {

    unordered_set<Simbolo> geradores;
    
    for(const auto& t : terminais) geradores.insert(t);
    
    geradores.insert(SIMBOLO_VAZIO);

    bool mudou;
    
    do {
        mudou = false;

        for(const auto& regra : regras){
            
            // Se o LER ainda não é considerado gerador
            if(geradores.find(regra.ler) == geradores.end()){
                
                bool gera = true;
                for(const auto& s : regra.ldr){
                    if(geradores.find(s) == geradores.end()){
                        gera = false; // Se um símbolo do LDR não é gerador, a regra falha
                        break;
                    }
                }

                if(gera){
                    geradores.insert(regra.ler);
                    mudou = true; // Novo símbolo gerador encontrado, iterar novamente
                }
            }
        }

    } while(mudou);

    // Filtrar regras mantendo apenas as construídas por símbolos geradores
    vector<Regra> regrasGeradoras;
    
    for(const auto& regra : regras){
        
        if(geradores.find(regra.ler) != geradores.end()){
            
            bool valida = true;
        
            for (const auto& s : regra.ldr){
                if(geradores.find(s) == geradores.end()){
                    valida = false;
                    break;
                }
            }

            if(valida) regrasGeradoras.push_back(regra);
        }
    }

    regras = move(regrasGeradoras);

    unordered_set<Simbolo> alcancaveis;
    queue<Simbolo> fila;
    
    alcancaveis.insert(simboloInicial);
    fila.push(simboloInicial);

    while(!fila.empty()){
        
        Simbolo atual = fila.front();
        fila.pop();

        for(const auto& regra : regras){
            
            if(regra.ler == atual){
                
                for(const auto& s : regra.ldr){
                    
                    if(alcancaveis.find(s) == alcancaveis.end()){
                        
                        alcancaveis.insert(s); // Marca como alcançável
                        
                        if(ehVariavel(s)){
                            fila.push(s); // Adiciona para expandir apenas se for variável
                        }
                    }
                }
            }
        }
    }

    // Filtrar regras mantendo apenas símbolos alcançáveis
    vector<Regra> regrasFinais;
    
    for(const auto& regra : regras){
        if(alcancaveis.find(regra.ler) != alcancaveis.end()){
            regrasFinais.push_back(regra);
        }
    }

    regras = move(regrasFinais);

    vector<Simbolo> variaveisComRepeticao;

    for(auto &r : regras) variaveisComRepeticao.push_back(r.ler);

    set<Simbolo> variaveisSemRepeticao(variaveisComRepeticao.begin(), variaveisComRepeticao.end());

    vector<Simbolo> novasVariaveis;

    for(auto &v : variaveisSemRepeticao) novasVariaveis.push_back(v);

    variaveis = move(novasVariaveis);

} 