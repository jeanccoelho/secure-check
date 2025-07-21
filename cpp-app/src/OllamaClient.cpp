#include "OllamaClient.h"
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QDebug>
#include <QUrlQuery>
#include <QHttpMultiPart>

// Endpoint fixo do Ollama
const QString OllamaClient::OLLAMA_ENDPOINT = "https://ollama.annabank.com.br";

OllamaClient::OllamaClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_timeoutTimer(new QTimer(this))
    , m_currentReply(nullptr)
{
    // Configurar timeout de 30 segundos
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(30000);
    connect(m_timeoutTimer, &QTimer::timeout, this, &OllamaClient::onTimeout);
    
    qDebug() << "OllamaClient inicializado com endpoint:" << OLLAMA_ENDPOINT;
}

OllamaClient::~OllamaClient()
{
    cleanup();
}

void OllamaClient::getAvailableModels()
{
    cleanup();
    
    QUrl url(OLLAMA_ENDPOINT + "/api/tags");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "SecurityChecker/1.0");
    
    qDebug() << "Buscando modelos disponíveis em:" << url.toString();
    
    m_currentReply = m_networkManager->get(request);
    connect(m_currentReply, &QNetworkReply::finished, this, &OllamaClient::onModelsReplyFinished);
    connect(m_currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &OllamaClient::onNetworkError);
    
    m_timeoutTimer->start();
}

void OllamaClient::analyzeSystemSecurity(const SystemInfo &systemInfo, const QString &modelName)
{
    cleanup();
    
    QUrl url(OLLAMA_ENDPOINT + "/api/generate");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("User-Agent", "SecurityChecker/1.0");
    
    // Construir prompt para análise de segurança
    QString prompt = buildSystemAnalysisPrompt(systemInfo);
    
    QJsonObject requestData;
    requestData["model"] = modelName;
    requestData["prompt"] = prompt;
    requestData["stream"] = false;
    requestData["options"] = QJsonObject{
        {"temperature", 0.1},
        {"top_p", 0.9},
        {"max_tokens", 4000}
    };
    
    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();
    
    qDebug() << "Enviando análise para modelo:" << modelName;
    qDebug() << "Tamanho do prompt:" << prompt.length() << "caracteres";
    
    m_currentReply = m_networkManager->post(request, data);
    connect(m_currentReply, &QNetworkReply::finished, this, &OllamaClient::onAnalysisReplyFinished);
    connect(m_currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &OllamaClient::onNetworkError);
    
    m_timeoutTimer->start();
}

void OllamaClient::testConnection()
{
    cleanup();
    
    QUrl url(OLLAMA_ENDPOINT + "/api/version");
    QNetworkRequest request(url);
    request.setRawHeader("User-Agent", "SecurityChecker/1.0");
    
    qDebug() << "Testando conexão com:" << url.toString();
    
    m_currentReply = m_networkManager->get(request);
    connect(m_currentReply, &QNetworkReply::finished, this, &OllamaClient::onConnectionTestFinished);
    connect(m_currentReply, QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred),
            this, &OllamaClient::onNetworkError);
    
    m_timeoutTimer->start();
}

void OllamaClient::onModelsReplyFinished()
{
    if (!m_currentReply) return;
    
    m_timeoutTimer->stop();
    
    if (m_currentReply->error() != QNetworkReply::NoError) {
        emit errorOccurred(QString("Erro ao buscar modelos: %1").arg(m_currentReply->errorString()));
        cleanup();
        return;
    }
    
    QByteArray data = m_currentReply->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        emit errorOccurred(QString("Erro ao parsear resposta dos modelos: %1").arg(parseError.errorString()));
        cleanup();
        return;
    }
    
    QList<OllamaModel> models;
    
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        if (obj.contains("models") && obj["models"].isArray()) {
            QJsonArray modelsArray = obj["models"].toArray();
            
            for (const QJsonValue &value : modelsArray) {
                if (value.isObject()) {
                    QJsonObject modelObj = value.toObject();
                    OllamaModel model;
                    model.name = modelObj["name"].toString();
                    model.size = modelObj["size"].toString();
                    model.modified = modelObj["modified_at"].toString();
                    model.digest = modelObj["digest"].toString();
                    
                    if (!model.name.isEmpty()) {
                        models.append(model);
                    }
                }
            }
        }
    }
    
    qDebug() << "Modelos encontrados:" << models.size();
    for (const auto &model : models) {
        qDebug() << "  -" << model.name << "(" << model.size << ")";
    }
    
    emit modelsReceived(models);
    cleanup();
}

void OllamaClient::onAnalysisReplyFinished()
{
    if (!m_currentReply) return;
    
    m_timeoutTimer->stop();
    
    if (m_currentReply->error() != QNetworkReply::NoError) {
        emit errorOccurred(QString("Erro na análise de segurança: %1").arg(m_currentReply->errorString()));
        cleanup();
        return;
    }
    
    QByteArray data = m_currentReply->readAll();
    qDebug() << "Raw Ollama API response data:" << data; // Adicionado para depuração
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        emit errorOccurred(QString("Erro ao parsear resposta da análise: %1").arg(parseError.errorString()));
        cleanup();
        return;
    }
    
    QString response;
    if (doc.isObject()) {
        QJsonObject obj = doc.object();
        response = obj["response"].toString();
        qDebug() << "Extracted 'response' field from Ollama:" << response.left(500) << "..."; // Adicionado para depuração
    }
    
    if (response.isEmpty()) {
        emit errorOccurred("Resposta vazia do Ollama");
        cleanup();
        return;
    }
    
    qDebug() << "Resposta do Ollama recebida:" << response.left(200) << "...";
    
    // Parsear vulnerabilidades da resposta
    QVector<VulnerabilityDefinition> vulnerabilities = parseVulnerabilitiesFromResponse(response);
    
    qDebug() << "Vulnerabilidades identificadas pela IA:" << vulnerabilities.size();
    
    emit vulnerabilitiesReceived(vulnerabilities);
    cleanup();
}

