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
    // Configurar timeout de 180 segundos para análises de IA (3 minutos)
    m_timeoutTimer->setSingleShot(true);
    m_timeoutTimer->setInterval(180000);
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
    
    // Adicionar headers adicionais para debugging
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("Connection", "keep-alive");
    
    // Construir prompt para análise de segurança
    QString prompt = buildSystemAnalysisPrompt(systemInfo);
    
    QJsonObject requestData;
    requestData["model"] = modelName;
    requestData["prompt"] = prompt;
    requestData["stream"] = false;
    requestData["options"] = QJsonObject{
        {"temperature", 0.1},
        {"top_p", 0.9},
        {"num_predict", 4000}
    };
    
    QJsonDocument doc(requestData);
    QByteArray data = doc.toJson();
    
    qDebug() << "Enviando análise para modelo:" << modelName;
    qDebug() << "Tamanho do prompt:" << prompt.length() << "caracteres";
    qDebug() << "Request JSON size:" << data.size() << "bytes";
    qDebug() << "Request URL:" << url.toString();
    
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
    
    // Log detalhado do status HTTP
    int httpStatus = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString httpReason = m_currentReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    qDebug() << "Models HTTP Status:" << httpStatus << httpReason;
    
    if (m_currentReply->error() != QNetworkReply::NoError) {
        QString errorDetails = QString("HTTP %1 %2 - %3").arg(httpStatus).arg(httpReason).arg(m_currentReply->errorString());
        qDebug() << "Erro detalhado nos modelos:" << errorDetails;
        emit errorOccurred(QString("Erro ao buscar modelos: %1").arg(errorDetails));
        cleanup();
        return;
    }
    
    QByteArray data = m_currentReply->readAll();
    qDebug() << "Raw Ollama API response data:" << data; // Adicionado para depuração
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
    
    // Log detalhado do status HTTP
    int httpStatus = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString httpReason = m_currentReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    qDebug() << "HTTP Status:" << httpStatus << httpReason;
    
    // Tratamento específico para erros de servidor
    if (httpStatus == 504) {
        emit errorOccurred("Servidor Ollama não respondeu (Gateway Timeout). O servidor pode estar sobrecarregado ou o modelo pode estar sendo carregado. Tente novamente em alguns minutos.");
        cleanup();
        return;
    }
    
    if (httpStatus == 502) {
        emit errorOccurred("Servidor Ollama indisponível (Bad Gateway). Verifique se o serviço Ollama está rodando.");
        cleanup();
        return;
    }
    
    if (httpStatus == 503) {
        emit errorOccurred("Servidor Ollama temporariamente indisponível (Service Unavailable). Tente novamente em alguns minutos.");
        cleanup();
        return;
    }
    
    if (m_currentReply->error() != QNetworkReply::NoError) {
        QString errorDetails = QString("HTTP %1 %2 - %3").arg(httpStatus).arg(httpReason).arg(m_currentReply->errorString());
        qDebug() << "Erro detalhado:" << errorDetails;
        
        // Mensagem mais amigável para o usuário
        QString userMessage;
        if (httpStatus >= 500) {
            userMessage = "Erro interno do servidor Ollama. Tente usar a verificação local ou aguarde alguns minutos.";
        } else if (httpStatus >= 400) {
            userMessage = "Erro na requisição para o Ollama. Verifique se o modelo está disponível.";
        } else {
            userMessage = QString("Erro de conectividade: %1").arg(m_currentReply->errorString());
        }
        
        emit errorOccurred(userMessage);
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
    
    // Log detalhado do status HTTP
    int httpStatus = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    QString httpReason = m_currentReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    qDebug() << "Connection test HTTP Status:" << httpStatus << httpReason;
    
    bool success = (m_currentReply->error() == QNetworkReply::NoError);
    QString message;
    
    if (success) {
        QByteArray data = m_currentReply->readAll();
        qDebug() << "Connection test response:" << data;
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isObject()) {
            QString version = doc.object()["version"].toString();
            message = QString("Conectado com sucesso! Versão do Ollama: %1").arg(version);
        } else {
            message = "Conectado com sucesso!";
        }
    } else {
        QString errorDetails = QString("HTTP %1 %2 - %3").arg(httpStatus).arg(httpReason).arg(m_currentReply->errorString());
        message = QString("Falha na conexão: %1").arg(errorDetails);
    }
    
    qDebug() << "Teste de conexão:" << (success ? "SUCESSO" : "FALHA") << "-" << message;
    
    emit connectionTestResult(success, message);
    cleanup();
}

void OllamaClient::onNetworkError(QNetworkReply::NetworkError error)
{
    qDebug() << "Network error code:" << error;
    
    if (m_currentReply) {
        int httpStatus = m_currentReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        QString httpReason = m_currentReply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        QByteArray responseData = m_currentReply->readAll();
        
        QString errorMsg = QString("Erro de rede (código %1): HTTP %2 %3 - %4")
                          .arg(error)
                          .arg(httpStatus)
                          .arg(httpReason)
                          .arg(m_currentReply->errorString());
        
        if (!responseData.isEmpty()) {
            qDebug() << "Response body on error:" << responseData;
            errorMsg += QString(" | Response: %1").arg(QString::fromUtf8(responseData.left(200)));
        }
        
        qDebug() << errorMsg;
        emit errorOccurred(errorMsg);
    }
}

void OllamaClient::onTimeout()
{
    qDebug() << "Timeout na requisição para o Ollama (180 segundos)";
    emit errorOccurred("Timeout: O servidor Ollama não respondeu em 3 minutos. O servidor pode estar sobrecarregado ou processando uma requisição complexa. Tente novamente mais tarde ou use a verificação local.");
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