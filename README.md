# Trabalho de LFA - Linguagens Livres de Contexto

- Este trabalho possui os seguintes arquivos:
    > */src/leitor.cpp* Funções para realizar a leitura da gramática
    > */src/gramatica.cpp* Funções da gramática (testar variável, testar terminal, simplificações, converter para forma normal)
    > */src/cyk.cpp* Implementa o Algoritmo CYK para o reconhecimento de palavras geradas pela gramática 

### Instruções

- Para executar o programa: *make run*

- O executável é enviado para o diretório */out* e então é executado pelo arquivo *Makefile* 

- A aplicação consiste de uma interface com 4 opções:
    > Simplificar a Gramática Lida
    > Converter a Gramática para Forma Normal de Chomsky
    > Testar uma Palavra
    > Sair do Programa

- Para testar uma palavra, deve convertê-la para forma normal de Chomsky, e para converter para a forma normal de Chomsky, deve primeiro simplificá-la.