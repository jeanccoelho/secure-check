#include "OllamaClient.h"
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QDebug>
#include <QUrlQuery>
#include <QRandomGenerator>

OllamaClient::OllamaClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(nullptr)
    , m_currentReply(nullptr)
    , m_timeoutTimer(nullptr)
    , m_isConnected(false)
{
    m_networkManager = new QNetworkAccessManager(this);
    
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setSingleShot(true);
    connect(m_timeoutTimer, &QTimer::timeout, this, &OllamaClient::onTimeout);
}

OllamaClient::~OllamaClient()
{
    cleanup();
}

void OllamaClient::setConfig(const OllamaConfig &config)
{
    m_config = config;
    qDebug() << "Ollama config atualizada:" << config.endpoint << config.model;
}

OllamaConfig OllamaClient::getConfig() const
{
    return m_config;
}

void OllamaClient::testConnection()
{
    if (!m_config.enabled) {
        emit connectionTestCompleted(false, "Ollama está desabilitado");
        return;
    }
    
    cleanup();
    
    emit progressUpdate("Testando conexão com Ollama...");
    
    QUrl url(m_config.endpoint + "/api/tags");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "SecurityChecker/1.0");
    
    m_currentReply = m_networkManager->get(request);
    connect(m_currentReply, &QNetworkReply::finished, this, &OllamaClient::onNetworkReply);
    connect(m_currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &OllamaClient::onNetworkError);
    
    m_timeoutTimer->start(5000); // 5 segundos para teste de conexão
}

void OllamaClient::analyzeSystem(const SystemInfo &systemInfo)
{
    if (!m_config.enabled) {
        emit errorOccurred("Ollama está desabilitado");
        return;
    }
    
    cleanup();
    
    emit progressUpdate("Coletando informações do sistema...");
    
    QString prompt = buildPrompt(systemInfo);
    QJsonObject requestData = buildOllamaRequest(prompt);
    
    emit progressUpdate("Enviando dados para análise IA...");
    
    QUrl url(m_config.endpoint + "/api/generate");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "SecurityChecker/1.0");
    
    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    
    qDebug() << "Enviando para Ollama:" << url.toString();
    qDebug() << "Payload size:" << data.size() << "bytes";
    
    m_currentReply = m_networkManager->post(request, data);
    connect(m_currentReply, &QNetworkReply::finished, this, &OllamaClient::onNetworkReply);
    connect(m_currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &OllamaClient::onNetworkError);
    
    m_timeoutTimer->start(m_config.timeout);
}

QString OllamaClient::buildPrompt(const SystemInfo &systemInfo) const
{
    QString prompt = QString(R"(
Você é um especialista em segurança cibernética. Analise as informações do sistema abaixo e identifique vulnerabilidades de segurança.

INFORMAÇÕES DO SISTEMA:
- Sistema Operacional: %1 %2
- Kernel: %3
- Arquitetura: %4

SERVIÇOS EM EXECUÇÃO:
%5

PORTAS DE REDE ABERTAS:
%6

CONTAS DE USUÁRIO:
%7

CONFIGURAÇÕES DO SISTEMA:
%8

INFORMAÇÕES ADICIONAIS:
%9

INSTRUÇÕES:
1. Analise cuidadosamente todas as informações fornecidas
2. Identifique vulnerabilidades de segurança específicas para este sistema
3. Para cada vulnerabilidade encontrada, forneça:
   - Nome descritivo
   - Descrição detalhada
   - Impacto potencial
   - Nível de severidade (Alta, Média, Baixa)
   - Comando ou instrução específica para correção

FORMATO DE RESPOSTA:
Responda APENAS com um JSON válido no seguinte formato:

{
  "vulnerabilities": [
    {
      "id": "OLLAMA_VULN_001",
      "name": "Nome da Vulnerabilidade",
      "description": "Descrição detalhada da vulnerabilidade encontrada.",
      "impact": "Impacto específico desta vulnerabilidade no sistema.",
      "severity": "Alta",
      "fix": "Comando específico ou instrução detalhada para correção"
    }
  ]
}

IMPORTANTE: 
- Responda APENAS com JSON válido, sem texto adicional
- Use IDs únicos começando com OLLAMA_VULN_
- Seja específico nos comandos de correção para %1
- Foque em vulnerabilidades reais e práticas
- Se não encontrar vulnerabilidades, retorne array vazio
)")
    .arg(systemInfo.osType)
    .arg(systemInfo.osVersion)
    .arg(systemInfo.kernelVersion)
    .arg(systemInfo.architecture)
    .arg(systemInfo.runningServices.join("\n"))
    .arg(systemInfo.networkPorts.join("\n"))
    .arg(systemInfo.userAccounts.join("\n"))
    .arg(systemInfo.systemConfigs.join("\n"))
    .arg(systemInfo.additionalInfo);
    
    return prompt;
}

