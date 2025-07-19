#!/bin/bash

# Script para build em múltiplas plataformas usando Docker
# Uso: ./build-all.sh [version]

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VERSION="${1:-1.0.0}"

echo "🏗️  SecurityChecker Multi-Platform Build"
echo "========================================"
echo "Versão: $VERSION"
echo ""

# Verificar se Docker está disponível
if ! command -v docker &> /dev/null; then
    echo "❌ Docker não encontrado. Este script requer Docker para builds multiplataforma."
    echo "   Instale Docker de: https://docs.docker.com/get-docker/"
    exit 1
fi

echo "✅ Docker encontrado"

# Criar Dockerfile para Linux
echo "🐧 Preparando build para Linux..."
cat > "$PROJECT_DIR/Dockerfile.linux" << 'EOF'
FROM ubuntu:22.04

# Instalar dependências
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-tools-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Build
RUN mkdir -p build-linux && cd build-linux && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    make -j$(nproc)

# Criar pacote
RUN mkdir -p /output && \
    cp build-linux/SecurityChecker /output/ && \
    cp data/vulnerabilities.json /output/ && \
    cp README.md /output/

CMD ["tar", "-czf", "/output/SecurityChecker-linux-x64.tar.gz", "-C", "/output", "."]
EOF

# Build Linux usando Docker
echo "🔨 Compilando para Linux..."

# Limpar cache anterior se existir
if [[ -d "$PROJECT_DIR/build-linux" ]]; then
    echo "🧹 Limpando cache anterior..."
    rm -rf "$PROJECT_DIR/build-linux"
fi

docker build -f "$PROJECT_DIR/Dockerfile.linux" -t securitychecker-linux "$PROJECT_DIR"

# Extrair resultado
mkdir -p "$PROJECT_DIR/packages"
docker run --rm -v "$PROJECT_DIR/packages:/output" securitychecker-linux

echo "✅ Build Linux concluído"

# Limpar Dockerfile temporário
rm "$PROJECT_DIR/Dockerfile.linux"

# Build local (se estivermos em macOS)
if [[ "$OSTYPE" == "darwin"* ]]; then
    echo ""
    echo "🍎 Compilando para macOS (local)..."
    cd "$PROJECT_DIR"
    ./build.sh release
    ./package.sh "$VERSION"
    echo "✅ Build macOS concluído"
fi

echo ""
echo "🎉 Build multiplataforma concluído!"
echo ""
echo "📦 Pacotes gerados:"
ls -la "$PROJECT_DIR/packages/"*.tar.gz 2>/dev/null || echo "   Nenhum pacote .tar.gz encontrado"

echo ""
echo "📋 Próximos passos:"
echo "   1. Teste os executáveis em máquinas limpas"
echo "   2. Faça upload para GitHub Releases"
echo "   3. Atualize a documentação com links de download"
echo "   4. Para Windows, compile manualmente ou use GitHub Actions"