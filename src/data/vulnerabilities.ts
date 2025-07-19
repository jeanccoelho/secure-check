import { VulnerabilityDefinitions } from '../types';

export const vulnerabilityDefinitions: VulnerabilityDefinitions = {
  "windows": [
    {
      "id": "UAC_DISABLED",
      "name": "UAC Desativado",
      "description": "O Controle de Conta de Usuário está desativado.",
      "impact": "Permite execução de malware com privilégios elevados.",
      "severity": "Alta",
      "fix": "reg add HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System /v EnableLUA /t REG_DWORD /d 1 /f"
    },
    {
      "id": "FIREWALL_DISABLED",
      "name": "Firewall Desativado",
      "description": "O firewall está desativado.",
      "impact": "O sistema pode ser acessado por invasores externos.",
      "severity": "Alta",
      "fix": "netsh advfirewall set allprofiles state on"
    },
    {
      "id": "SMB1_ENABLED",
      "name": "SMBv1 Ativado",
      "description": "Protocolo legado SMBv1 está habilitado.",
      "impact": "Exploração por ransomwares como WannaCry.",
      "severity": "Alta",
      "fix": "Disable-WindowsOptionalFeature -Online -FeatureName SMB1Protocol"
    },
    {
      "id": "AUTORUN_ENABLED",
      "name": "AutoRun Habilitado",
      "description": "Mídias removíveis executam programas automaticamente.",
      "impact": "Execução automática de malware por USB.",
      "severity": "Média",
      "fix": "reg add HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer /v NoDriveTypeAutoRun /t REG_DWORD /d 255 /f"
    },
    {
      "id": "WINDOWS_UPDATE_DISABLED",
      "name": "Atualizações Desativadas",
      "description": "O serviço do Windows Update está desativado.",
      "impact": "O sistema não recebe correções de segurança.",
      "severity": "Alta",
      "fix": "sc config wuauserv start= auto && net start wuauserv"
    },
    {
      "id": "RDP_PORT_OPEN",
      "name": "Porta RDP Aberta",
      "description": "Porta 3389 está exposta à Internet.",
      "impact": "Ataques de força bruta no RDP.",
      "severity": "Alta",
      "fix": "netsh advfirewall firewall add rule name=\"Block RDP\" dir=in action=block protocol=TCP localport=3389"
    },
    {
      "id": "ADMIN_USER_DEFAULT",
      "name": "Conta Admin Padrão Ativa",
      "description": "A conta Administrator está ativa e sem senha.",
      "impact": "Invasores podem obter acesso total facilmente.",
      "severity": "Alta",
      "fix": "net user Administrator /active:no"
    },
    {
      "id": "MACROS_ENABLED",
      "name": "Macros do Office Ativadas",
      "description": "Execução automática de macros em arquivos do Office.",
      "impact": "Execução de scripts maliciosos.",
      "severity": "Alta",
      "fix": "Desativar macros via configuração do Office ou GPO"
    },
    {
      "id": "ANTIVIRUS_DISABLED",
      "name": "Antivírus Desativado",
      "description": "O Windows Defender está desativado.",
      "impact": "O sistema está vulnerável a malwares.",
      "severity": "Alta",
      "fix": "powershell Set-MpPreference -DisableRealtimeMonitoring 0"
    },
    {
      "id": "GUEST_ACCOUNT_ENABLED",
      "name": "Conta Convidado Ativa",
      "description": "A conta Guest está ativa.",
      "impact": "Acesso não autorizado ao sistema.",
      "severity": "Média",
      "fix": "net user Guest /active:no"
    }
  ],
  "linux": [
    {
      "id": "SSH_ROOT_LOGIN",
      "name": "SSH com Root Permitido",
      "description": "PermitRootLogin está ativado.",
      "impact": "Acesso remoto total com brute-force.",
      "severity": "Alta",
      "fix": "sed -i 's/^PermitRootLogin.*/PermitRootLogin no/' /etc/ssh/sshd_config && systemctl restart sshd"
    },
    {
      "id": "NO_FIREWALL",
      "name": "UFW não instalado",
      "description": "O firewall UFW não está instalado no sistema.",
      "impact": "Sistema sem proteção de firewall, todas as portas acessíveis.",
      "severity": "Alta",
      "fix": "apt update && apt install -y ufw"
    },
    {
      "id": "UFW_INACTIVE",
      "name": "UFW instalado mas inativo",
      "description": "O firewall UFW está instalado mas não está ativo.",
      "impact": "Firewall disponível mas não protegendo o sistema.",
      "severity": "Alta",
      "fix": "ufw --force enable"
    },
    {
      "id": "SUDO_NOPASSWD",
      "name": "Sudo sem senha",
      "description": "Usuários sudo não precisam de senha.",
      "impact": "Escalonamento fácil de privilégios.",
      "severity": "Alta",
      "fix": "Remover entradas com NOPASSWD de /etc/sudoers"
    },
    {
      "id": "OLD_KERNEL",
      "name": "Kernel Desatualizado",
      "description": "Versão do kernel não está atualizada.",
      "impact": "Explorações conhecidas estão disponíveis.",
      "severity": "Alta",
      "fix": "apt update && apt upgrade -y && reboot"
    },
    {
      "id": "FTP_ANON_ENABLED",
      "name": "FTP com Anônimo Permitido",
      "description": "Servidor FTP permite login anônimo.",
      "impact": "Exposição de arquivos.",
      "severity": "Alta",
      "fix": "Desabilitar anon_login no vsftpd.conf"
    }
  ],
  "macos": [
    {
      "id": "GATEKEEPER_OFF",
      "name": "Gatekeeper desativado",
      "description": "Gatekeeper permite apps de qualquer origem.",
      "impact": "Execução de software não verificado.",
      "severity": "Alta",
      "fix": "spctl --master-enable"
    },
    {
      "id": "FIREWALL_OFF",
      "name": "Firewall desativado",
      "description": "Firewall interno do macOS está desativado.",
      "impact": "Acesso remoto não controlado.",
      "severity": "Alta",
      "fix": "/usr/libexec/ApplicationFirewall/socketfilterfw --setglobalstate on"
    },
    {
      "id": "NO_FILEVAULT",
      "name": "FileVault desativado",
      "description": "Disco não está criptografado.",
      "impact": "Dados vulneráveis em caso de perda do equipamento.",
      "severity": "Alta",
      "fix": "Ativar via Preferências do Sistema > Segurança > FileVault"
    }
  ]
};