void OllamaClient::onConnectionTestFinished()
{
    if (!m_currentReply) return;
    
    m_timeoutTimer->stop();
    
    bool success = (m_currentReply->error() == QNetworkReply::NoError);
    QString message;
    
    if (success) {
        QByteArray data = m_currentReply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QString version = doc.object()["version"].toString();
            message = QString("Conectado com sucesso! Versão do Ollama: %1").arg(version);
        } else {
            message = "Conectado com sucesso!";
        }
    } else {
        message = QString("Falha na conexão: %1").arg(m_currentReply->errorString());
    }
    
    qDebug() << "Teste de conexão:" << (success ? "SUCESSO" : "FALHA") << "-" << message;
    
    emit connectionTestResult(success, message);
    cleanup();
}

void OllamaClient::onNetworkError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    
    if (m_currentReply) {
        QString errorMsg = QString("Erro de rede: %1").arg(m_currentReply->errorString());
        qDebug() << errorMsg;
        emit errorOccurred(errorMsg);
    }
}

void OllamaClient::onTimeout()
{
    qDebug() << "Timeout na requisição para o Ollama";
    emit errorOccurred("Timeout: O servidor Ollama não respondeu em 30 segundos");
    cleanup();
}

QString OllamaClient::buildSystemAnalysisPrompt(const SystemInfo &systemInfo) const
{
    QString prompt = QString(R"(
Você é um especialista em segurança cibernética. Analise as informações do sistema abaixo e identifique vulnerabilidades de segurança.

INFORMAÇÕES DO SISTEMA:
- Sistema Operacional: %1 %2
- Kernel: %3
- Arquitetura: %4
- Serviços em execução: %5
- Portas abertas: %6
- Software instalado: %7
- Configurações do sistema: %8

INSTRUÇÕES:
1. Identifique vulnerabilidades de segurança baseadas nas informações fornecidas
2. Para cada vulnerabilidade, forneça uma correção específica
3. Classifique a severidade como "Alta", "Média" ou "Baixa"
4. Retorne APENAS um JSON válido no seguinte formato:

{
  "vulnerabilities": [
    {
      "id": "OLLAMA_VULN_001",
      "name": "Nome da Vulnerabilidade",
      "description": "Descrição detalhada da vulnerabilidade encontrada",
      "impact": "Impacto específico desta vulnerabilidade no sistema",
      "severity": "Alta",
      "fix": "Comando ou instrução específica para correção"
    }
  ]
}

IMPORTANTE: Retorne APENAS o JSON, sem texto adicional antes ou depois.
)").arg(
        systemInfo.osType,
        systemInfo.osVersion,
        systemInfo.kernelVersion,
        systemInfo.architecture,
        systemInfo.runningServices.join(", "),
        systemInfo.openPorts.join(", "),
        systemInfo.installedSoftware.join(", "),
        systemInfo.systemConfigs.join(", ")
    );
    
    return prompt;
}

QVector<VulnerabilityDefinition> OllamaClient::parseVulnerabilitiesFromResponse(const QString &response) const
{
    QVector<VulnerabilityDefinition> vulnerabilities;
    
    // Tentar encontrar JSON na resposta
    qDebug() << "Attempting to parse this string as JSON:" << response.left(500) << "..."; // Adicionado para depuração
    QString jsonStr = response.trimmed();
    
    // Se a resposta não começar com {, tentar encontrar o JSON
    if (!jsonStr.startsWith("{")) {
        int startIndex = jsonStr.indexOf("{");
        int endIndex = jsonStr.lastIndexOf("}");
        
        if (startIndex != -1 && endIndex != -1 && endIndex > startIndex) {
            jsonStr = jsonStr.mid(startIndex, endIndex - startIndex + 1);
        }
    }
    
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);
    
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "Erro ao parsear JSON da resposta do Ollama:" << parseError.errorString();
        qWarning() << "Resposta recebida:" << response.left(500);
        return vulnerabilities;
    }
    
    if (!doc.isObject()) {
        qWarning() << "Resposta do Ollama não é um objeto JSON válido";
        return vulnerabilities;
    }
    
    QJsonObject rootObj = doc.object();
    if (!rootObj.contains("vulnerabilities") || !rootObj["vulnerabilities"].isArray()) {
        qWarning() << "Resposta do Ollama não contém array 'vulnerabilities'";
        return vulnerabilities;
    }
    
    QJsonArray vulnArray = rootObj["vulnerabilities"].toArray();
    
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
        else vuln.severity = Severity::Media; // Default
        
        // Validar campos obrigatórios
        if (!vuln.id.isEmpty() && !vuln.name.isEmpty() && !vuln.description.isEmpty()) {
            vulnerabilities.append(vuln);
        }
    }
    
    return vulnerabilities;
}

void OllamaClient::cleanup()
{
    if (m_timeoutTimer->isActive()) {
        m_timeoutTimer->stop();
    }
    
    if (m_currentReply) {
        m_currentReply->disconnect();
        m_currentReply->deleteLater();
        m_currentReply = nullptr;
    }
}