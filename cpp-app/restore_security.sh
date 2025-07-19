#!/bin/bash

# Script para restaurar a segurança após os testes
# Remove as vulnerabilidades criadas pelo simulate_vulnerabilities.sh

set -e

echo "🔒 Restaurador de Segurança Linux"
echo "================================="
echo ""
echo "📋 Restaurando configurações seguras..."
echo ""

# Função para mostrar progresso
show_progress() {
    echo "🔧 $1"
}

# 1. Restaurar SSH config se backup existir
show_progress "Restaurando configuração SSH..."
if [[ -f /etc/ssh/sshd_config.secure_backup ]]; then
    cp /etc/ssh/sshd_config.secure_backup /etc/ssh/sshd_config
    echo "   ✅ Configuração SSH restaurada do backup"
else
    # Configuração manual segura
    sed -i 's/^PermitRootLogin.*/PermitRootLogin no/' /etc/ssh/sshd_config
    sed -i 's/^Port 22$/Port 2222/' /etc/ssh/sshd_config
    if ! grep -q "^Port" /etc/ssh/sshd_config; then
        echo "Port 2222" >> /etc/ssh/sshd_config
    fi
    echo "   ✅ Configuração SSH corrigida manualmente"
fi

# 2. Instalar e configurar UFW
show_progress "Instalando e configurando UFW..."
apt update >/dev/null 2>&1
apt install -y ufw >/dev/null 2>&1
ufw --force enable >/dev/null 2>&1
echo "   ✅ UFW instalado e ativado"

# 3. Instalar e configurar Fail2Ban
show_progress "Instalando e configurando Fail2Ban..."
apt install -y fail2ban >/dev/null 2>&1
systemctl enable fail2ban >/dev/null 2>&1
systemctl start fail2ban >/dev/null 2>&1
echo "   ✅ Fail2Ban instalado e ativado"

# 4. Configurar atualizações automáticas
show_progress "Configurando atualizações automáticas..."
apt install -y unattended-upgrades >/dev/null 2>&1
echo 'unattended-upgrades unattended-upgrades/enable_auto_updates boolean true' | debconf-set-selections
dpkg-reconfigure -f noninteractive unattended-upgrades >/dev/null 2>&1
echo "   ✅ Atualizações automáticas configuradas"

# 5. Instalar e configurar AppArmor
show_progress "Instalando e configurando AppArmor..."
apt install -y apparmor apparmor-utils >/dev/null 2>&1
systemctl enable apparmor >/dev/null 2>&1
systemctl start apparmor >/dev/null 2>&1
echo "   ✅ AppArmor instalado e ativado"

# 6. Remover configuração NOPASSWD
show_progress "Removendo configuração sudo NOPASSWD..."
rm -f /etc/sudoers.d/99-test-nopasswd
echo "   ✅ Configuração sudo NOPASSWD removida"

# 7. Corrigir permissões de arquivos
show_progress "Corrigindo permissões de arquivos..."
chmod 644 /etc/passwd
chmod 600 /etc/shadow
chmod 644 /etc/group
chmod 600 /etc/gshadow
echo "   ✅ Permissões de arquivos corrigidas"

# 8. Atualizar sistema
show_progress "Atualizando sistema..."
apt update >/dev/null 2>&1
apt upgrade -y >/dev/null 2>&1
echo "   ✅ Sistema atualizado"

# 9. Reiniciar SSH
show_progress "Reiniciando serviço SSH..."
if systemctl is-active ssh >/dev/null 2>&1; then
    systemctl restart ssh
    echo "   ✅ SSH reiniciado"
elif systemctl is-active sshd >/dev/null 2>&1; then
    systemctl restart sshd
    echo "   ✅ SSHD reiniciado"
fi

echo ""
echo "🔒 Segurança restaurada com sucesso!"
echo ""
echo "📊 Configurações aplicadas:"
echo "   1. ✅ SSH root login desabilitado"
echo "   2. ✅ UFW instalado e ativado"
echo "   3. ✅ SSH movido para porta 2222"
echo "   4. ✅ Fail2Ban instalado e ativado"
echo "   5. ✅ Atualizações automáticas configuradas"
echo "   6. ✅ AppArmor instalado e ativado"
echo "   7. ✅ Sudo NOPASSWD removido"
echo "   8. ✅ Permissões de arquivos corrigidas"
echo "   9. ✅ Sistema atualizado"
echo ""
echo "🛡️  Seu sistema está agora protegido!"
echo ""
echo "⚠️  IMPORTANTE: Se você estava usando SSH na porta 22,"
echo "   agora precisa conectar na porta 2222:"
echo "   ssh -p 2222 usuario@servidor"