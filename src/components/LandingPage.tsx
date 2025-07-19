import React from 'react';
import { Shield, Github, Download, CheckCircle, AlertTriangle, Lock } from 'lucide-react';

interface LandingPageProps {
  onStartScan: () => void;
}

export const LandingPage: React.FC<LandingPageProps> = ({ onStartScan }) => {
  return (
    <div className="min-h-screen bg-gradient-to-br from-blue-50 to-indigo-100">
      {/* Header */}
      <header className="bg-white shadow-sm">
        <div className="max-w-6xl mx-auto px-4 py-4 flex items-center justify-between">
          <div className="flex items-center space-x-3">
            <Shield className="h-8 w-8 text-blue-600" />
            <h1 className="text-2xl font-bold text-gray-900">SecureCheck</h1>
          </div>
          <a
            href="https://github.com/jeanccoelho/secure-check"
            target="_blank"
            rel="noopener noreferrer"
            className="flex items-center space-x-2 bg-gray-900 text-white px-4 py-2 rounded-lg hover:bg-gray-800 transition-colors"
          >
            <Github className="h-5 w-5" />
            <span>GitHub</span>
          </a>
        </div>
      </header>

      {/* Hero Section */}
      <section className="max-w-6xl mx-auto px-4 py-16">
        <div className="text-center mb-16">
          <h2 className="text-5xl font-bold text-gray-900 mb-6">
            Proteja seu sistema com um clique
          </h2>
          <p className="text-xl text-gray-600 mb-8 max-w-3xl mx-auto">
            SecureCheck é uma ferramenta open-source que verifica e corrige automaticamente 
            vulnerabilidades de segurança em sistemas Windows, Linux e macOS.
          </p>
          <div className="flex justify-center space-x-4">
            <button
              onClick={onStartScan}
              className="bg-blue-600 text-white px-8 py-4 rounded-lg text-lg font-semibold hover:bg-blue-700 transition-colors flex items-center space-x-2"
            >
              <Shield className="h-6 w-6" />
              <span>Iniciar Verificação</span>
            </button>
            <a
              href="#download"
              className="bg-white text-gray-900 px-8 py-4 rounded-lg text-lg font-semibold border-2 border-gray-300 hover:border-gray-400 transition-colors flex items-center space-x-2"
            >
              <Download className="h-6 w-6" />
              <span>Download</span>
            </a>
          </div>
        </div>

        {/* Features */}
        <div className="grid md:grid-cols-3 gap-8 mb-16">
          <div className="bg-white p-8 rounded-xl shadow-sm text-center">
            <CheckCircle className="h-12 w-12 text-green-600 mx-auto mb-4" />
            <h3 className="text-xl font-semibold mb-3">Verificação Automática</h3>
            <p className="text-gray-600">
              Detecta automaticamente vulnerabilidades conhecidas em seu sistema operacional.
            </p>
          </div>
          <div className="bg-white p-8 rounded-xl shadow-sm text-center">
            <AlertTriangle className="h-12 w-12 text-orange-600 mx-auto mb-4" />
            <h3 className="text-xl font-semibold mb-3">Correção Guiada</h3>
            <p className="text-gray-600">
              Interface intuitiva que guia você através do processo de correção passo a passo.
            </p>
          </div>
          <div className="bg-white p-8 rounded-xl shadow-sm text-center">
            <Lock className="h-12 w-12 text-blue-600 mx-auto mb-4" />
            <h3 className="text-xl font-semibold mb-3">Open Source</h3>
            <p className="text-gray-600">
              Código aberto e transparente, auditado pela comunidade de segurança.
            </p>
          </div>
        </div>

        {/* How it Works */}
        <div className="bg-white rounded-xl shadow-sm p-8 mb-16">
          <h3 className="text-3xl font-bold text-center mb-8">Como Funciona</h3>
          <div className="grid md:grid-cols-4 gap-6">
            <div className="text-center">
              <div className="bg-blue-100 w-16 h-16 rounded-full flex items-center justify-center mx-auto mb-4">
                <span className="text-2xl font-bold text-blue-600">1</span>
              </div>
              <h4 className="font-semibold mb-2">Execute como Admin</h4>
              <p className="text-sm text-gray-600">
                O programa precisa de privilégios administrativos para verificar e corrigir vulnerabilidades.
              </p>
            </div>
            <div className="text-center">
              <div className="bg-blue-100 w-16 h-16 rounded-full flex items-center justify-center mx-auto mb-4">
                <span className="text-2xl font-bold text-blue-600">2</span>
              </div>
              <h4 className="font-semibold mb-2">Verificação Step-by-Step</h4>
              <p className="text-sm text-gray-600">
                Cada vulnerabilidade é verificada individualmente e o resultado é exibido na tela.
              </p>
            </div>
            <div className="text-center">
              <div className="bg-blue-100 w-16 h-16 rounded-full flex items-center justify-center mx-auto mb-4">
                <span className="text-2xl font-bold text-blue-600">3</span>
              </div>
              <h4 className="font-semibold mb-2">Escolha a Ação</h4>
              <p className="text-sm text-gray-600">
                Para cada vulnerabilidade encontrada, você pode escolher corrigir ou pular.
              </p>
            </div>
            <div className="text-center">
              <div className="bg-blue-100 w-16 h-16 rounded-full flex items-center justify-center mx-auto mb-4">
                <span className="text-2xl font-bold text-blue-600">4</span>
              </div>
              <h4 className="font-semibold mb-2">Sistema Protegido</h4>
              <p className="text-sm text-gray-600">
                Ao final, seu sistema estará protegido contra as principais ameaças conhecidas.
              </p>
            </div>
          </div>
        </div>

        {/* Supported Systems */}
        <div className="text-center mb-16">
          <h3 className="text-3xl font-bold mb-8">Sistemas Suportados</h3>
          <div className="grid md:grid-cols-3 gap-6">
            <div className="bg-white p-6 rounded-xl shadow-sm">
              <h4 className="text-xl font-semibold mb-3">Windows</h4>
              <p className="text-gray-600 mb-4">10 verificações de segurança incluindo UAC, Firewall, SMBv1 e mais.</p>
              <span className="bg-blue-100 text-blue-800 px-3 py-1 rounded-full text-sm">10 verificações</span>
            </div>
            <div className="bg-white p-6 rounded-xl shadow-sm">
              <h4 className="text-xl font-semibold mb-3">Linux</h4>
              <p className="text-gray-600 mb-4">5 verificações essenciais incluindo SSH, Firewall, Sudo e kernel.</p>
              <span className="bg-green-100 text-green-800 px-3 py-1 rounded-full text-sm">5 verificações</span>
            </div>
            <div className="bg-white p-6 rounded-xl shadow-sm">
              <h4 className="text-xl font-semibold mb-3">macOS</h4>
              <p className="text-gray-600 mb-4">3 verificações principais incluindo Gatekeeper, Firewall e FileVault.</p>
              <span className="bg-purple-100 text-purple-800 px-3 py-1 rounded-full text-sm">3 verificações</span>
            </div>
          </div>
        </div>

        {/* Download Section */}
        <div id="download" className="bg-gradient-to-r from-blue-600 to-indigo-600 text-white rounded-xl p-8 text-center">
          <h3 className="text-3xl font-bold mb-4">Pronto para começar?</h3>
          <p className="text-xl mb-6 opacity-90">
            Baixe o SecureCheck e proteja seu sistema em minutos.
          </p>
          <div className="flex justify-center space-x-4">
            <button className="bg-white text-blue-600 px-6 py-3 rounded-lg font-semibold hover:bg-gray-100 transition-colors">
              Download para Windows
            </button>
            <button className="bg-white text-blue-600 px-6 py-3 rounded-lg font-semibold hover:bg-gray-100 transition-colors">
              Download para Linux
            </button>
            <button className="bg-white text-blue-600 px-6 py-3 rounded-lg font-semibold hover:bg-gray-100 transition-colors">
              Download para macOS
            </button>
          </div>
          <p className="mt-4 text-sm opacity-75">
            Código fonte disponível no <a href="https://github.com/jeanccoelho/secure-check" className="underline">GitHub</a>
          </p>
        </div>
      </section>

      {/* Footer */}
      <footer className="bg-gray-900 text-white py-8">
        <div className="max-w-6xl mx-auto px-4 text-center">
          <div className="flex items-center justify-center space-x-3 mb-4">
            <Shield className="h-6 w-6" />
            <span className="text-lg font-semibold">SecureCheck</span>
          </div>
          <p className="text-gray-400 mb-4">
            Ferramenta open-source para verificação e correção de vulnerabilidades de segurança.
          </p>
          <div className="flex justify-center space-x-6">
            <a href="https://github.com/jeanccoelho/secure-check" className="text-gray-400 hover:text-white transition-colors">
              GitHub
            </a>
            <a href="#" className="text-gray-400 hover:text-white transition-colors">
              Documentação
            </a>
            <a href="#" className="text-gray-400 hover:text-white transition-colors">
              Suporte
            </a>
          </div>
        </div>
      </footer>
    </div>
  );
};