#include "SystemChecker.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QRandomGenerator>

#ifdef _WIN32
#include <windows.h>
#include <lmcons.h>
#else
#include <unistd.h>
#include <sys/types.h>
#endif

SystemChecker::SystemChecker(QObject *parent)
    : QObject(parent)
    , m_checkProcess(nullptr)
    , m_fixProcess(nullptr)
    , m_infoProcess(nullptr)
    , m_currentInfoCommandIndex(0)
{
}

void SystemChecker::checkVulnerability(const VulnerabilityDefinition &vuln)
{
    if (m_checkProcess && m_checkProcess->state() != QProcess::NotRunning) {
        emit errorOccurred("Uma verificação já está em andamento");
        return;
    }
    
    m_currentCheckId = vuln.id;
    
    if (!m_checkProcess) {
        m_checkProcess = new QProcess(this);
        connect(m_checkProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &SystemChecker::onCheckProcessFinished);
    }
    
    QString command = getCheckCommand(vuln);
    if (command.isEmpty()) {
        emit errorOccurred("Comando de verificação não implementado para esta vulnerabilidade");
        return;
    }
    
    if (!executeCommand(command, m_checkProcess)) {
        emit errorOccurred("Falha ao executar comando de verificação");
    }
}

void SystemChecker::fixVulnerability(const VulnerabilityDefinition &vuln)
{
    if (m_fixProcess && m_fixProcess->state() != QProcess::NotRunning) {
        emit errorOccurred("Uma correção já está em andamento");
        return;
    }
    
    m_currentFixId = vuln.id;
    
    if (!m_fixProcess) {
        m_fixProcess = new QProcess(this);
        connect(m_fixProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &SystemChecker::onFixProcessFinished);
    }
    
    QString command = getFixCommand(vuln);
    if (command.isEmpty()) {
        emit errorOccurred("Comando de correção não implementado para esta vulnerabilidade");
        return;
    }
    
    if (!executeCommand(command, m_fixProcess)) {
        emit errorOccurred("Falha ao executar comando de correção");
    }
}

bool SystemChecker::isRunningAsAdmin() const
{
#ifdef Q_OS_WIN
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    
    return isAdmin == TRUE;
#else
    return geteuid() == 0;
#endif
}

void SystemChecker::onCheckProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        emit errorOccurred("Processo de verificação falhou");
        return;
    }
    
    // Interpretar código de saída:
    // 0 = comando encontrou algo (vulnerável)
    // 1 = comando não encontrou nada (seguro)
    bool isVulnerable = (exitCode == 0);
    emit checkCompleted(m_currentCheckId, isVulnerable);
}

void SystemChecker::onFixProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitStatus == QProcess::CrashExit) {
        emit errorOccurred("Processo de correção falhou");
        return;
    }
    
    // 0 = sucesso, outros = falha
    bool success = (exitCode == 0);
    emit fixCompleted(m_currentFixId, success);
}

