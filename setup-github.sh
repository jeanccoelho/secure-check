#!/bin/bash

# Script para configurar o repositório GitHub e fazer o primeiro release
# Uso: ./setup-github.sh [nome-do-repositorio]

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
echo "🚀 SecureCheck - Setup GitHub Repository"
echo "========================================"
echo -e "${NC}"

# Verificar se Git está instalado
if ! command -v git &> /dev/null; then
    log_error "Git não está instalado!"
    exit 1
fi

# Verificar se GitHub CLI está instalado
if ! command -v gh &> /dev/null; then
    log_warning "GitHub CLI não encontrado."
    echo "Para criar o repositório automaticamente, instale o GitHub CLI:"
    echo "  • macOS: brew install gh"
    echo "  • Ubuntu: sudo apt install gh"
    echo "  • Windows: winget install GitHub.cli"
    echo ""
    echo "Ou crie o repositório manualmente em: https://github.com/new"
    echo ""
fi

# Obter nome do repositório
REPO_NAME="${1:-secure-check}"
log_info "Nome do repositório: $REPO_NAME"

# Verificar se já é um repositório Git
if [ -d ".git" ]; then
    log_warning "Já é um repositório Git. Verificando remote..."
    
    if git remote get-url origin &>/dev/null; then
        CURRENT_REMOTE=$(git remote get-url origin)
        log_info "Remote atual: $CURRENT_REMOTE"
        
        read -p "Deseja continuar com este repositório? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            log_info "Operação cancelada."
            exit 0
        fi
    else
        log_info "Repositório Git local sem remote configurado."
    fi
else
    log_info "Inicializando repositório Git..."
    git init
    log_success "Repositório Git inicializado"
fi

# Configurar .gitignore se não existir
if [ ! -f ".gitignore" ]; then
    log_info "Criando .gitignore..."
    cat > .gitignore << 'EOF'
# Dependências
node_modules/
package-lock.json

# Build outputs
dist/
build/
*.exe
*.app
*.dmg
*.deb
*.rpm

# C++ builds
cpp-app/build*/
cpp-app/packages/

# Logs
*.log
npm-debug.log*

# Environment
.env
.env.local

# IDE
.vscode/
.idea/

# OS
.DS_Store
Thumbs.db

# Temporary files
*.tmp
*.temp
*~
EOF
    log_success ".gitignore criado"
fi

# Adicionar todos os arquivos
log_info "Adicionando arquivos ao Git..."
git add .

# Verificar se há algo para commitar
if git diff --staged --quiet; then
    log_warning "Nenhuma mudança para commitar."
else
    log_info "Fazendo commit inicial..."
    git commit -m "feat: initial commit - SecureCheck v1.0.0

🛡️ SecureCheck - Ferramenta open-source para verificação de vulnerabilidades

✨ Características:
- Interface web moderna (React + TypeScript)
- Aplicação desktop multiplataforma (Qt6 + C++)
- Verificações para Windows, Linux e macOS
- Correções automáticas guiadas
- Builds automáticos via GitHub Actions

📦 Componentes:
- Frontend web responsivo
- Aplicação C++ com Qt6
- Scripts de build multiplataforma
- GitHub Actions para releases
- Documentação completa

🚀 Pronto para primeiro release!"

    log_success "Commit inicial realizado"
fi

# Configurar remote se GitHub CLI estiver disponível
if command -v gh &> /dev/null; then
    log_info "Verificando autenticação GitHub..."
    
    if gh auth status &>/dev/null; then
        log_success "GitHub CLI autenticado"
        
        # Verificar se repositório já existe
        if gh repo view "$REPO_NAME" &>/dev/null; then
            log_warning "Repositório $REPO_NAME já existe no GitHub"
            REPO_URL="https://github.com/$(gh api user --jq .login)/$REPO_NAME"
        else
            log_info "Criando repositório no GitHub..."
            
            gh repo create "$REPO_NAME" \
                --description "🛡️ Ferramenta open-source para verificação e correção de vulnerabilidades de segurança" \
                --public \
                --clone=false \
                --add-readme=false
            
            REPO_URL="https://github.com/$(gh api user --jq .login)/$REPO_NAME"
            log_success "Repositório criado: $REPO_URL"
        fi
        
        # Configurar remote
        if ! git remote get-url origin &>/dev/null; then
            git remote add origin "$REPO_URL.git"
            log_success "Remote origin configurado"
        fi
        
    else
        log_warning "GitHub CLI não está autenticado."
        echo "Execute: gh auth login"
        echo ""
    fi
fi

# Push para o GitHub
if git remote get-url origin &>/dev/null; then
    log_info "Enviando código para o GitHub..."
    
    # Verificar se branch main existe no remote
    if git ls-remote --heads origin main | grep -q main; then
        git push origin main
    else
        # Primeiro push
        git branch -M main
        git push -u origin main
    fi
    
    log_success "Código enviado para o GitHub"
    
    # Obter URL do repositório
    REPO_URL=$(git remote get-url origin | sed 's/\.git$//')
    
    echo ""
    log_success "🎉 Repositório configurado com sucesso!"
    echo ""
    echo "📋 Próximos passos:"
    echo "  1. 🌐 Acesse: $REPO_URL"
    echo "  2. ⚙️  Configure secrets se necessário"
    echo "  3. 🚀 Execute: ./release.sh 1.0.0"
    echo ""
    echo "🔗 Links úteis:"
    echo "  • Repositório: $REPO_URL"
    echo "  • Actions: $REPO_URL/actions"
    echo "  • Releases: $REPO_URL/releases"
    echo ""
    
    # Perguntar se quer fazer o primeiro release
    read -p "🚀 Deseja fazer o primeiro release agora? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        if [ -f "./release.sh" ]; then
            log_info "Executando script de release..."
            chmod +x ./release.sh
            ./release.sh 1.0.0
        else
            log_error "Script release.sh não encontrado!"
        fi
    fi
    
else
    log_warning "Remote origin não configurado."
    echo ""
    echo "📋 Para configurar manualmente:"
    echo "  1. Crie um repositório em: https://github.com/new"
    echo "  2. Execute: git remote add origin https://github.com/SEU-USUARIO/$REPO_NAME.git"
    echo "  3. Execute: git push -u origin main"
    echo "  4. Execute: ./release.sh 1.0.0"
fi

log_success "Setup concluído! 🎉"