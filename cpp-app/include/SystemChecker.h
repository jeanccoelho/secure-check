#ifndef SYSTEMCHECKER_H
#define SYSTEMCHECKER_H

#include <QObject>
#include <QProcess>
#include <QTimer>
#include <QRandomGenerator>
#include "VulnerabilityDefinition.h"

class SystemChecker : public QObject
{
    Q_OBJECT

public:
    explicit SystemChecker(QObject *parent = nullptr);
    
    void checkVulnerability(const VulnerabilityDefinition &vuln);
    void fixVulnerability(const VulnerabilityDefinition &vuln);
    bool isRunningAsAdmin() const;

signals:
    void checkCompleted(const QString &id, bool isVulnerable);
    void fixCompleted(const QString &id, bool success);
    void errorOccurred(const QString &error);

private slots:
    void onCheckProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onFixProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess *m_checkProcess;
    QProcess *m_fixProcess;
    QString m_currentCheckId;
    QString m_currentFixId;
    
    QString getCheckCommand(const VulnerabilityDefinition &vuln) const;
    QString getFixCommand(const VulnerabilityDefinition &vuln) const;
    bool executeCommand(const QString &command, QProcess *process);
};

#endif // SYSTEMCHECKER_H