QString SystemChecker::getCheckCommand(const VulnerabilityDefinition &vuln) const
{
    // Comandos reais de verificação para cada vulnerabilidade
    
#ifdef Q_OS_WIN
    if (vuln.id == "UAC_DISABLED") {
        return "reg query \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v EnableLUA | findstr \"0x0\"";
    }
    else if (vuln.id == "FIREWALL_DISABLED") {
        return "netsh advfirewall show allprofiles state | findstr \"OFF\"";
    }
    else if (vuln.id == "SMB1_ENABLED") {
        return "powershell \"Get-WindowsOptionalFeature -Online -FeatureName SMB1Protocol | Where-Object {$_.State -eq 'Enabled'}\"";
    }
    else if (vuln.id == "AUTORUN_ENABLED") {
        return "reg query \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\" /v NoDriveTypeAutoRun | findstr \"0x\"";
    }
    else if (vuln.id == "WINDOWS_UPDATE_DISABLED") {
        return "sc query wuauserv | findstr \"STOPPED\"";
    }
    else if (vuln.id == "RDP_PORT_OPEN") {
        return "netstat -an | findstr \":3389\"";
    }
    else if (vuln.id == "ADMIN_USER_DEFAULT") {
        return "net user Administrator | findstr \"Account active.*Yes\"";
    }
    else if (vuln.id == "MACROS_ENABLED") {
        return "reg query \"HKCU\\Software\\Microsoft\\Office\\16.0\\Excel\\Security\" /v VBAWarnings | findstr \"0x1\"";
    }
    else if (vuln.id == "ANTIVIRUS_DISABLED") {
        return "powershell \"Get-MpPreference | Where-Object {$_.DisableRealtimeMonitoring -eq $true}\"";
    }
    else if (vuln.id == "GUEST_ACCOUNT_ENABLED") {
        return "net user Guest | findstr \"Account active.*Yes\"";
    }
#elif defined(Q_OS_LINUX)
    if (vuln.id == "SSH_ROOT_LOGIN") {
        return "grep -i '^PermitRootLogin yes' /etc/ssh/sshd_config";
    }
    else if (vuln.id == "NO_FIREWALL") {
        // Verificar se UFW não está instalado
        return "! command -v ufw >/dev/null 2>&1";
    }
    else if (vuln.id == "UFW_INACTIVE") {
        // Verificar se UFW está instalado mas inativo
        return "command -v ufw >/dev/null 2>&1 && ufw status | grep -i 'Status: inactive'";
    }
    else if (vuln.id == "SSH_DEFAULT_PORT") {
        return "grep -q '^Port 22$' /etc/ssh/sshd_config 2>/dev/null || (! grep -q '^Port' /etc/ssh/sshd_config 2>/dev/null && test -f /etc/ssh/sshd_config)";
    }
    else if (vuln.id == "FAIL2BAN_NOT_INSTALLED") {
        return "! command -v fail2ban-server >/dev/null 2>&1";
    }
    else if (vuln.id == "FAIL2BAN_INACTIVE") {
        return "command -v fail2ban-server >/dev/null 2>&1 && ! systemctl is-active fail2ban >/dev/null 2>&1";
    }
    else if (vuln.id == "UNATTENDED_UPGRADES_OFF") {
        return "! dpkg -l | grep -q unattended-upgrades || ! systemctl is-enabled unattended-upgrades >/dev/null 2>&1";
    }
    else if (vuln.id == "APPARMOR_NOT_INSTALLED") {
        return "! command -v apparmor_status >/dev/null 2>&1";
    }
    else if (vuln.id == "APPARMOR_INACTIVE") {
        return "command -v apparmor_status >/dev/null 2>&1 && ! systemctl is-active apparmor >/dev/null 2>&1";
    }
    else if (vuln.id == "SUDO_NOPASSWD") {
        return "grep -r 'NOPASSWD' /etc/sudoers /etc/sudoers.d/ 2>/dev/null";
    }
    else if (vuln.id == "WEAK_FILE_PERMS") {
        return "stat -c '%a' /etc/passwd | grep -v '^644$' || stat -c '%a' /etc/shadow | grep -v '^600$' || stat -c '%a' /etc/group | grep -v '^644$' || stat -c '%a' /etc/gshadow | grep -v '^600$'";
    }
    else if (vuln.id == "OLD_KERNEL") {
        return "apt list --upgradable 2>/dev/null | grep linux-image";
    }
#elif defined(Q_OS_MACOS)
    if (vuln.id == "GATEKEEPER_OFF") {
        return "spctl --status | grep 'assessments disabled'";
    }
    else if (vuln.id == "FIREWALL_OFF") {
        return "/usr/libexec/ApplicationFirewall/socketfilterfw --getglobalstate | grep 'Firewall is disabled'";
    }
    else if (vuln.id == "NO_FILEVAULT") {
        return "fdesetup status | grep 'FileVault is Off'";
    }
    else if (vuln.id == "AUTOMOUNT_USB") {
        return "defaults read /Library/Preferences/SystemConfiguration/autodiskmount AutomountDisksWithoutUserLogin | grep 1";
    }
    else if (vuln.id == "ICLOUD_BACKUP_ON") {
        return "defaults read ~/Library/Preferences/MobileMeAccounts Accounts | grep Documents";
    }
    else if (vuln.id == "UNKNOWN_EXTENSIONS_ENABLED") {
        return "spctl --list | grep 'unknown'";
    }
    else if (vuln.id == "NO_PASSWORD_SLEEP") {
        return "defaults read com.apple.screensaver askForPassword | grep 0";
    }
    else if (vuln.id == "SHARING_SERVICES_ON") {
        return "launchctl list | grep com.apple.sharing";
    }
    else if (vuln.id == "SIP_DISABLED") {
        return "csrutil status | grep disabled";
    }
    else if (vuln.id == "REMOTE_LOGIN_ENABLED") {
        return "systemsetup -getremotelogin | grep On";
    }
#endif
    
    return QString(); // Comando não implementado
}

