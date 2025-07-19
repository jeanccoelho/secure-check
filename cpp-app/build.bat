@echo off
REM Script de build para Windows
REM Uso: build.bat [clean|release|debug]

setlocal enabledelayedexpansion

set PROJECT_DIR=%~dp0
set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

echo 🛡️  SecurityChecker Build Script
echo ==================================
echo Diretório do projeto: %PROJECT_DIR%
echo Tipo de build: %BUILD_TYPE%
echo.

REM Verificar se é comando de limpeza
if "%1"=="clean" (
    echo 🧹 Limpando builds anteriores...
    if exist "%PROJECT_DIR%build-windows" rmdir /s /q "%PROJECT_DIR%build-windows"
    echo ✅ Build limpo
    goto :eof
)

REM Verificar dependências
echo 🔍 Verificando dependências...

where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo ❌ CMake não encontrado. Instale o CMake primeiro.
    echo    Download: https://cmake.org/download/
    pause
    exit /b 1
)

REM Tentar encontrar Qt6
set QT6_FOUND=0
for %%i in (
    "C:\Qt\6.5.0\msvc2019_64"
    "C:\Qt\6.6.0\msvc2019_64"
    "C:\Qt\6.7.0\msvc2019_64"
    "C:\Qt\Tools\QtCreator\bin"
) do (
    if exist "%%i\bin\qmake.exe" (
        set QT6_PATH=%%i
        set QT6_FOUND=1
        goto :qt6_found
    )
)

:qt6_found
if %QT6_FOUND%==0 (
    echo ❌ Qt6 não encontrado.
    echo    Instale o Qt6 de: https://www.qt.io/download
    echo    Ou defina a variável QT6_PATH manualmente
    pause
    exit /b 1
)

echo ✅ Qt6 encontrado em: %QT6_PATH%
echo ✅ Dependências verificadas

REM Criar diretório de build
set BUILD_DIR=%PROJECT_DIR%build-windows
echo.
echo 📁 Criando diretório de build: %BUILD_DIR%
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM Configurar CMake
echo.
echo ⚙️  Configurando projeto com CMake...
cmake -DCMAKE_BUILD_TYPE=%BUILD_TYPE% -DCMAKE_PREFIX_PATH="%QT6_PATH%" "%PROJECT_DIR%"
if %errorlevel% neq 0 (
    echo ❌ Erro na configuração do CMake
    pause
    exit /b 1
)

REM Compilar
echo.
echo 🔨 Compilando projeto...
cmake --build . --config %BUILD_TYPE%
if %errorlevel% neq 0 (
    echo ❌ Erro na compilação
    pause
    exit /b 1
)

echo.
echo ✅ Compilação concluída com sucesso!
echo.

REM Verificar se executável foi gerado
set EXE_PATH=%BUILD_DIR%\%BUILD_TYPE%\SecurityChecker.exe
if exist "%EXE_PATH%" (
    echo 📦 Executável gerado:
    echo    %EXE_PATH%
    echo.
    echo ℹ️  Informações do executável:
    dir "%EXE_PATH%"
) else (
    echo ❌ Executável não encontrado!
    pause
    exit /b 1
)

echo.
echo 🚀 Para executar:
echo    cd "%BUILD_DIR%\%BUILD_TYPE%"
echo    SecurityChecker.exe
echo.
echo ⚠️  Lembre-se: Execute como Administrador para funcionalidade completa!
echo    (Clique com botão direito → "Executar como administrador")
echo.