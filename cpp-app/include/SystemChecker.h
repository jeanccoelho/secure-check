#ifndef SYSTEMCHECKER_H
#define SYSTEMCHECKER_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QRandomGenerator>
#include "VulnerabilityDefinition.h"
#include "OllamaClient.h"

class SystemChecker : public QObject
{
    Q_OBJECT

public:
    explicit SystemChecker(QObject *parent = nullptr);
    
    void checkVulnerability(const VulnerabilityDefinition &vuln);
    void fixVulnerability(const VulnerabilityDefinition &vuln);
    bool isRunningAsAdmin() const;
    
    // Novos métodos para coleta de dados do sistema
    SystemInfo collectSystemInfo() const;
    void collectSystemInfoAsync();

signals:
    void checkCompleted(const QString &id, bool isVulnerable);
    void fixCompleted(const QString &id, bool success);
    void errorOccurred(const QString &error);
    void systemInfoCollected(const SystemInfo &info);

private slots:
    void onCheckProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onFixProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onSystemInfoProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess *m_checkProcess;
    QProcess *m_fixProcess;
    QString m_currentCheckId;
    QString m_currentFixId;
    
    // Novos membros para coleta de informações
    QProcess *m_infoProcess;
    SystemInfo m_systemInfo;
    QStringList m_pendingInfoCommands;
    int m_currentInfoCommandIndex;
    
    QString getCheckCommand(const VulnerabilityDefinition &vuln) const;
    QString getFixCommand(const VulnerabilityDefinition &vuln) const;
    bool executeCommand(const QString &command, QProcess *process);
    
    // Novos métodos privados para coleta de dados
    QStringList getSystemInfoCommands() const;
    void processSystemInfoCommand(const QString &command, const QString &output);
    void executeNextInfoCommand();
};

#endif // SYSTEMCHECKER_H