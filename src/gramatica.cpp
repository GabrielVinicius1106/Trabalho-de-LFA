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

// Converte para Chomsky
void Gramatica::converterParaChomsky(){
    
    std::map<std::string, std::string> mapaTerminais;
    std::vector<Regra> novasRegras;
    int contadorVariaveis = 1;

    for (auto& regra : regras) {

        // Criar Produções Unitárias de Terminais
        if (regra.dir.size() >= 2) {
            for (size_t i = 0; i < regra.dir.size(); ++i) {
                if (ehTerminal(regra.dir[i])) {
                    
                    std::string terminal = regra.dir[i];
                    
                    if(mapaTerminais.find(terminal) == mapaTerminais.end()){
                        std::string novaVariavel = "T_" + std::to_string(contadorVariaveis++);
                        mapaTerminais[terminal] = novaVariavel;
                        novasRegras.push_back({novaVariavel, {terminal}});
                    }

                    regra.dir[i] = mapaTerminais[terminal];
                }
            }
        }

        // Criar Produções de Duas Variáveis
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

    // Insere Novas Regras
    regras.insert(regras.end(), novasRegras.begin(), novasRegras.end());
}

void Gramatica::removerProducoesVazias() {

    // Variáveis que Geram a Palavra Vazia (Direta ou Indiretamente)
    std::unordered_set<Simbolo> anulaveis;
    
    bool mudou;
    
    // Mapear as Variáveis Anuláveis Diretamente
    for(const auto& regra : regras){
        if(regra.dir.empty() || (regra.dir.size() == 1 && regra.dir[0] == SIMBOLO_VAZIO)){
            anulaveis.insert(regra.esq);
        }
    }
    
    // Mapear as Variáveis Anuláveis Indiretamente
    do {

        mudou = false;
        
        for(const auto& regra : regras){
            if(anulaveis.find(regra.esq) == anulaveis.end()){
                
                bool todosAnulaveis = true;
                
                if(regra.dir.empty() || (regra.dir.size() == 1 && regra.dir[0] == SIMBOLO_VAZIO)){
                    continue;
                }
                
                for(const auto& s : regra.dir){
                    
                    // Se não encontrar nenhuma variável anulável indiretamente
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

    // Criação de Novas Regras de Produção 
    for(const auto& regra : regras){
        
        // Pulas as Regras de Produções Vazias Diretamente
        if(regra.dir.empty() || (regra.dir.size() == 1 && regra.dir[0] == SIMBOLO_VAZIO)){
            continue;
        }
        
        std::vector<int> indicesAnulaveis;

        // Determinar os Indices das Produções que são Anuláveis
        for(size_t i = 0; i < regra.dir.size(); ++i){
            if(anulaveis.find(regra.dir[i]) != anulaveis.end()){
                indicesAnulaveis.push_back(i);
            }
        }
        
        // Determinar Número de Combinações com Bitshift
        int numCombinacoes = 1 << indicesAnulaveis.size();
        
        // Montar Novas Regras
        for(int mask = 0; mask < numCombinacoes; ++mask){
            Regra novaRegra;
            novaRegra.esq = regra.esq;
            
            for(size_t i = 0; i < regra.dir.size(); ++i){
                bool pular = false;
                for(size_t j = 0; j < indicesAnulaveis.size(); ++j){
                    // Encontra a Variável Anulável e Pula. Gera as Novas Produções Utilizando um Bitmask
                    if(i == (size_t)indicesAnulaveis[j]){
                        if((mask & (1 << j)) != 0){
                            pular = true;
                        }
                        break;
                    }
                }

                // Insere Nova Regra
                if(!pular){
                    novaRegra.dir.push_back(regra.dir[i]);
                }
            }
            
            // Remove Duplicatas
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
    
    // Move as Novas Regras para as Atuais
    regras = move(novasRegras);
}

void Gramatica::removerProducoesUnitarias() {

    // Remover Produções A -> B e B -> a

    std::unordered_map<Simbolo, std::unordered_set<Simbolo>> paresUnitarios;
    
    for (const auto& nt : variaveis) {
        paresUnitarios[nt].insert(nt);
    }
    
    // Variável que Produz Variável
    for (const auto& regra : regras) {
        if (regra.dir.size() == 1 && ehVariavel(regra.dir[0])) {
            paresUnitarios[regra.esq].insert(regra.dir[0]);
        }
    }
    
    bool mudou;

    do {

        mudou = false;
        

        // Constrói Variáveis Alcançaveis
        for(const auto& nt1 : variaveis){
            auto alcancaveis = paresUnitarios[nt1];

            // Para cada Alcançavel, verifica o que ELE ALCANÇA 
            for(const auto& nt2 : alcancaveis){
                for(const auto& nt3 : paresUnitarios[nt2]){
                    if(paresUnitarios[nt1].find(nt3) == paresUnitarios[nt1].end()){
                        paresUnitarios[nt1].insert(nt3); // Insere se for NOVO ALCANÇAVEL
                        mudou = true;
                    }
                }
            }
        
        }

    } while (mudou);
    

    // Criação de Novas Regras
    std::vector<Regra> novasRegras;
    for(const auto& nt : variaveis){
        for(const auto& b : paresUnitarios[nt]){
            for(const auto& regra : regras){
                
                if(regra.esq == b){ // Filtra Pelas Regras que Partem de 'b'(VARIÁVEL)

                    bool ehUnitaria = (regra.dir.size() == 1 && ehVariavel(regra.dir[0])); // Verifica se Produz Variável Unitária
                    
                    if(!ehUnitaria){ // COPIA se NÃO FOR UNITÁRIA, ou seja, CRIA O ATALHO
                        Regra novaRegra;
                        novaRegra.esq = nt;
                        novaRegra.dir = regra.dir;
                        
                        bool existe = false;

                        // Se a regra JÁ EXISTE. PULA
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

    // Move NOVAS REGRAS DE PRODUÇÃO
    regras = move(novasRegras);
}

void Gramatica::removerSimbolosInuteis() {

    std::unordered_set<Simbolo> geradores;

    // Inicializa Conjunto de Geradores com os Terminais
    
    for(const auto& t : terminais) geradores.insert(t);
    
    geradores.insert(SIMBOLO_VAZIO);

    bool mudou;
    
    do {

        mudou = false;

        // Mapear Símbolos Alcançaveis
        for(const auto& regra : regras){
            
            if(geradores.find(regra.esq) == geradores.end()){ // Pula se SÍMBOLO (regra.esq) já for um GERADOR
                
                bool gera = true;

                // Se não GERAR pelo menos UM SÍMBOLO. Pula
                for(const auto& s : regra.dir){
                    if(geradores.find(s) == geradores.end()){
                        gera = false;
                        break;
                    }
                }

                // Se for GERADOR. Adiciona
                if(gera){
                    geradores.insert(regra.esq);
                    mudou = true;
                }
            }
        }

    } while(mudou);

    std::vector<Regra> regrasGeradoras;
    
    for(const auto& regra : regras){

        // Filtra AQUILO QUE DE FATO GERA TERMINAIS
        if(geradores.find(regra.esq) != geradores.end()){
            
            bool valida = true;
            
            // Mesma Lógica Anterior
            for (const auto& s : regra.dir){
                if(geradores.find(s) == geradores.end()){
                    valida = false;
                    break;
                }
            }

            if(valida) regrasGeradoras.push_back(regra);
        }
    }

    // Move Novas Regras
    regras = move(regrasGeradoras);

    std::unordered_set<Simbolo> alcancaveis;
    std::queue<Simbolo> fila;
    
    alcancaveis.insert(simboloInicial);
    fila.push(simboloInicial);

    // Aplica um BFS para Verificar Produções Alcançaveis
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

    // Filtra Regras Alcançaveis
    for(const auto& regra : regras){
        if(alcancaveis.find(regra.esq) != alcancaveis.end()){
            regrasFinais.push_back(regra);
        }
    }

    // Move Novas Regras
    regras = move(regrasFinais);

    // Definir Novas Variáveis

    std::vector<Simbolo> variaveisComRepeticao;

    // Mapear Todas as Regras (com repetição)
    for(auto &r : regras) variaveisComRepeticao.push_back(r.esq);

    // Criar SET sem REPETIÇÃO 
    std::set<Simbolo> variaveisSemRepeticao(variaveisComRepeticao.begin(), variaveisComRepeticao.end());

    std::vector<Simbolo> novasVariaveis;

    // Criar Novamente um VETOR SEM REPETIÇÃO
    for(auto &v : variaveisSemRepeticao) novasVariaveis.push_back(v);

    // Mover para Variáveis
    variaveis = move(novasVariaveis);

} 