QJsonObject OllamaClient::buildOllamaRequest(const QString &prompt) const
{
    QJsonObject request;
    request["model"] = m_config.model;
    request["prompt"] = prompt;
    request["stream"] = false;
    request["format"] = "json";
    
    // Parâmetros para melhor qualidade de resposta
    QJsonObject options;
    options["temperature"] = 0.1; // Mais determinístico
    options["top_p"] = 0.9;
    options["num_predict"] = 2048; // Máximo de tokens
    request["options"] = options;
    
    return request;
}

void OllamaClient::onNetworkReply()
{
    if (!m_currentReply) {
        return;
    }
    
    m_timeoutTimer->stop();
    
    QByteArray responseData = m_currentReply->readAll();
    int statusCode = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    
    qDebug() << "Ollama response status:" << statusCode;
    qDebug() << "Response size:" << responseData.size() << "bytes";
    
    if (statusCode == 200) {
        // Verificar se é teste de conexão ou análise
        QString url = m_currentReply->url().toString();
        
        if (url.contains("/api/tags")) {
            // Teste de conexão
            m_isConnected = true;
            emit connectionTestCompleted(true, "Conexão com Ollama estabelecida com sucesso");
        } else {
            // Análise de vulnerabilidades
            emit progressUpdate("Processando resposta da IA...");
            
            QJsonParseError parseError;
            QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);
            
            if (parseError.error != QJsonParseError::NoError) {
                m_lastError = QString("Erro ao parsear resposta: %1").arg(parseError.errorString());
                emit errorOccurred(m_lastError);
                emit analysisCompleted(false);
            } else {
                try {
                    QVector<VulnerabilityDefinition> vulnerabilities = parseOllamaResponse(doc);
                    emit vulnerabilitiesFound(vulnerabilities);
                    emit analysisCompleted(true);
                } catch (const std::exception &e) {
                    m_lastError = QString("Erro ao processar vulnerabilidades: %1").arg(e.what());
                    emit errorOccurred(m_lastError);
                    emit analysisCompleted(false);
                }
            }
        }
    } else {
        m_lastError = QString("Erro HTTP %1: %2").arg(statusCode).arg(QString::fromUtf8(responseData));
        emit errorOccurred(m_lastError);
        
        if (m_currentReply->url().toString().contains("/api/tags")) {
            emit connectionTestCompleted(false, m_lastError);
        } else {
            emit analysisCompleted(false);
        }
    }
    
    cleanup();
}

