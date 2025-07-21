#ifndef OLLAMACLIENT_H
#define OLLAMACLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include "VulnerabilityDefinition.h"

struct OllamaConfig {
    QString endpoint = "http://localhost:11434";
    QString model = "llama3.1";
    int timeout = 30000; // 30 segundos
    bool enabled = false;
};

struct SystemInfo {
    QString osType;
    QString osVersion;
    QString kernelVersion;
    QString architecture;
    QStringList runningServices;
    QStringList installedPackages;
    QStringList networkPorts;
    QStringList userAccounts;
    QStringList systemConfigs;
    QString additionalInfo;
};

class OllamaClient : public QObject
{
    Q_OBJECT

public:
    explicit OllamaClient(QObject *parent = nullptr);
    ~OllamaClient();
    
    void setConfig(const OllamaConfig &config);
    OllamaConfig getConfig() const;
    
    void analyzeSystem(const SystemInfo &systemInfo);
    void testConnection();
    
    bool isConnected() const;
    QString getLastError() const;

signals:
    void vulnerabilitiesFound(const QVector<VulnerabilityDefinition> &vulnerabilities);
    void analysisCompleted(bool success);
    void connectionTestCompleted(bool success, const QString &message);
    void errorOccurred(const QString &error);
    void progressUpdate(const QString &status);

private slots:
    void onNetworkReply();
    void onNetworkError(QNetworkReply::NetworkError error);
    void onTimeout();

private:
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    QTimer *m_timeoutTimer;
    OllamaConfig m_config;
    QString m_lastError;
    bool m_isConnected;
    
    QString buildPrompt(const SystemInfo &systemInfo) const;
    QVector<VulnerabilityDefinition> parseOllamaResponse(const QJsonDocument &response) const;
    QJsonObject buildOllamaRequest(const QString &prompt) const;
    void cleanup();
};

#endif // OLLAMACLIENT_H