QString SystemChecker::getFixCommand(const VulnerabilityDefinition &vuln) const
{
    // Comandos de correção específicos e inteligentes
    
#ifdef Q_OS_LINUX
    if (vuln.id == "NO_FIREWALL") {
        // Instalar UFW
        return "echo 'Instalando UFW...' && apt update && apt install -y ufw && echo 'UFW instalado com sucesso!'";
    }
    else if (vuln.id == "UFW_INACTIVE") {
        // Ativar UFW
        return "echo 'Ativando UFW...' && ufw --force enable && echo 'UFW ativado com sucesso!'";
    }
    else if (vuln.id == "SSH_DEFAULT_PORT") {
        return "echo 'Configurando SSH na porta 2222...' && "
               "cp /etc/ssh/sshd_config /etc/ssh/sshd_config.backup && "
               "if grep -q '^Port' /etc/ssh/sshd_config; then "
               "  sed -i 's/^Port.*/Port 2222/' /etc/ssh/sshd_config; "
               "else "
               "  echo 'Port 2222' >> /etc/ssh/sshd_config; "
               "fi && "
               "sshd -t && "
               "systemctl restart sshd && "
               "echo 'SSH configurado na porta 2222 com sucesso!'";
    }
    else if (vuln.id == "FAIL2BAN_NOT_INSTALLED") {
        return "echo 'Instalando Fail2Ban...' && apt update && apt install -y fail2ban && echo 'Fail2Ban instalado com sucesso!'";
    }
    else if (vuln.id == "FAIL2BAN_INACTIVE") {
        return "echo 'Ativando Fail2Ban...' && systemctl enable fail2ban && systemctl start fail2ban && echo 'Fail2Ban ativado com sucesso!'";
    }
    else if (vuln.id == "UNATTENDED_UPGRADES_OFF") {
        return "echo 'Configurando atualizações automáticas...' && apt update && apt install -y unattended-upgrades && echo 'unattended-upgrades unattended-upgrades/enable_auto_updates boolean true' | debconf-set-selections && dpkg-reconfigure -f noninteractive unattended-upgrades && echo 'Atualizações automáticas configuradas!'";
    }
    else if (vuln.id == "APPARMOR_NOT_INSTALLED") {
        return "echo 'Instalando AppArmor...' && apt update && apt install -y apparmor apparmor-utils && echo 'AppArmor instalado com sucesso!'";
    }
    else if (vuln.id == "APPARMOR_INACTIVE") {
        return "echo 'Ativando AppArmor...' && systemctl enable apparmor && systemctl start apparmor && echo 'AppArmor ativado com sucesso!'";
    }
    else if (vuln.id == "SUDO_NOPASSWD") {
        return "echo 'Removendo configurações NOPASSWD...' && sed -i '/NOPASSWD/d' /etc/sudoers && find /etc/sudoers.d/ -type f -exec sed -i '/NOPASSWD/d' {} \\; && echo 'Configurações NOPASSWD removidas!'";
    }
    else if (vuln.id == "WEAK_FILE_PERMS") {
        return "echo 'Corrigindo permissões de arquivos...' && chmod 644 /etc/passwd && chmod 600 /etc/shadow && chmod 644 /etc/group && chmod 600 /etc/gshadow && echo 'Permissões corrigidas!'";
    }
    else if (vuln.id == "OLD_KERNEL") {
        return "echo 'Atualizando sistema...' && apt update && apt upgrade -y && echo 'Sistema atualizado! Reinicie quando possível.'";
    }
#endif
    
    // Para outras vulnerabilidades, usar comando da definição
    return vuln.fix;
}

