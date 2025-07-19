# SecureCheck - Documentação

## 📖 Visão Geral

O **SecureCheck** é uma ferramenta open-source desenvolvida para verificar e corrigir automaticamente vulnerabilidades de segurança em sistemas Windows, Linux e macOS. Com uma interface intuitiva e processo guiado, permite que usuários domésticos protejam seus sistemas sem conhecimento técnico avançado.

## 🎯 Objetivo

Democratizar a segurança digital, oferecendo uma ferramenta gratuita e transparente que:
- Detecta vulnerabilidades conhecidas
- Oferece correções automáticas
- Educa sobre riscos de segurança
- Mantém o controle nas mãos do usuário

## 🚀 Como Usar

### 1. Download e Instalação

#### Windows
1. Baixe o arquivo `.exe` da [página de releases](https://github.com/jeanccoelho/secure-check/releases)
2. Execute como **Administrador** (clique com botão direito → "Executar como administrador")
3. Siga as instruções na tela

#### Linux
1. Baixe o arquivo `.tar.gz` para Linux
2. Extraia: `tar -xzf SecurityChecker-*.tar.gz`
3. Execute: `sudo ./SecurityChecker`

#### macOS
1. Baixe o arquivo `.dmg` para macOS
2. Monte a imagem e arraste para Applications
3. Execute com privilégios administrativos

### 2. Processo de Verificação

1. **Detecção Automática**: O sistema operacional é detectado automaticamente
2. **Verificação Step-by-Step**: Cada vulnerabilidade é verificada individualmente
3. **Escolha Informada**: Para cada problema encontrado, você decide se corrige ou ignora
4. **Relatório Final**: Resumo completo das ações realizadas

## 🛡️ Vulnerabilidades Verificadas

### Windows (10 verificações)
- **UAC Desativado**: Controle de Conta de Usuário
- **Firewall Desativado**: Proteção de rede
- **SMBv1 Ativado**: Protocolo legado vulnerável
- **AutoRun Habilitado**: Execução automática de mídias
- **Atualizações Desativadas**: Windows Update
- **Porta RDP Aberta**: Acesso remoto exposto
- **Conta Admin Padrão**: Administrator ativa
- **Macros do Office**: Execução automática
- **Antivírus Desativado**: Windows Defender
- **Conta Convidado**: Guest ativa

### Linux (12 verificações)
- **SSH Root Login**: Acesso root remoto
- **Firewall Inativo**: UFW desabilitado
- **SSH Porta Padrão**: Porta 22 exposta
- **Fail2Ban Ausente**: Proteção contra força bruta
- **Atualizações Manuais**: Sem atualizações automáticas
- **AppArmor Inativo**: Proteção adicional
- **Sudo sem Senha**: Configuração NOPASSWD
- **Permissões Inseguras**: Arquivos do sistema
- **Kernel Desatualizado**: Versão antiga
- **Serviços Desnecessários**: Exposição de rede

### macOS (10 verificações)
- **Gatekeeper Desativado**: Verificação de apps
- **Firewall Desativado**: Proteção de rede
- **FileVault Desativado**: Criptografia de disco
- **Montagem Automática**: USB automático
- **iCloud Backup**: Sincronização inadvertida
- **Extensões Desconhecidas**: Software não verificado
- **Sem Senha no Sleep**: Desbloqueio automático
- **Serviços de Compartilhamento**: Exposição na rede
- **SIP Desativado**: System Integrity Protection
- **Login Remoto**: SSH ativo

## ⚙️ Como Funciona

### Detecção de Vulnerabilidades
O SecureCheck utiliza comandos específicos do sistema operacional para verificar configurações de segurança:

- **Windows**: Registry, PowerShell, netsh
- **Linux**: Arquivos de configuração, systemctl, comandos de sistema
- **macOS**: defaults, spctl, comandos específicos do macOS

### Processo de Correção
1. **Verificação Prévia**: Confirma se a vulnerabilidade existe
2. **Backup Automático**: Salva configurações originais quando possível
3. **Aplicação Segura**: Executa comandos de correção testados
4. **Verificação Posterior**: Confirma se a correção foi aplicada

### Segurança do Processo
- **Código Aberto**: Todo código é auditável
- **Privilégios Mínimos**: Solicita admin apenas quando necessário
- **Controle do Usuário**: Você decide o que corrigir
- **Transparência**: Mostra exatamente o que será executado

## 🔧 Requisitos Técnicos

### Compilação (Desenvolvedores)
- **Qt6** (Core, Widgets, Network)
- **CMake 3.16+**
- **Compilador C++17**
- **Privilégios administrativos** para execução

### Execução (Usuários)
- **Sistema Operacional**: Windows 10+, Linux (Ubuntu 18.04+), macOS 10.14+
- **Privilégios**: Administrador/root
- **Espaço**: ~50MB
- **Memória**: ~100MB RAM

## 📊 Interpretando Resultados

### Status das Verificações
- **✅ Seguro**: Configuração está correta
- **⚠️ Vulnerável**: Problema encontrado, correção disponível
- **🔧 Corrigido**: Vulnerabilidade foi corrigida automaticamente
- **⏭️ Ignorado**: Usuário optou por não corrigir

### Níveis de Severidade
- **🔴 Alta**: Risco crítico, correção urgente recomendada
- **🟡 Média**: Risco moderado, correção recomendada
- **🟢 Baixa**: Risco menor, correção opcional

## 🛠️ Solução de Problemas

### Problemas Comuns

#### "Acesso Negado"
- **Causa**: Aplicação não executada como administrador
- **Solução**: Execute com privilégios elevados

#### "Comando não encontrado"
- **Causa**: Sistema não suportado ou comando ausente
- **Solução**: Verifique compatibilidade do sistema

#### "Falha na correção"
- **Causa**: Configuração protegida ou conflito
- **Solução**: Execute comando manualmente ou consulte documentação específica

### Logs e Debug
O SecureCheck gera logs detalhados que podem ser úteis para diagnóstico:
- **Windows**: `%TEMP%\SecurityChecker.log`
- **Linux/macOS**: `/tmp/SecurityChecker.log`

## 🤝 Contribuindo

### Como Contribuir
1. **Fork** o repositório
2. **Clone** sua fork: `git clone https://github.com/seu-usuario/secure-check.git`
3. **Crie branch**: `git checkout -b feature/nova-funcionalidade`
4. **Faça mudanças** e teste
5. **Commit**: `git commit -m "Adiciona nova funcionalidade"`
6. **Push**: `git push origin feature/nova-funcionalidade`
7. **Pull Request**: Abra PR no repositório original

### Áreas de Contribuição
- **Novas Vulnerabilidades**: Adicione verificações para outras ameaças
- **Suporte a Sistemas**: Expanda compatibilidade
- **Interface**: Melhorias na UX/UI
- **Documentação**: Traduções e melhorias
- **Testes**: Casos de teste automatizados

## 📝 Licença

Este projeto está licenciado sob a **MIT License**:

```
MIT License

Copyright (c) 2024 SecureCheck Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🔗 Links Úteis

- **Repositório**: [GitHub](https://github.com/jeanccoelho/secure-check)
- **Issues**: [Reportar Problemas](https://github.com/jeanccoelho/secure-check/issues)
- **Releases**: [Downloads](https://github.com/jeanccoelho/secure-check/releases)
- **Wiki**: [Documentação Avançada](https://github.com/jeanccoelho/secure-check/wiki)

## 📞 Suporte

### Canais de Suporte
- **GitHub Issues**: Para bugs e solicitações de recursos
- **Discussions**: Para perguntas gerais e discussões
- **Email**: Para questões de segurança sensíveis

### Reportando Vulnerabilidades
Se você encontrar uma vulnerabilidade de segurança no próprio SecureCheck:
1. **NÃO** abra uma issue pública
2. Envie email para: security@securecheck.org
3. Inclua detalhes técnicos e passos para reproduzir
4. Aguarde resposta em até 48 horas

---

**SecureCheck** - Democratizando a segurança digital 🛡️