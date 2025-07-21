#ifndef OLLAMACLIENT_H
#define OLLAMACLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QFuture>
#include <QFutureWatcher>
#include "VulnerabilityDefinition.h"

struct OllamaModel {
    QString name;
    QString size;
    QString modified;
    QString digest;
};

struct SystemInfo {
    QString osType;
    QString osVersion;
    QString kernelVersion;
    QString architecture;
    QStringList runningServices;
    QStringList openPorts;
    QStringList installedSoftware;
    QStringList systemConfigs;
};

class OllamaClient : public QObject
{
    Q_OBJECT

public:
    explicit OllamaClient(QObject *parent = nullptr);
    ~OllamaClient();
    
    // Descoberta de modelos disponíveis
    void getAvailableModels();
    
    // Análise de vulnerabilidades via IA
    void analyzeSystemSecurity(const SystemInfo &systemInfo, const QString &modelName);
    
    // Verificar conectividade
    void testConnection();
    
    // Constantes
    static const QString OLLAMA_ENDPOINT;

signals:
    void modelsReceived(const QList<OllamaModel> &models);
    void vulnerabilitiesReceived(const QVector<VulnerabilityDefinition> &vulnerabilities);
    void errorOccurred(const QString &error);
    void connectionTestResult(bool success, const QString &message);

private slots:
    void onModelsReplyFinished();
    void onAnalysisReplyFinished();
    void onConnectionTestFinished();
    void onNetworkError(QNetworkReply::NetworkError error);
    void onTimeout();

private:
    QNetworkAccessManager *m_networkManager;
    QTimer *m_timeoutTimer;
    QNetworkReply *m_currentReply;
    
    QString buildSystemAnalysisPrompt(const SystemInfo &systemInfo) const;
    QVector<VulnerabilityDefinition> parseVulnerabilitiesFromResponse(const QString &response) const;
    void handleNetworkReply(QNetworkReply *reply);
    void cleanup();
};

#endif // OLLAMACLIENT_H