#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <set>
#include <map>
#include <queue>
#include <deque>

#include "gramatica.hpp"

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

void Gramatica::converterParaChomsky(){
    
    std::map<std::string, std::string> mapaTerminais;
    std::vector<Regra> novasRegras;
    int contadorVariaveis = 1;

    for (auto& regra : regras) {
        if (regra.dir.size() >= 2) {
            for (size_t i = 0; i < regra.dir.size(); ++i) {
                if (ehTerminal(regra.dir[i])) {
                    
                    std::string terminal = regra.dir[i];
                    
                    if (mapaTerminais.find(terminal) == mapaTerminais.end()) {
                        std::string novaVariavel = "T_" + std::to_string(contadorVariaveis++);
                        mapaTerminais[terminal] = novaVariavel;
                        novasRegras.push_back({novaVariavel, {terminal}});
                    }

                    regra.dir[i] = mapaTerminais[terminal];
                }
            }
        }

        while (regra.dir.size() > 2) {
            
            std::string novaVariavel = "V_" + std::to_string(contadorVariaveis++);
            
            std::string ultimo = regra.dir.back();
            regra.dir.pop_back();
            
            std::string penultimo = regra.dir.back();
            regra.dir.pop_back();

            novasRegras.push_back({novaVariavel, {penultimo, ultimo}});

            regra.dir.push_back(novaVariavel);
        }
    }

    regras.insert(regras.end(), novasRegras.begin(), novasRegras.end());
}

void Gramatica::removerProducoesVazias() {

    // Variáveis que Geram a Palavra Vazia (Direta ou Indiretamente)
    std::unordered_set<Simbolo> anulaveis;
    
    bool mudou;
    
    // Mapear as Variáveis Anuláveis
    for(const auto& regra : regras){
        if(regra.dir.empty() || (regra.dir.size() == 1 && regra.dir[0] == SIMBOLO_VAZIO)){
            anulaveis.insert(regra.esq);
        }
    }
    
    do {
        mudou = false;
        
        for(const auto& regra : regras){
            if(anulaveis.find(regra.esq) == anulaveis.end()){
                
                bool todosAnulaveis = true;
                
                if(regra.dir.empty() || (regra.dir.size() == 1 && regra.dir[0] == SIMBOLO_VAZIO)){
                    continue;
                }
                
                for(const auto& s : regra.dir){
                    
                    if(anulaveis.find(s) == anulaveis.end()){
                        todosAnulaveis = false;
                        break;
                    }
                
                }
                
                if(todosAnulaveis){
                    anulaveis.insert(regra.esq);
                    mudou = true;
                }
            }
        }

    } while(mudou);

    std::vector<Regra> novasRegras;
    for(const auto& regra : regras){
        
        if(regra.dir.empty() || (regra.dir.size() == 1 && regra.dir[0] == SIMBOLO_VAZIO)){
            continue;
        }
        
        std::vector<int> indicesAnulaveis;
        for(size_t i = 0; i < regra.dir.size(); ++i){
            if(anulaveis.find(regra.dir[i]) != anulaveis.end()){
                indicesAnulaveis.push_back(i);
            }
        }
        
        int numCombinacoes = 1 << indicesAnulaveis.size();
        
        for(int mask = 0; mask < numCombinacoes; ++mask){
            Regra novaRegra;
            novaRegra.esq = regra.esq;
            
            for(size_t i = 0; i < regra.dir.size(); ++i){
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
                    novaRegra.dir.push_back(regra.dir[i]);
                }
            }
            
            if(!novaRegra.dir.empty()){
                bool existe = false;
                for(const auto& nr : novasRegras){
                    if(nr.esq == novaRegra.esq && nr.dir == novaRegra.dir){
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

    std::unordered_map<Simbolo, std::unordered_set<Simbolo>> paresUnitarios;
    
    for (const auto& nt : variaveis) {
        paresUnitarios[nt].insert(nt);
    }
    
    for (const auto& regra : regras) {
        if (regra.dir.size() == 1 && ehVariavel(regra.dir[0])) {
            paresUnitarios[regra.esq].insert(regra.dir[0]);
        }
    }
    
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
    
    std::vector<Regra> novasRegras;
    for (const auto& nt : variaveis) {
        for (const auto& b : paresUnitarios[nt]) {
            for (const auto& regra : regras) {
                if (regra.esq == b) {
                    bool ehUnitaria = (regra.dir.size() == 1 && ehVariavel(regra.dir[0]));
                    
                    if (!ehUnitaria) {
                        Regra novaRegra;
                        novaRegra.esq = nt;
                        novaRegra.dir = regra.dir;
                        
                        bool existe = false;
                        for (const auto& nr : novasRegras) {
                            if (nr.esq == novaRegra.esq && nr.dir == novaRegra.dir) {
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

    std::unordered_set<Simbolo> geradores;
    
    for(const auto& t : terminais) geradores.insert(t);
    
    geradores.insert(SIMBOLO_VAZIO);

    bool mudou;
    
    do {
        mudou = false;

        for(const auto& regra : regras){
            
            if(geradores.find(regra.esq) == geradores.end()){
                
                bool gera = true;
                for(const auto& s : regra.dir){
                    if(geradores.find(s) == geradores.end()){
                        gera = false;
                        break;
                    }
                }

                if(gera){
                    geradores.insert(regra.esq);
                    mudou = true;
                }
            }
        }

    } while(mudou);

    std::vector<Regra> regrasGeradoras;
    
    for(const auto& regra : regras){
        
        if(geradores.find(regra.esq) != geradores.end()){
            
            bool valida = true;
        
            for (const auto& s : regra.dir){
                if(geradores.find(s) == geradores.end()){
                    valida = false;
                    break;
                }
            }

            if(valida) regrasGeradoras.push_back(regra);
        }
    }

    regras = move(regrasGeradoras);

    std::unordered_set<Simbolo> alcancaveis;
    std::queue<Simbolo> fila;
    
    alcancaveis.insert(simboloInicial);
    fila.push(simboloInicial);

    while(!fila.empty()){
        
        Simbolo atual = fila.front();
        fila.pop();

        for(const auto& regra : regras){
            
            if(regra.esq == atual){
                
                for(const auto& s : regra.dir){
                    
                    if(alcancaveis.find(s) == alcancaveis.end()){
                        
                        alcancaveis.insert(s);
                        
                        if(ehVariavel(s)){
                            fila.push(s);
                        }
                    }
                }
            }
        }
    }

    std::vector<Regra> regrasFinais;
    
    for(const auto& regra : regras){
        if(alcancaveis.find(regra.esq) != alcancaveis.end()){
            regrasFinais.push_back(regra);
        }
    }

    regras = move(regrasFinais);

    std::vector<Simbolo> variaveisComRepeticao;

    for(auto &r : regras) variaveisComRepeticao.push_back(r.esq);

    std::set<Simbolo> variaveisSemRepeticao(variaveisComRepeticao.begin(), variaveisComRepeticao.end());

    std::vector<Simbolo> novasVariaveis;

    for(auto &v : variaveisSemRepeticao) novasVariaveis.push_back(v);

    variaveis = move(novasVariaveis);

} 