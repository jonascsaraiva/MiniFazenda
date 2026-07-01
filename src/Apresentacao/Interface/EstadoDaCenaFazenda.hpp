#pragma once

namespace MiniFazenda::Apresentacao::Interface {

class EstadoDaCenaFazenda {
public:
    bool painelConfiguracoesAberto() const {
        return painelConfiguracoesAberto_;
    }

    void alternarPainelConfiguracoes() {
        painelConfiguracoesAberto_ = !painelConfiguracoesAberto_;
    }

    void fecharPainelConfiguracoes() {
        painelConfiguracoesAberto_ = false;
    }

    bool audioMutado() const {
        return audioMutado_;
    }

    void definirAudioMutado(bool audioMutado) {
        audioMutado_ = audioMutado;
    }

    void alternarAudioMutado() {
        audioMutado_ = !audioMutado_;
    }

    bool painelDaLojaAberto() const {
        return painelDaLojaAberto_;
    }

    void alternarPainelDaLoja() {
        painelDaLojaAberto_ = !painelDaLojaAberto_;
    }

    void fecharPainelDaLoja() {
        painelDaLojaAberto_ = false;
    }

private:
    bool painelConfiguracoesAberto_ = false;
    bool audioMutado_ = false;
    bool painelDaLojaAberto_ = false;
};

} // namespace MiniFazenda::Apresentacao::Interface
