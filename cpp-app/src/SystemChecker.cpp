#include "SystemChecker.h"
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

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
        // Simular verificação para comandos não implementados
        QTimer::singleShot(1000, [this, vuln]() {
            // Simular resultado aleatório para demonstração
            bool isVulnerable = (qrand() % 100) > 40; // 60% chance de ser vulnerável
            emit checkCompleted(vuln.id, isVulnerable);
        });
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
        // Simular correção para comandos não implementados
        QTimer::singleShot(2000, [this, vuln]() {
            emit fixCompleted(vuln.id, true);
        });
        return;
    }
    
    if (!executeCommand(command, m_fixProcess)) {
        emit errorOccurred("Falha ao executar comando de correção");
    }
}

bool SystemChecker::isRunningAsAdmin() const
{
#ifdef _WIN32
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
    
    // Interpretar código de saída
    // 0 = seguro, 1 = vulnerável, outros = erro
    bool isVulnerable = (exitCode == 1);
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
    // Implementar comandos específicos de verificação para cada vulnerabilidade
    
#ifdef _WIN32
    if (vuln.id == "UAC_DISABLED") {
        return "reg query \"HKLM\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\" /v EnableLUA";
    }
    else if (vuln.id == "FIREWALL_DISABLED") {
        return "netsh advfirewall show allprofiles state";
    }
    else if (vuln.id == "SMB1_ENABLED") {
        return "powershell \"Get-WindowsOptionalFeature -Online -FeatureName SMB1Protocol\"";
    }
    else if (vuln.id == "WINDOWS_UPDATE_DISABLED") {
        return "sc query wuauserv";
    }
    else if (vuln.id == "ADMIN_USER_DEFAULT") {
        return "net user Administrator";
    }
    else if (vuln.id == "ANTIVIRUS_DISABLED") {
        return "powershell \"Get-MpPreference | Select-Object DisableRealtimeMonitoring\"";
    }
    else if (vuln.id == "GUEST_ACCOUNT_ENABLED") {
        return "net user Guest";
    }
#elif defined(__linux__)
    if (vuln.id == "SSH_ROOT_LOGIN") {
        return "grep -i PermitRootLogin /etc/ssh/sshd_config";
    }
    else if (vuln.id == "NO_FIREWALL") {
        return "ufw status";
    }
    else if (vuln.id == "SUDO_NOPASSWD") {
        return "grep NOPASSWD /etc/sudoers";
    }
    else if (vuln.id == "OLD_KERNEL") {
        return "apt list --upgradable | grep linux-image";
    }
#elif defined(__APPLE__)
    if (vuln.id == "GATEKEEPER_OFF") {
        return "spctl --status";
    }
    else if (vuln.id == "FIREWALL_OFF") {
        return "/usr/libexec/ApplicationFirewall/socketfilterfw --getglobalstate";
    }
    else if (vuln.id == "NO_FILEVAULT") {
        return "fdesetup status";
    }
#endif
    
    return QString(); // Comando não implementado - usar simulação
}

QString SystemChecker::getFixCommand(const VulnerabilityDefinition &vuln) const
{
    // Retornar o comando de correção diretamente da definição
    return vuln.fix;
}

bool SystemChecker::executeCommand(const QString &command, QProcess *process)
{
    if (command.isEmpty() || !process) {
        return false;
    }
    
#ifdef _WIN32
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