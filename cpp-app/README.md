# SecurityChecker

Uma ferramenta open-source para verificação e correção automática de vulnerabilidades de segurança em sistemas Windows, Linux e macOS.

## Características

- ✅ **Verificação Automática**: Detecta automaticamente vulnerabilidades conhecidas
- ⚠️ **Correção Guiada**: Interface intuitiva para correção passo a passo
- 🔓 **Open Source**: Código transparente e auditado pela comunidade
- 🖥️ **Multi-plataforma**: Suporte para Windows, Linux e macOS
- 🛡️ **Interface Intuitiva**: Projetada para usuários domésticos

## Sistemas Suportados

### Windows (10 verificações)
- UAC Desativado
- Firewall Desativado
- SMBv1 Ativado
- AutoRun Habilitado
- Atualizações Desativadas
- Porta RDP Aberta
- Conta Admin Padrão Ativa
- Macros do Office Ativadas
- Antivírus Desativado
- Conta Convidado Ativa

### Linux (10 verificações)
- SSH com Root Permitido
- Firewall Inativo
- Sudo sem senha
- Kernel Desatualizado
- FTP com Anônimo Permitido
- Atualizações automáticas desativadas
- AppArmor desativado
- SSH na Porta 22
- Fail2Ban ausente
- Permissões inseguras em arquivos do sistema

### macOS (10 verificações)
- Gatekeeper desativado
- Firewall desativado
- FileVault desativado
- Montagem automática de USB
- Backup iCloud ativo para pastas sensíveis
- Execução de extensões não identificadas
- Sem senha ao sair do repouso
- Serviços de Compartilhamento Ativos
- System Integrity Protection desativado
- Login remoto ativo

## Requisitos

### Compilação
- Qt6 (Core, Widgets, Network)
- CMake 3.16+
- Compilador C++17

### Execução
- **IMPORTANTE**: Deve ser executado como administrador/root
- Privilégios elevados são necessários para verificar e corrigir vulnerabilidades

## Compilação

### Windows
```bash
# Instalar Qt6 e CMake
# Configurar variáveis de ambiente para Qt6

mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Linux
```bash
# Ubuntu/Debian
sudo apt install qt6-base-dev qt6-tools-dev cmake build-essential

# Fedora
sudo dnf install qt6-qtbase-devel qt6-qttools-devel cmake gcc-c++

mkdir build
cd build
cmake ..
make -j$(nproc)
```

### macOS
```bash
# Instalar Qt6 via Homebrew
brew install qt6 cmake

mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=$(brew --prefix qt6) ..
make -j$(sysctl -n hw.ncpu)
```

## Uso

1. **Execute como Administrador**
   - Windows: Clique com botão direito → "Executar como administrador"
   - Linux/macOS: `sudo ./SecurityChecker`

2. **Selecione o Sistema Operacional**
   - A detecção automática é feita, mas você pode alterar se necessário

3. **Verificação Step-by-Step**
   - Clique em "Iniciar Verificação" para cada vulnerabilidade
   - Aguarde o resultado da verificação

4. **Escolha a Ação**
   - **Corrigir e Continuar**: Aplica a correção automaticamente
   - **Pular sem Corrigir**: Ignora esta vulnerabilidade

5. **Visualize o Resumo**
   - Ao final, veja estatísticas completas das verificações

## Como Funciona

1. **Carregamento das Definições**: Lê o arquivo `vulnerabilities.json` com as definições de vulnerabilidades
2. **Detecção do Sistema**: Identifica automaticamente o sistema operacional
3. **Verificação Individual**: Cada vulnerabilidade é verificada usando comandos específicos do sistema
4. **Correção Automática**: Executa comandos de correção quando solicitado
5. **Relatório Final**: Apresenta resumo completo das ações realizadas

## Estrutura do Projeto

```
SecurityChecker/
├── CMakeLists.txt              # Configuração do CMake
├── README.md                   # Este arquivo
├── data/
│   └── vulnerabilities.json   # Definições das vulnerabilidades
├── include/                    # Headers C++
│   ├── MainWindow.h
│   ├── LandingPage.h
│   ├── SecurityChecker.h
│   ├── VulnerabilityManager.h
│   ├── SystemChecker.h
│   └── VulnerabilityDefinition.h
└── src/                        # Código fonte C++
    ├── main.cpp
    ├── MainWindow.cpp
    ├── LandingPage.cpp
    ├── SecurityChecker.cpp
    ├── VulnerabilityManager.cpp
    └── SystemChecker.cpp
```

## Segurança

- **Código Aberto**: Todo o código é auditável e transparente
- **Privilégios Mínimos**: Solicita privilégios administrativos apenas quando necessário
- **Verificação Antes da Correção**: Sempre verifica antes de aplicar correções
- **Controle do Usuário**: O usuário decide quais correções aplicar

## Contribuição

1. Fork o projeto
2. Crie uma branch para sua feature (`git checkout -b feature/AmazingFeature`)
3. Commit suas mudanças (`git commit -m 'Add some AmazingFeature'`)
4. Push para a branch (`git push origin feature/AmazingFeature`)
5. Abra um Pull Request

## Licença

Este projeto está licenciado sob a MIT License - veja o arquivo [LICENSE](LICENSE) para detalhes.

## Aviso Legal

Esta ferramenta é fornecida "como está" sem garantias. Use por sua própria conta e risco. Sempre faça backup de seus dados antes de aplicar correções de segurança.

## Suporte

- 🐛 **Issues**: [GitHub Issues](https://github.com/seu-usuario/securitychecker/issues)
- 📖 **Documentação**: [Wiki](https://github.com/seu-usuario/securitychecker/wiki)
- 💬 **Discussões**: [GitHub Discussions](https://github.com/seu-usuario/securitychecker/discussions)