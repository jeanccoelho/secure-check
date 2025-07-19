import React from 'react';
import { Shield, Github, Download, CheckCircle, AlertTriangle, Lock } from 'lucide-react';

interface LandingPageProps {
  onStartScan: () => void;
}

// Vulnerabilidades por sistema operacional
const vulnerabilities = {
  windows: [
    "UAC Desativado",
    "Firewall Desativado", 
    "SMBv1 Ativado",
    "AutoRun Habilitado",
    "Atualizações Desativadas",
    "Porta RDP Aberta",
    "Conta Admin Padrão Ativa",
    "Macros do Office Ativadas",
    "Antivírus Desativado",
    "Conta Convidado Ativa"
  ],
  linux: [
    "SSH com Root Permitido",
    "UFW não instalado",
    "UFW instalado mas inativo", 
    "SSH na porta padrão 22",
    "Fail2Ban não instalado",
    "Fail2Ban instalado mas inativo",
    "Atualizações automáticas não configuradas",
    "AppArmor não instalado",
    "AppArmor instalado mas inativo",
    "Sudo sem senha configurado",
    "Permissões inseguras em arquivos do sistema",
    "Kernel desatualizado"
  ],
  macos: [
    "Gatekeeper desativado",
    "Firewall desativado",
    "FileVault desativado"
  ]
};

export const LandingPage: React.FC<LandingPageProps> = ({ onStartScan }) => {
  const scrollToDownloads = () => {
    const downloadSection = document.getElementById('download');
    if (downloadSection) {
      downloadSection.scrollIntoView({ behavior: 'smooth' });
    }
  };
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
              onClick={scrollToDownloads}
              className="bg-blue-600 text-white px-8 py-4 rounded-lg text-lg font-semibold hover:bg-blue-700 transition-colors flex items-center space-x-2"
            >
              <Shield className="h-6 w-6" />
              <span>Ver Downloads</span>
            </button>
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
            <div className="bg-white p-6 rounded-xl shadow-sm border border-gray-200">
              <div className="flex items-center mb-4">
                <span className="text-3xl mr-3">🪟</span>
                <h4 className="text-xl font-semibold">Windows</h4>
              </div>
              <div className="text-left mb-4 max-h-48 overflow-y-auto">
                <p className="text-sm text-gray-600 mb-3">Verificações incluídas:</p>
                <ul className="text-sm text-gray-700 space-y-1">
                  {vulnerabilities.windows.map((vuln, index) => (
                    <li key={index} className="flex items-start">
                      <span className="text-green-500 mr-2 mt-0.5">•</span>
                      <span>{vuln}</span>
                    </li>
                  ))}
                </ul>
              </div>
              <div className="mt-4 pt-4 border-t border-gray-100">
                <span className="bg-blue-100 text-blue-800 px-3 py-1 rounded-full text-sm font-medium">
                  {vulnerabilities.windows.length} verificações
                </span>
                <button className="w-full mt-3 bg-blue-600 text-white px-4 py-2 rounded-lg hover:bg-blue-700 transition-colors font-medium">
                  <Download className="h-4 w-4 inline mr-2" />
                  Download Windows
                </button>
              </div>
            </div>
            
            <div className="bg-white p-6 rounded-xl shadow-sm border border-gray-200">
              <div className="flex items-center mb-4">
                <span className="text-3xl mr-3">🐧</span>
                <h4 className="text-xl font-semibold">Linux</h4>
              </div>
              <div className="text-left mb-4 max-h-48 overflow-y-auto">
                <p className="text-sm text-gray-600 mb-3">Verificações incluídas:</p>
                <ul className="text-sm text-gray-700 space-y-1">
                  {vulnerabilities.linux.map((vuln, index) => (
                    <li key={index} className="flex items-start">
                      <span className="text-green-500 mr-2 mt-0.5">•</span>
                      <span>{vuln}</span>
                    </li>
                  ))}
                </ul>
              </div>
              <div className="mt-4 pt-4 border-t border-gray-100">
                <span className="bg-green-100 text-green-800 px-3 py-1 rounded-full text-sm font-medium">
                  {vulnerabilities.linux.length} verificações
                </span>
                <button className="w-full mt-3 bg-green-600 text-white px-4 py-2 rounded-lg hover:bg-green-700 transition-colors font-medium">
                  <Download className="h-4 w-4 inline mr-2" />
                  Download Linux
                </button>
              </div>
            </div>
            
            <div className="bg-white p-6 rounded-xl shadow-sm border border-gray-200">
              <div className="flex items-center mb-4">
                <span className="text-3xl mr-3">🍎</span>
                <h4 className="text-xl font-semibold">macOS</h4>
              </div>
              <div className="text-left mb-4 max-h-48 overflow-y-auto">
                <p className="text-sm text-gray-600 mb-3">Verificações incluídas:</p>
                <ul className="text-sm text-gray-700 space-y-1">
                  {vulnerabilities.macos.map((vuln, index) => (
                    <li key={index} className="flex items-start">
                      <span className="text-green-500 mr-2 mt-0.5">•</span>
                      <span>{vuln}</span>
                    </li>
                  ))}
                </ul>
              </div>
              <div className="mt-4 pt-4 border-t border-gray-100">
                <span className="bg-purple-100 text-purple-800 px-3 py-1 rounded-full text-sm font-medium">
                  {vulnerabilities.macos.length} verificações
                </span>
                <button className="w-full mt-3 bg-purple-600 text-white px-4 py-2 rounded-lg hover:bg-purple-700 transition-colors font-medium">
                  <Download className="h-4 w-4 inline mr-2" />
                  Download macOS
                </button>
              </div>
            </div>
          </div>
        </div>

        {/* Download Section */}
        <div id="download" className="bg-gradient-to-r from-blue-600 to-indigo-600 text-white rounded-xl p-8 text-center mb-16">
          <h3 className="text-3xl font-bold mb-4">Pronto para começar?</h3>
          <p className="text-xl mb-6 opacity-90">
            Baixe o SecureCheck e proteja seu sistema em minutos.
          </p>
          <div className="flex flex-wrap justify-center gap-4">
            <button 
              onClick={onStartScan}
              className="bg-white text-blue-600 px-6 py-3 rounded-lg font-semibold hover:bg-gray-100 transition-colors flex items-center space-x-2"
            >
              <Shield className="h-5 w-5" />
              <span>Testar Online</span>
            </button>
            <a 
              href="https://github.com/jeanccoelho/secure-check/releases"
              target="_blank"
              rel="noopener noreferrer"
              className="bg-gray-800 text-white px-6 py-3 rounded-lg font-semibold hover:bg-gray-700 transition-colors flex items-center space-x-2"
            >
              <Github className="h-5 w-5" />
              <span>Ver Releases</span>
            </a>
          </div>
          <div className="mt-6 text-center">
            <p className="text-sm opacity-75">
              Ou baixe diretamente pelos cards acima para seu sistema operacional
            </button>
            <p className="mt-2 text-sm opacity-75">
              Código fonte disponível no <a href="https://github.com/jeanccoelho/secure-check" className="underline">GitHub</a>
            </p>
          </div>
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