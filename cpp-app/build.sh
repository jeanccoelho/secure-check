#!/bin/bash

# Script de build para Linux/macOS
# Uso: ./build.sh [clean|release|debug]

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_TYPE="${1:-Release}"

echo "🛡️  SecurityChecker Build Script"
echo "=================================="
echo "Diretório do projeto: $PROJECT_DIR"
echo "Tipo de build: $BUILD_TYPE"
echo ""

# Detectar sistema operacional
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
    echo "🐧 Sistema detectado: Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
    echo "🍎 Sistema detectado: macOS"
else
    echo "❌ Sistema operacional não suportado: $OSTYPE"
    exit 1
fi

# Verificar dependências
echo ""
echo "🔍 Verificando dependências..."

if ! command -v cmake &> /dev/null; then
    echo "❌ CMake não encontrado. Instale o CMake primeiro."
    exit 1
fi

if ! command -v qmake6 &> /dev/null && ! command -v qmake &> /dev/null; then
    echo "❌ Qt6 não encontrado."
    if [[ "$OS" == "linux" ]]; then
        echo "   Instale com: sudo apt install qt6-base-dev qt6-tools-dev"
    elif [[ "$OS" == "macos" ]]; then
        echo "   Instale com: brew install qt6"
    fi
    exit 1
fi

echo "✅ Dependências verificadas"

# Limpar build anterior se solicitado
if [[ "$1" == "clean" ]]; then
    echo ""
    echo "🧹 Limpando builds anteriores..."
    rm -rf "$PROJECT_DIR/build-$OS"
    echo "✅ Build limpo"
    exit 0
fi

# Criar diretório de build
BUILD_DIR="$PROJECT_DIR/build-$OS"
echo ""
echo "📁 Criando diretório de build: $BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configurar CMake
echo ""
echo "⚙️  Configurando projeto com CMake..."

if [[ "$OS" == "macos" ]]; then
    # Para macOS, especificar caminho do Qt6
    QT6_PATH=$(brew --prefix qt6)
    cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" -DCMAKE_PREFIX_PATH="$QT6_PATH" "$PROJECT_DIR"
else
    # Para Linux
    cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" "$PROJECT_DIR"
fi

# Compilar
echo ""
echo "🔨 Compilando projeto..."
if [[ "$OS" == "macos" ]]; then
    make -j$(sysctl -n hw.ncpu)
else
    make -j$(nproc)
fi

echo ""
echo "✅ Compilação concluída com sucesso!"
echo ""
echo "📦 Executável gerado:"
if [[ -f "$BUILD_DIR/SecurityChecker" ]]; then
    echo "   $BUILD_DIR/SecurityChecker"
    
    # Mostrar informações do executável
    echo ""
    echo "ℹ️  Informações do executável:"
    ls -lh "$BUILD_DIR/SecurityChecker"
    file "$BUILD_DIR/SecurityChecker"
else
    echo "❌ Executável não encontrado!"
    exit 1
fi

echo ""
echo "🚀 Para executar:"
echo "   cd $BUILD_DIR"
echo "   sudo ./SecurityChecker"
echo ""
echo "⚠️  Lembre-se: Execute como administrador/root para funcionalidade completa!"