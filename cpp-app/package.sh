#!/bin/bash

# Script para empacotar executáveis para distribuição
# Uso: ./package.sh [version]

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VERSION="${1:-1.0.0}"

echo "📦 SecurityChecker Package Script"
echo "================================="
echo "Versão: $VERSION"
echo ""

# Detectar sistema operacional
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
    ARCH="x64"
    EXT=""
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    ARCH="x64"
    EXT=""
else
    echo "❌ Sistema operacional não suportado para este script"
    exit 1
fi

BUILD_DIR="$PROJECT_DIR/build-$OS"
PACKAGE_DIR="$PROJECT_DIR/packages"
DIST_DIR="$PACKAGE_DIR/SecurityChecker-$VERSION-$OS-$ARCH"

# Verificar se executável existe
EXECUTABLE="$BUILD_DIR/SecurityChecker$EXT"
if [[ ! -f "$EXECUTABLE" ]]; then
    echo "❌ Executável não encontrado: $EXECUTABLE"
    echo "   Execute primeiro: ./build.sh"
    exit 1
fi

echo "🔍 Executável encontrado: $EXECUTABLE"

# Criar diretório de distribuição
echo "📁 Criando pacote em: $DIST_DIR"
rm -rf "$DIST_DIR"
mkdir -p "$DIST_DIR"

# Copiar executável
echo "📋 Copiando executável..."
cp "$EXECUTABLE" "$DIST_DIR/"

# Copiar arquivos de dados
echo "📋 Copiando arquivos de dados..."
cp "$PROJECT_DIR/data/vulnerabilities.json" "$DIST_DIR/"

# Copiar documentação
echo "📋 Copiando documentação..."
cp "$PROJECT_DIR/README.md" "$DIST_DIR/"

# Criar script de execução
echo "📋 Criando script de execução..."
if [[ "$OS" == "linux" ]]; then
    cat > "$DIST_DIR/run.sh" << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
echo "🛡️  SecurityChecker"
echo "=================="
echo ""
if [[ $EUID -ne 0 ]]; then
    echo "⚠️  Este programa precisa ser executado como root."
    echo "   Execute: sudo ./run.sh"
    exit 1
fi
./SecurityChecker
EOF
    chmod +x "$DIST_DIR/run.sh"
elif [[ "$OS" == "macos" ]]; then
    cat > "$DIST_DIR/run.sh" << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
echo "🛡️  SecurityChecker"
echo "=================="
echo ""
if [[ $EUID -ne 0 ]]; then
    echo "⚠️  Este programa precisa ser executado como root."
    echo "   Execute: sudo ./run.sh"
    exit 1
fi
./SecurityChecker
EOF
    chmod +x "$DIST_DIR/run.sh"
fi

# Criar arquivo de informações
echo "📋 Criando arquivo de informações..."
cat > "$DIST_DIR/INFO.txt" << EOF
SecurityChecker v$VERSION
========================

Sistema: $OS $ARCH
Data de build: $(date)

INSTRUÇÕES DE USO:
==================

1. Execute como administrador/root:
   - Linux/macOS: sudo ./run.sh
   - Ou diretamente: sudo ./SecurityChecker

2. O programa irá:
   - Detectar automaticamente seu sistema operacional
   - Verificar vulnerabilidades conhecidas
   - Oferecer correções automáticas

ARQUIVOS INCLUSOS:
==================
- SecurityChecker: Executável principal
- vulnerabilities.json: Definições de vulnerabilidades
- run.sh: Script de execução (Linux/macOS)
- README.md: Documentação completa
- INFO.txt: Este arquivo

REQUISITOS:
===========
- Privilégios de administrador/root
- Sistema operacional suportado (Windows/Linux/macOS)

SUPORTE:
========
- GitHub: https://github.com/seu-usuario/securitychecker
- Issues: https://github.com/seu-usuario/securitychecker/issues

LICENÇA:
========
MIT License - Veja README.md para detalhes completos.
EOF

# Verificar dependências dinâmicas (Linux)
if [[ "$OS" == "linux" ]]; then
    echo "🔍 Verificando dependências dinâmicas..."
    echo "Dependências do executável:" > "$DIST_DIR/DEPENDENCIES.txt"
    ldd "$EXECUTABLE" >> "$DIST_DIR/DEPENDENCIES.txt" 2>/dev/null || echo "Não foi possível listar dependências" >> "$DIST_DIR/DEPENDENCIES.txt"
fi

# Criar arquivo compactado
echo "🗜️  Criando arquivo compactado..."
cd "$PACKAGE_DIR"
ARCHIVE_NAME="SecurityChecker-$VERSION-$OS-$ARCH.tar.gz"
tar -czf "$ARCHIVE_NAME" "SecurityChecker-$VERSION-$OS-$ARCH"

echo ""
echo "✅ Pacote criado com sucesso!"
echo ""
echo "📦 Arquivos gerados:"
echo "   Diretório: $DIST_DIR"
echo "   Arquivo:   $PACKAGE_DIR/$ARCHIVE_NAME"
echo ""
echo "📊 Tamanho do pacote:"
ls -lh "$PACKAGE_DIR/$ARCHIVE_NAME"
echo ""
echo "🚀 Para distribuir:"
echo "   1. Teste o pacote em uma máquina limpa"
echo "   2. Faça upload para GitHub Releases"
echo "   3. Documente os requisitos do sistema"