bool SystemChecker::executeCommand(const QString &command, QProcess *process)
{
    if (command.isEmpty() || !process) {
        return false;
    }
    
    qDebug() << "Executando comando:" << command;
    
#ifdef Q_OS_WIN
    if (command.startsWith("powershell")) {
        QString psCommand = command.mid(11); // Remove "powershell "
        process->start("powershell.exe", QStringList() << "-Command" << psCommand);
    } else {
        process->start("cmd.exe", QStringList() << "/c" << command);
    }
#else
    process->start("/bin/bash", QStringList() << "-c" << command);
#endif
    
    return process->waitForStarted();
}

SystemInfo SystemChecker::collectSystemInfo() const
{
    SystemInfo info;
    
    // Informações básicas do sistema
    info.osType = QSysInfo::kernelType();
    info.osVersion = QSysInfo::productVersion();
    info.kernelVersion = QSysInfo::kernelVersion();
    info.architecture = QSysInfo::currentCpuArchitecture();
    
    // Executar comandos síncronos para coleta rápida
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    
#ifdef Q_OS_WIN
    // Windows - Serviços em execução
    process.start("sc", QStringList() << "query" << "state=" << "all");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.contains("SERVICE_NAME:")) {
                info.runningServices.append(line.trimmed());
            }
        }
    }
    
    // Portas abertas
    process.start("netstat", QStringList() << "-an");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.contains("LISTENING") || line.contains("ESTABLISHED")) {
                info.networkPorts.append(line.trimmed());
            }
        }
    }
    
    // Contas de usuário
    process.start("net", QStringList() << "user");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        info.userAccounts = output.split('\n');
    }
    
#elif defined(Q_OS_LINUX)
    // Linux - Serviços em execução
    process.start("systemctl", QStringList() << "list-units" << "--type=service" << "--state=running" << "--no-pager");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.contains(".service") && line.contains("running")) {
                info.runningServices.append(line.trimmed());
            }
        }
    }
    
    // Portas abertas
    process.start("ss", QStringList() << "-tuln");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        info.networkPorts = output.split('\n');
    }
    
    // Contas de usuário
    process.start("cat", QStringList() << "/etc/passwd");
    if (process.waitForFinished(3000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (!line.isEmpty() && !line.startsWith("#")) {
                QStringList parts = line.split(':');
                if (parts.size() >= 3) {
                    info.userAccounts.append(QString("%1 (UID: %2)").arg(parts[0], parts[2]));
                }
            }
        }
    }
    
    // Configurações de segurança importantes
    QStringList securityConfigs;
    
    // SSH config
    process.start("cat", QStringList() << "/etc/ssh/sshd_config");
    if (process.waitForFinished(3000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.contains("PermitRootLogin") || line.contains("Port") || 
                line.contains("PasswordAuthentication") || line.contains("PubkeyAuthentication")) {
                securityConfigs.append("SSH: " + line.trimmed());
            }
        }
    }
    
    // Firewall status
    process.start("ufw", QStringList() << "status");
    if (process.waitForFinished(3000)) {
        QString output = process.readAllStandardOutput();
        securityConfigs.append("UFW: " + output.split('\n').first());
    }
    
    info.systemConfigs = securityConfigs;
    
#elif defined(Q_OS_MACOS)
    // macOS - Serviços em execução
    process.start("launchctl", QStringList() << "list");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (!line.isEmpty() && !line.startsWith("-")) {
                info.runningServices.append(line.trimmed());
            }
        }
    }
    
    // Portas abertas
    process.start("netstat", QStringList() << "-an");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.contains("LISTEN")) {
                info.networkPorts.append(line.trimmed());
            }
        }
    }
    
    // Contas de usuário
    process.start("dscl", QStringList() << "." << "-list" << "/Users");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        info.userAccounts = output.split('\n');
    }
#endif
    
    // Informações adicionais do sistema
    QStringList additionalInfo;
    additionalInfo.append(QString("Hostname: %1").arg(QSysInfo::machineHostName()));
    additionalInfo.append(QString("CPU Architecture: %1").arg(QSysInfo::currentCpuArchitecture()));
    additionalInfo.append(QString("Kernel Type: %1").arg(QSysInfo::kernelType()));
    additionalInfo.append(QString("Product Type: %1").arg(QSysInfo::productType()));
    
    info.additionalInfo = additionalInfo.join("\n");
    
    return info;
}

