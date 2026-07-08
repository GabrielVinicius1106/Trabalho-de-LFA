#ifndef LEITOR_HPP
#define LEITOR_HPP

#include "gramatica.hpp"
#include <stdexcept>
#include <string>

using namespace std;

class ErroLeituraGLC : public runtime_error {
public:
    explicit ErroLeituraGLC(const string& msg) : runtime_error(msg) {}
};

Gramatica lerArquivoGLC(const string& caminhoArquivo);

#endif
