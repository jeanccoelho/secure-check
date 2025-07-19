#!/bin/bash

# Script para simular vulnerabilidades Linux para testar o SecurityChecker
# ATENÇÃO: Este script INTENCIONALMENTE torna o sistema menos seguro para fins de teste
# Use apenas em ambientes de teste/desenvolvimento!

set -e

echo "🔓 Simulador de Vulnerabilidades Linux"
echo "======================================"
echo ""
echo "⚠️  ATENÇÃO: Este script vai INTENCIONALMENTE tornar seu sistema menos seguro!"
echo "   Use apenas em ambientes de teste/desenvolvimento."
echo "   Faça backup das configurações antes de continuar."
echo ""

read -p "Tem certeza que deseja continuar? (digite 'SIM' para confirmar): " confirm
if [[ "$confirm" != "SIM" ]]; then
    echo "❌ Operação cancelada."
    exit 1
fi

echo ""
echo "📋 Criando vulnerabilidades para teste..."
echo ""

# Função para mostrar progresso
show_progress() {
    echo "🔧 $1"
}

# 1. SSH Root Login
show_progress "Habilitando SSH root login..."
if [[ -f /etc/ssh/sshd_config ]]; then
    cp /etc/ssh/sshd_config /etc/ssh/sshd_config.secure_backup
    sed -i 's/^#*PermitRootLogin.*/PermitRootLogin yes/' /etc/ssh/sshd_config
    if ! grep -q "^PermitRootLogin yes" /etc/ssh/sshd_config; then
        echo "PermitRootLogin yes" >> /etc/ssh/sshd_config
    fi
    echo "   ✅ SSH root login habilitado"
else
    echo "   ⚠️  SSH não encontrado"
fi

# 2. Desinstalar UFW (se instalado)
show_progress "Removendo UFW..."
if command -v ufw >/dev/null 2>&1; then
    ufw --force disable 2>/dev/null || true
    apt remove -y ufw 2>/dev/null || true
    echo "   ✅ UFW removido"
else
    echo "   ℹ️  UFW já não estava instalado"
fi

# 3. SSH na porta padrão
show_progress "Configurando SSH na porta padrão (22)..."
if [[ -f /etc/ssh/sshd_config ]]; then
    sed -i 's/^Port.*/Port 22/' /etc/ssh/sshd_config
    sed -i 's/^#Port.*/Port 22/' /etc/ssh/sshd_config
    if ! grep -q "^Port 22" /etc/ssh/sshd_config; then
        echo "Port 22" >> /etc/ssh/sshd_config
    fi
    echo "   ✅ SSH configurado na porta 22"
fi

# 4. Remover Fail2Ban
show_progress "Removendo Fail2Ban..."
if command -v fail2ban-server >/dev/null 2>&1; then
    systemctl stop fail2ban 2>/dev/null || true
    systemctl disable fail2ban 2>/dev/null || true
    apt remove -y fail2ban 2>/dev/null || true
    echo "   ✅ Fail2Ban removido"
else
    echo "   ℹ️  Fail2Ban já não estava instalado"
fi

# 5. Desabilitar atualizações automáticas
show_progress "Desabilitando atualizações automáticas..."
if dpkg -l | grep -q unattended-upgrades; then
    systemctl stop unattended-upgrades 2>/dev/null || true
    systemctl disable unattended-upgrades 2>/dev/null || true
    apt remove -y unattended-upgrades 2>/dev/null || true
    echo "   ✅ Atualizações automáticas desabilitadas"
else
    echo "   ℹ️  Atualizações automáticas já não estavam configuradas"
fi

# 6. Remover AppArmor
show_progress "Removendo AppArmor..."
if command -v apparmor_status >/dev/null 2>&1; then
    systemctl stop apparmor 2>/dev/null || true
    systemctl disable apparmor 2>/dev/null || true
    apt remove -y apparmor apparmor-utils 2>/dev/null || true
    echo "   ✅ AppArmor removido"
else
    echo "   ℹ️  AppArmor já não estava instalado"
fi

# 7. Adicionar configuração NOPASSWD no sudo
show_progress "Adicionando configuração sudo NOPASSWD..."
echo "# VULNERABILIDADE DE TESTE - REMOVER EM PRODUÇÃO" > /etc/sudoers.d/99-test-nopasswd
echo "ALL ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers.d/99-test-nopasswd
echo "   ✅ Sudo NOPASSWD configurado"

# 8. Alterar permissões de arquivos críticos
show_progress "Alterando permissões de arquivos críticos..."
chmod 666 /etc/passwd 2>/dev/null || true
chmod 644 /etc/shadow 2>/dev/null || true
chmod 666 /etc/group 2>/dev/null || true
chmod 644 /etc/gshadow 2>/dev/null || true
echo "   ✅ Permissões alteradas para valores inseguros"

# 9. Reiniciar SSH para aplicar mudanças
show_progress "Reiniciando serviço SSH..."
if systemctl is-active ssh >/dev/null 2>&1; then
    systemctl restart ssh
    echo "   ✅ SSH reiniciado"
elif systemctl is-active sshd >/dev/null 2>&1; then
    systemctl restart sshd
    echo "   ✅ SSHD reiniciado"
else
    echo "   ⚠️  Serviço SSH não encontrado"
fi

echo ""
echo "🔓 Vulnerabilidades simuladas com sucesso!"
echo ""
echo "📊 Resumo das vulnerabilidades criadas:"
echo "   1. ✅ SSH root login habilitado"
echo "   2. ✅ UFW removido/desabilitado"
echo "   3. ✅ SSH na porta padrão (22)"
echo "   4. ✅ Fail2Ban removido"
echo "   5. ✅ Atualizações automáticas desabilitadas"
echo "   6. ✅ AppArmor removido"
echo "   7. ✅ Sudo NOPASSWD configurado"
echo "   8. ✅ Permissões de arquivos alteradas"
echo ""
echo "🛡️  Agora execute o SecurityChecker para testar:"
echo "   sudo ./build-linux/SecurityChecker"
echo ""
echo "🔒 Para restaurar a segurança depois dos testes:"
echo "   ./restore_security.sh"
echo ""
echo "⚠️  IMPORTANTE: Este sistema está agora INTENCIONALMENTE inseguro!"
echo "   Use apenas para testes e restaure a segurança depois."