void SystemChecker::collectSystemInfoAsync()
{
    // Coletar informações básicas primeiro
    m_systemInfo = SystemInfo();
    m_systemInfo.osType = QSysInfo::kernelType();
    m_systemInfo.osVersion = QSysInfo::productVersion();
    m_systemInfo.kernelVersion = QSysInfo::kernelVersion();
    m_systemInfo.architecture = QSysInfo::currentCpuArchitecture();
    
    // Preparar comandos para execução assíncrona
    m_pendingInfoCommands = getSystemInfoCommands();
    m_currentInfoCommandIndex = 0;
    
    if (!m_infoProcess) {
        m_infoProcess = new QProcess(this);
        connect(m_infoProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
                this, &SystemChecker::onSystemInfoProcessFinished);
    }
    
    executeNextInfoCommand();
}

QStringList SystemChecker::getSystemInfoCommands() const
{
    QStringList commands;
    
#ifdef Q_OS_WIN
    commands << "sc query state= all"
             << "netstat -an"
             << "net user"
             << "wmic service get name,state,startmode"
             << "tasklist /svc";
#elif defined(Q_OS_LINUX)
    commands << "systemctl list-units --type=service --state=running --no-pager"
             << "ss -tuln"
             << "cat /etc/passwd"
             << "cat /etc/ssh/sshd_config"
             << "ufw status"
             << "ps aux"
             << "netstat -tuln";
#elif defined(Q_OS_MACOS)
    commands << "launchctl list"
             << "netstat -an"
             << "dscl . -list /Users"
             << "ps aux"
             << "spctl --status";
#endif
    
    return commands;
}

void SystemChecker::executeNextInfoCommand()
{
    if (m_currentInfoCommandIndex >= m_pendingInfoCommands.size()) {
        // Finalizar coleta
        QStringList additionalInfo;
        additionalInfo.append(QString("Hostname: %1").arg(QSysInfo::machineHostName()));
        additionalInfo.append(QString("CPU Architecture: %1").arg(QSysInfo::currentCpuArchitecture()));
        additionalInfo.append(QString("Kernel Type: %1").arg(QSysInfo::kernelType()));
        additionalInfo.append(QString("Product Type: %1").arg(QSysInfo::productType()));
        
        m_systemInfo.additionalInfo = additionalInfo.join("\n");
        
        emit systemInfoCollected(m_systemInfo);
        return;
    }
    
    QString command = m_pendingInfoCommands[m_currentInfoCommandIndex];
    executeCommand(command, m_infoProcess);
}

void SystemChecker::onSystemInfoProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (m_currentInfoCommandIndex < m_pendingInfoCommands.size()) {
        QString command = m_pendingInfoCommands[m_currentInfoCommandIndex];
        QString output = m_infoProcess->readAllStandardOutput();
        
        processSystemInfoCommand(command, output);
        
        m_currentInfoCommandIndex++;
        executeNextInfoCommand();
    }
}

void SystemChecker::processSystemInfoCommand(const QString &command, const QString &output)
{
    QStringList lines = output.split('\n');
    
    if (command.contains("systemctl") || command.contains("sc query") || command.contains("launchctl")) {
        // Serviços em execução
        for (const QString &line : lines) {
            if (!line.trimmed().isEmpty()) {
                m_systemInfo.runningServices.append(line.trimmed());
            }
        }
    } else if (command.contains("netstat") || command.contains("ss")) {
        // Portas de rede
        for (const QString &line : lines) {
            if (line.contains("LISTEN") || line.contains("LISTENING") || line.contains("ESTABLISHED")) {
                m_systemInfo.networkPorts.append(line.trimmed());
            }
        }
    } else if (command.contains("passwd") || command.contains("net user") || command.contains("dscl")) {
        // Contas de usuário
        for (const QString &line : lines) {
            if (!line.trimmed().isEmpty() && !line.startsWith("#")) {
                m_systemInfo.userAccounts.append(line.trimmed());
            }
        }
    } else if (command.contains("ssh") || command.contains("ufw") || command.contains("spctl")) {
        // Configurações de segurança
        for (const QString &line : lines) {
            if (!line.trimmed().isEmpty()) {
                m_systemInfo.systemConfigs.append(line.trimmed());
            }
        }
    }
}