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

# Evitar prompts interativos
ENV DEBIAN_FRONTEND=noninteractive

# Instalar dependências
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-tools-dev \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    libxcb1-dev \
    libxcb-xkb-dev \
    libxcb-icccm4-dev \
    libxcb-image0-dev \
    libxcb-keysyms1-dev \
    libxcb-randr0-dev \
    libxcb-render-util0-dev \
    libxcb-shape0-dev \
    libxcb-sync-dev \
    libxcb-xfixes0-dev \
    libxcb-xinerama0-dev \
    libfontconfig1-dev \
    libfreetype6-dev \
    libx11-dev \
    libx11-xcb-dev \
    libxext-dev \
    libxfixes-dev \
    libxi-dev \
    libxrender-dev \
    libxrandr-dev \
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

# Criar diretório de saída
mkdir -p "$PROJECT_DIR/packages"

# Build com output direto para diretório local
DOCKER_BUILDKIT=1 docker build \
    --output type=local,dest="$PROJECT_DIR/packages/temp" \
    -f "$PROJECT_DIR/Dockerfile.linux" \
    "$PROJECT_DIR"

if [ $? -eq 0 ]; then
    echo "   ✅ Build e extração concluídos com sucesso"
    
    # Mover apenas os arquivos necessários
    echo "📦 Organizando arquivos..."
    mkdir -p "$PROJECT_DIR/packages/SecurityChecker-$VERSION-linux-x64"
    
    if [[ -d "$PROJECT_DIR/packages/temp/output" ]]; then
        cp "$PROJECT_DIR/packages/temp/output/"* "$PROJECT_DIR/packages/SecurityChecker-$VERSION-linux-x64/" 2>/dev/null || true
    fi
    
    # Criar script de execução
    cat > "$PROJECT_DIR/packages/SecurityChecker-$VERSION-linux-x64/run.sh" << 'EOF'
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
    
    chmod +x "$PROJECT_DIR/packages/SecurityChecker-$VERSION-linux-x64/run.sh"
    chmod +x "$PROJECT_DIR/packages/SecurityChecker-$VERSION-linux-x64/SecurityChecker" 2>/dev/null || true
    
    # Criar pacote tar.gz
    cd "$PROJECT_DIR/packages"
    tar -czf "SecurityChecker-$VERSION-linux-x64.tar.gz" "SecurityChecker-$VERSION-linux-x64"
    
    # Limpar arquivos temporários
    rm -rf temp
    
    echo "   ✅ Pacote criado: SecurityChecker-$VERSION-linux-x64.tar.gz"
else
    echo "   ❌ Erro no build Docker"
fi

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
ls -la "$PROJECT_DIR/packages/" 2>/dev/null || echo "   Nenhum arquivo encontrado"

# Criar pacote tar.gz se arquivos foram extraídos
if [[ -f "$PROJECT_DIR/packages/SecurityChecker" ]]; then
    echo ""
    echo "📦 Criando pacote tar.gz..."
    cd "$PROJECT_DIR/packages"
    tar -czf "SecurityChecker-$VERSION-linux-x64.tar.gz" SecurityChecker vulnerabilities.json README.md
    echo "✅ Pacote criado: SecurityChecker-$VERSION-linux-x64.tar.gz"
fi

echo ""
echo "📋 Próximos passos:"
echo "   1. Teste os executáveis em máquinas limpas"
echo "   2. Faça upload para GitHub Releases"
echo "   3. Atualize a documentação com links de download"
echo "   4. Para Windows, compile manualmente ou use GitHub Actions"