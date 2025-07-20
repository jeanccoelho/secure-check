#!/bin/bash

# Script automatizado para criar releases do SecurityChecker
# Uso: ./release.sh [version]

set -e

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Função para mostrar mensagens coloridas
log_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

log_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

log_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

log_error() {
    echo -e "${RED}❌ $1${NC}"
}

# Banner
echo -e "${BLUE}"
echo "🚀 SecurityChecker Release Automation"
echo "====================================="
echo -e "${NC}"

# Verificar se estamos em um repositório git
if ! git rev-parse --git-dir > /dev/null 2>&1; then
    log_error "Este diretório não é um repositório Git!"
    exit 1
fi

# Verificar se há mudanças não commitadas
if ! git diff-index --quiet HEAD --; then
    log_warning "Há mudanças não commitadas no repositório."
    echo "Mudanças pendentes:"
    git status --porcelain
    echo ""
    read -p "Deseja continuar mesmo assim? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        log_info "Release cancelado."
        exit 0
    fi
fi

# Obter versão
VERSION="$1"
if [[ -z "$VERSION" ]]; then
    echo "Versões existentes:"
    git tag -l | sort -V | tail -5
    echo ""
    read -p "Digite a nova versão (ex: 1.0.0): " VERSION
fi

# Validar formato da versão
if [[ ! $VERSION =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    log_error "Formato de versão inválido! Use o formato: X.Y.Z (ex: 1.0.0)"
    exit 1
fi

TAG="v$VERSION"

# Verificar se a tag já existe
if git rev-parse "$TAG" >/dev/null 2>&1; then
    log_error "A tag $TAG já existe!"
    exit 1
fi

log_info "Preparando release da versão: $VERSION"
echo ""

# Verificar se o remote origin existe
if ! git remote get-url origin >/dev/null 2>&1; then
    log_error "Remote 'origin' não configurado!"
    exit 1
fi

REPO_URL=$(git remote get-url origin)
log_info "Repositório: $REPO_URL"

# Verificar se estamos na branch main/master
CURRENT_BRANCH=$(git branch --show-current)
if [[ "$CURRENT_BRANCH" != "main" && "$CURRENT_BRANCH" != "master" ]]; then
    log_warning "Você está na branch '$CURRENT_BRANCH', não em 'main' ou 'master'."
    read -p "Continuar mesmo assim? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        log_info "Release cancelado."
        exit 0
    fi
fi

# Atualizar arquivos de versão
log_info "Atualizando arquivos de versão..."

# Atualizar CMakeLists.txt
if [[ -f "cpp-app/CMakeLists.txt" ]]; then
    sed -i.bak "s/set(CPACK_PACKAGE_VERSION_MAJOR [0-9]*)/set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION%%.*})/" cpp-app/CMakeLists.txt
    VERSION_MINOR=$(echo $VERSION | cut -d. -f2)
    VERSION_PATCH=$(echo $VERSION | cut -d. -f3)
    sed -i.bak "s/set(CPACK_PACKAGE_VERSION_MINOR [0-9]*)/set(CPACK_PACKAGE_VERSION_MINOR $VERSION_MINOR)/" cpp-app/CMakeLists.txt
    sed -i.bak "s/set(CPACK_PACKAGE_VERSION_PATCH [0-9]*)/set(CPACK_PACKAGE_VERSION_PATCH $VERSION_PATCH)/" cpp-app/CMakeLists.txt
    rm -f cpp-app/CMakeLists.txt.bak
    log_success "CMakeLists.txt atualizado"
fi

# Atualizar package.json
if [[ -f "package.json" ]]; then
    sed -i.bak "s/\"version\": \"[^\"]*\"/\"version\": \"$VERSION\"/" package.json
    rm -f package.json.bak
    log_success "package.json atualizado"
fi

# Atualizar main.cpp
if [[ -f "cpp-app/src/main.cpp" ]]; then
    sed -i.bak "s/app.setApplicationVersion(\"[^\"]*\")/app.setApplicationVersion(\"$VERSION\")/" cpp-app/src/main.cpp
    rm -f cpp-app/src/main.cpp.bak
    log_success "main.cpp atualizado"
fi

# Gerar changelog automático
log_info "Gerando changelog..."
CHANGELOG_FILE="CHANGELOG-$VERSION.md"

cat > "$CHANGELOG_FILE" << EOF
# SecurityChecker v$VERSION

## 📅 Data de Release
$(date '+%d/%m/%Y %H:%M:%S')

## 📥 Downloads

- **Windows**: [SecurityChecker-$VERSION-windows-x64.zip](https://github.com/$(git remote get-url origin | sed 's/.*github.com[:/]\([^/]*\/[^/]*\).*/\1/' | sed 's/\.git$//')/releases/download/v$VERSION/SecurityChecker-$VERSION-windows-x64.zip)
- **Linux**: [SecurityChecker-$VERSION-linux-x64.tar.gz](https://github.com/$(git remote get-url origin | sed 's/.*github.com[:/]\([^/]*\/[^/]*\).*/\1/' | sed 's/\.git$//')/releases/download/v$VERSION/SecurityChecker-$VERSION-linux-x64.tar.gz)
- **macOS**: [SecurityChecker-$VERSION-macos-x64.tar.gz](https://github.com/$(git remote get-url origin | sed 's/.*github.com[:/]\([^/]*\/[^/]*\).*/\1/' | sed 's/\.git$//')/releases/download/v$VERSION/SecurityChecker-$VERSION-macos-x64.tar.gz)

## 🚀 Como usar

1. Baixe o arquivo para seu sistema operacional
2. Extraia o arquivo
3. Execute como administrador/root:
   - **Windows**: Clique com botão direito em \`run.bat\` → "Executar como administrador"
   - **Linux/macOS**: \`sudo ./run.sh\`

## 📋 Mudanças nesta versão

$(git log --oneline --pretty=format:"- %s" $(git describe --tags --abbrev=0 2>/dev/null || echo "HEAD~10")..HEAD | head -20)

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

**Código fonte**: https://github.com/$(git remote get-url origin | sed 's/.*github.com[:/]\([^/]*\/[^/]*\).*/\1/' | sed 's/\.git$//')
EOF

log_success "Changelog gerado: $CHANGELOG_FILE"

# Mostrar resumo
echo ""
log_info "📋 Resumo do Release:"
echo "  • Versão: $VERSION"
echo "  • Tag: $TAG"
echo "  • Branch: $CURRENT_BRANCH"
echo "  • Arquivos atualizados: CMakeLists.txt, package.json, main.cpp"
echo "  • Changelog: $CHANGELOG_FILE"
echo ""

# Confirmação final
read -p "🚀 Confirma a criação do release? (y/N): " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    log_info "Release cancelado."
    rm -f "$CHANGELOG_FILE"
    exit 0
fi

# Commit das mudanças de versão
log_info "Fazendo commit das mudanças de versão..."
git add .
git commit -m "chore: bump version to $VERSION

- Update version in CMakeLists.txt
- Update version in package.json  
- Update version in main.cpp
- Generate changelog for v$VERSION"

log_success "Commit realizado"

# Push das mudanças
log_info "Enviando mudanças para o repositório..."
git push origin "$CURRENT_BRANCH"
log_success "Mudanças enviadas"

# Criar e enviar tag
log_info "Criando tag $TAG..."
git tag -a "$TAG" -m "Release $VERSION

$(cat "$CHANGELOG_FILE")"

log_info "Enviando tag para o repositório..."
git push origin "$TAG"
log_success "Tag $TAG criada e enviada"

# Aguardar um pouco para o GitHub processar
log_info "Aguardando GitHub processar a tag..."
sleep 3

# Verificar se o GitHub Actions foi acionado
log_info "Verificando GitHub Actions..."
REPO_NAME=$(git remote get-url origin | sed 's/.*github.com[:/]\([^/]*\/[^/]*\).*/\1/' | sed 's/\.git$//')
ACTIONS_URL="https://github.com/$REPO_NAME/actions"
RELEASES_URL="https://github.com/$REPO_NAME/releases"

echo ""
log_success "🎉 Release iniciado com sucesso!"
echo ""
echo "📋 Próximos passos:"
echo "  1. ⏳ Aguarde o GitHub Actions compilar (~10-15 minutos)"
echo "  2. 👀 Acompanhe o progresso: $ACTIONS_URL"
echo "  3. 📦 Verifique o release: $RELEASES_URL"
echo "  4. 🧪 Teste os executáveis quando prontos"
echo ""
echo "📁 Arquivos que serão gerados:"
echo "  • SecurityChecker-$VERSION-windows-x64.zip"
echo "  • SecurityChecker-$VERSION-linux-x64.tar.gz"
echo "  • SecurityChecker-$VERSION-macos-x64.tar.gz"
echo ""

# Abrir URLs no navegador (se disponível)
if command -v xdg-open >/dev/null 2>&1; then
    read -p "🌐 Abrir GitHub Actions no navegador? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        xdg-open "$ACTIONS_URL"
    fi
elif command -v open >/dev/null 2>&1; then
    read -p "🌐 Abrir GitHub Actions no navegador? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        open "$ACTIONS_URL"
    fi
fi

# Limpar arquivo temporário
rm -f "$CHANGELOG_FILE"

log_success "Script concluído! 🚀"
echo ""
echo "💡 Dica: Use 'git log --oneline' para ver o histórico de commits"
echo "💡 Dica: Use 'git tag -l' para ver todas as tags criadas"