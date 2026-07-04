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

private:
    bool painelConfiguracoesAberto_ = false;
    bool audioMutado_ = false;
};

} // namespace MiniFazenda::Apresentacao::Interface
