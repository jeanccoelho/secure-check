# SecurityChecker v1.0.0

## 📅 Data de Release
19/07/2025 21:04:19

## 📥 Downloads

- **Windows**: [SecurityChecker-1.0.0-windows-x64.zip](https://github.com/jeanccoelho/secure-check/releases/download/v1.0.0/SecurityChecker-1.0.0-windows-x64.zip)
- **Linux**: [SecurityChecker-1.0.0-linux-x64.tar.gz](https://github.com/jeanccoelho/secure-check/releases/download/v1.0.0/SecurityChecker-1.0.0-linux-x64.tar.gz)
- **macOS**: [SecurityChecker-1.0.0-macos-x64.tar.gz](https://github.com/jeanccoelho/secure-check/releases/download/v1.0.0/SecurityChecker-1.0.0-macos-x64.tar.gz)

## 🚀 Como usar

1. Baixe o arquivo para seu sistema operacional
2. Extraia o arquivo
3. Execute como administrador/root:
   - **Windows**: Clique com botão direito em `run.bat` → "Executar como administrador"
   - **Linux/macOS**: `sudo ./run.sh`

## 📋 Mudanças nesta versão

- ...
- Update codebase with external changes
- ...
- Sincronizar vulnerabilidades do macOS entre JSON e web
- Updated .gitignore
- Configurar repositório GitHub e primeiro release
- Criar script automatizado de release
- Configurar GitHub Actions para releases automáticos
- Updated README.md
- Corrigir responsividade do card CTA

## 🛡️ Verificações de Segurança

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

### Linux (12 verificações)
- SSH Root Login
- UFW não instalado/inativo
- SSH na porta padrão
- Fail2Ban ausente/inativo
- Atualizações automáticas desativadas
- AppArmor ausente/inativo
- Sudo sem senha
- Permissões inseguras
- Kernel desatualizado

### macOS (10 verificações)
- Gatekeeper desativado
- Firewall desativado
- FileVault desativado
- Montagem automática USB
- iCloud Backup ativo
- Extensões desconhecidas
- Sem senha no sleep
- Serviços de compartilhamento
- SIP desativado
- Login remoto ativo

## 🔧 Requisitos Técnicos

- **Privilégios**: Administrador/root obrigatório
- **Sistemas**: Windows 10+, Linux (Ubuntu 18.04+), macOS 10.14+
- **Espaço**: ~50MB
- **Memória**: ~100MB RAM

---

**Código fonte**: https://github.com/jeanccoelho/secure-check