void OllamaClient::onNetworkError(QNetworkReply::NetworkError error)
{
    m_timeoutTimer->stop();
    
    QString errorString;
    switch (error) {
        case QNetworkReply::ConnectionRefusedError:
            errorString = "Conexão recusada. Verifique se o Ollama está rodando.";
            break;
        case QNetworkReply::HostNotFoundError:
            errorString = "Host não encontrado. Verifique o endpoint do Ollama.";
            break;
        case QNetworkReply::TimeoutError:
            errorString = "Timeout na conexão.";
            break;
        default:
            errorString = QString("Erro de rede: %1").arg(m_currentReply ? m_currentReply->errorString() : "Desconhecido");
            break;
    }
    
    m_lastError = errorString;
    m_isConnected = false;
    
    emit errorOccurred(errorString);
    
    if (m_currentReply && m_currentReply->url().toString().contains("/api/tags")) {
        emit connectionTestCompleted(false, errorString);
    } else {
        emit analysisCompleted(false);
    }
    
    cleanup();
}

void OllamaClient::onTimeout()
{
    m_lastError = "Timeout na requisição ao Ollama";
    emit errorOccurred(m_lastError);
    
    if (m_currentReply && m_currentReply->url().toString().contains("/api/tags")) {
        emit connectionTestCompleted(false, m_lastError);
    } else {
        emit analysisCompleted(false);
    }
    
    cleanup();
}

QVector<VulnerabilityDefinition> OllamaClient::parseOllamaResponse(const QJsonDocument &response) const
{
    QVector<VulnerabilityDefinition> vulnerabilities;
    
    if (!response.isObject()) {
        throw std::runtime_error("Resposta não é um objeto JSON válido");
    }
    
    QJsonObject root = response.object();
    
    // Ollama retorna a resposta em "response"
    QString responseText;
    if (root.contains("response")) {
        responseText = root["response"].toString();
    } else {
        throw std::runtime_error("Campo 'response' não encontrado na resposta do Ollama");
    }
    
    // Parsear o JSON dentro da resposta
    QJsonParseError parseError;
    QJsonDocument innerDoc = QJsonDocument::fromJson(responseText.toUtf8(), &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "Resposta do Ollama:" << responseText;
        throw std::runtime_error(QString("Erro ao parsear JSON interno: %1").arg(parseError.errorString()).toStdString());
    }
    
    if (!innerDoc.isObject()) {
        throw std::runtime_error("JSON interno não é um objeto válido");
    }
    
    QJsonObject innerRoot = innerDoc.object();
    
    if (!innerRoot.contains("vulnerabilities") || !innerRoot["vulnerabilities"].isArray()) {
        qDebug() << "JSON parseado:" << innerDoc.toJson();
        throw std::runtime_error("Campo 'vulnerabilities' não encontrado ou não é um array");
    }
    
    QJsonArray vulnArray = innerRoot["vulnerabilities"].toArray();
    
    for (const QJsonValue &value : vulnArray) {
        if (!value.isObject()) continue;
        
        QJsonObject vulnObj = value.toObject();
        VulnerabilityDefinition vuln;
        
        vuln.id = vulnObj["id"].toString();
        vuln.name = vulnObj["name"].toString();
        vuln.description = vulnObj["description"].toString();
        vuln.impact = vulnObj["impact"].toString();
        vuln.fix = vulnObj["fix"].toString();
        
        QString severityStr = vulnObj["severity"].toString();
        if (severityStr == "Alta") vuln.severity = Severity::Alta;
        else if (severityStr == "Média") vuln.severity = Severity::Media;
        else if (severityStr == "Baixa") vuln.severity = Severity::Baixa;
        else vuln.severity = Severity::Media;
        
        // Validar campos obrigatórios
        if (!vuln.id.isEmpty() && !vuln.name.isEmpty() && !vuln.description.isEmpty()) {
            vulnerabilities.append(vuln);
        }
    }
    
    qDebug() << "Vulnerabilidades parseadas:" << vulnerabilities.size();
    return vulnerabilities;
}

bool OllamaClient::isConnected() const
{
    return m_isConnected;
}

QString OllamaClient::getLastError() const
{
    return m_lastError;
}

void OllamaClient::cleanup()
{
    if (m_currentReply) {
        m_currentReply->disconnect();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
    
    if (m_timeoutTimer) {
        m_timeoutTimer->stop();
    }
}