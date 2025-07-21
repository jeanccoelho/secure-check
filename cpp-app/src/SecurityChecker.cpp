#include "SecurityChecker.h"
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QDateTime>
#include <QScrollArea>
#include <QSysInfo>
#include <QProcess>

SecurityChecker::SecurityChecker(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_vulnerabilityManager(nullptr)
    , m_systemChecker(nullptr)
    , m_ollamaClient(nullptr)
    , m_currentCheckIndex(0)
    , m_isCompleted(false)
    , m_scanMode(LandingPage::ScanMode::Local)
{
    setupUI();
    
    // Inicializar managers
    m_vulnerabilityManager = new VulnerabilityManager(this);
    m_systemChecker = new SystemChecker(this);
    m_ollamaClient = new OllamaClient(this);
    
    // Conectar sinais
    connect(m_systemChecker, &SystemChecker::checkCompleted,
            this, &SecurityChecker::onCheckCompleted);
    connect(m_systemChecker, &SystemChecker::fixCompleted,
            this, &SecurityChecker::onFixCompleted);
    connect(m_systemChecker, &SystemChecker::errorOccurred,
            this, &SecurityChecker::onErrorOccurred);
    
    // Conectar sinais do Ollama
    connect(m_ollamaClient, &OllamaClient::vulnerabilitiesReceived,
            this, &SecurityChecker::onOllamaVulnerabilitiesReceived);
    connect(m_ollamaClient, &OllamaClient::errorOccurred,
            this, &SecurityChecker::onOllamaError);
    
    // Carregar vulnerabilidades
    loadVulnerabilities();
}

void SecurityChecker::setScanMode(LandingPage::ScanMode mode, const QString &modelName)
{
    m_scanMode = mode;
    m_selectedModel = modelName;
    
    qDebug() << "Modo de verificação definido:" << (mode == LandingPage::ScanMode::Local ? "Local" : "Ollama");
    if (mode == LandingPage::ScanMode::Ollama) {
        qDebug() << "Modelo selecionado:" << modelName;
    }
    
    // Recarregar vulnerabilidades com base no novo modo
    loadVulnerabilities();
}

void SecurityChecker::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Forçar fundo branco no widget principal
    setStyleSheet("SecurityChecker { background-color: white; }");
    
    createHeader();
    createProgressSection();
    createCheckSection();
    createActionButtons();
    createResultsSection();
    
    // Aplicar estilos modernos similares à versão web
    setStyleSheet(R"(
        SecurityChecker, SecurityChecker QWidget {
            background: white;
            background-color: white;
            font-family: "Segoe UI", "Roboto", "Inter", sans-serif;
        }
        
        SecurityChecker {
            background: white;
            background-color: white;
        }
        
        QFrame#headerFrame {
            background: white;
            background-color: white;
            border: none;
            border-bottom: 1px solid #e5e7eb;
        }
        
        QFrame#progressFrame {
            background: white;
            background-color: white;
            border: 1px solid #e5e7eb;
            border-radius: 12px;
            margin: 8px;
        }
        
        QFrame#checkFrame {
            background: white;
            background-color: white;
            border: 1px solid #e5e7eb;
            border-radius: 12px;
            margin: 8px;
        }
        
        QFrame#resultFrame {
            background: white;
            background-color: white;
            border: 1px solid #e5e7eb;
            border-radius: 12px;
            margin: 8px;
        }
        
        QFrame#resultsFrame {
            background: white;
            background-color: white;
            border: 1px solid #e5e7eb;
            border-radius: 12px;
            margin: 8px;
        }
        
        QPushButton {
            border: none;
            border-radius: 8px;
            padding: 12px 24px;
            font-weight: 600;
            font-size: 14px;
            font-family: "Segoe UI", sans-serif;
        }
        
        QPushButton#backButton {
            background: #f3f4f6;
            color: #374151;
            padding: 8px 16px;
        }
        
        QPushButton#backButton:hover {
            background: #e5e7eb;
        }
        
        QPushButton#primaryButton {
            background: #2563eb;
            color: white;
            font-size: 16px;
            padding: 16px 32px;
            font-weight: 600;
        }
        
        QPushButton#primaryButton:hover {
            background: #1d4ed8;
        }
        
        QPushButton#primaryButton:disabled {
            background: #9ca3af;
        }
        
        QPushButton#successButton {
            background: #059669;
            color: white;
            font-weight: 600;
        }
        
        QPushButton#successButton:hover {
            background: #047857;
        }
        
        QPushButton#warningButton {
            background: #d97706;
            color: white;
            font-weight: 600;
        }
        
        QPushButton#warningButton:hover {
            background: #b45309;
        }
        
        QPushButton#secondaryButton {
            background: #6b7280;
            color: white;
            font-weight: 600;
        }
        
        QPushButton#secondaryButton:hover {
            background: #4b5563;
        }
        
        QLabel#titleLabel {
            font-size: 28px;
            font-weight: 700;
            color: #1f2937;
            background: white;
        }
        
        QLabel#progressLabel {
            font-size: 14px;
            font-weight: 600;
            color: #374151;
            background: white;
        }
        
        QLabel#severityHigh {
            background: #fef2f2;
            color: #dc2626;
            padding: 6px 12px;
            border-radius: 16px;
            font-weight: 600;
            font-size: 12px;
        }
        
        QLabel#severityMedium {
            background: #fffbeb;
            color: #d97706;
            padding: 6px 12px;
            border-radius: 16px;
            font-weight: 600;
            font-size: 12px;
        }
        
        QLabel#severityLow {
            background: #fefce8;
            color: #ca8a04;
            padding: 6px 12px;
            border-radius: 16px;
            font-weight: 600;
            font-size: 12px;
        }
        
        QLabel#sectionTitle {
            font-size: 16px;
            font-weight: 600;
            color: #1f2937;
            background: white;
        }
        
        QLabel#description {
            font-size: 14px;
            color: #4b5563;
            line-height: 1.5;
            background: white;
        }
        
        QLabel#resultText {
            font-size: 18px;
            font-weight: 600;
            background: white;
        }
        
        QTextEdit {
            background: white;
            color: #1f2937;
            border: 1px solid #d1d5db;
            border-radius: 8px;
            font-family: 'Consolas', 'Monaco', 'Courier New', monospace;
            font-size: 12px;
            padding: 12px;
        }
        
        QProgressBar {
            border: none;
            border-radius: 8px;
            text-align: center;
            background: #f3f4f6;
            height: 16px;
        }
        
        QProgressBar::chunk {
            background: #2563eb;
            border-radius: 8px;
        }
        
        QComboBox {
            border: 1px solid #d1d5db;
            border-radius: 8px;
            padding: 8px 12px;
            background: white;
            color: #1f2937;
            font-weight: 500;
        }
        
        QComboBox:focus {
            border-color: #2563eb;
            outline: none;
        }
        
        QComboBox::drop-down {
            border: none;
            width: 20px;
        }
        
        QComboBox::down-arrow {
            image: none;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 5px solid #6b7280;
            margin-right: 5px;
        }
    )");
}

void SecurityChecker::createHeader()
{
    QFrame *headerFrame = new QFrame();
    headerFrame->setObjectName("headerFrame");
    headerFrame->setFixedHeight(80);
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(32, 0, 32, 0);
    
    // Botão voltar
    m_backButton = new QPushButton("← Voltar");
    m_backButton->setObjectName("backButton");
    connect(m_backButton, &QPushButton::clicked, this, &SecurityChecker::onBackClicked);
    
    headerLayout->addWidget(m_backButton);
    headerLayout->addStretch();
    
    // Exibição do OS detectado (somente leitura)
    QLabel *osLabel = new QLabel("Sistema Detectado:");
    osLabel->setStyleSheet("font-weight: 600; color: #374151; background: white;");
    
    m_osDisplay = new QLabel("Detectando...");
    m_osDisplay->setStyleSheet(
        "background: #f3f4f6; "
        "color: #1f2937; "
        "border: 1px solid #d1d5db; "
        "border-radius: 6px; "
        "padding: 8px 12px; "
        "font-weight: 600; "
        "min-width: 80px;"
    );
    
    m_osDisplay->setWordWrap(true);
    m_osDisplay->setStyleSheet(
        "background: #f3f4f6; "
        "color: #1f2937; "
        "border: 1px solid #d1d5db; "
        "border-radius: 6px; "
        "padding: 12px 16px; "
        "font-weight: 600; "
        "min-width: 200px; "
        "max-width: 300px; "
        "font-size: 12px; "
        "line-height: 1.4;"
    );
    
    headerLayout->addWidget(osLabel);
    headerLayout->addSpacing(8);
    headerLayout->addWidget(m_osDisplay);
    
    m_mainLayout->addWidget(headerFrame);
}

void SecurityChecker::createProgressSection()
{
    QFrame *progressFrame = new QFrame();
    progressFrame->setObjectName("progressFrame");
    
    QVBoxLayout *progressLayout = new QVBoxLayout(progressFrame);
    progressLayout->setContentsMargins(32, 24, 32, 24);
    progressLayout->setSpacing(12);
    
    // Header do progresso
    QHBoxLayout *progressHeaderLayout = new QHBoxLayout();
    
    QLabel *progressTitle = new QLabel("Progresso da Verificação");
    progressTitle->setStyleSheet("font-size: 16px; font-weight: 600; color: #1f2937; background: white;");
    
    m_progressLabel = new QLabel("0 de 0");
    m_progressLabel->setObjectName("progressLabel");
    
    progressHeaderLayout->addWidget(progressTitle);
    progressHeaderLayout->addStretch();
    progressHeaderLayout->addWidget(m_progressLabel);
    
    // Barra de progresso
    m_progressBar = new QProgressBar();
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    m_progressBar->setFixedHeight(16);
    m_progressBar->setTextVisible(false);
    
    progressLayout->addLayout(progressHeaderLayout);
    progressLayout->addWidget(m_progressBar);
    
    m_mainLayout->addWidget(progressFrame);
}

void SecurityChecker::createCheckSection()
{
    m_checkFrame = new QFrame();
    m_checkFrame->setObjectName("checkFrame");
    
    QVBoxLayout *checkLayout = new QVBoxLayout(m_checkFrame);
    checkLayout->setContentsMargins(32, 32, 32, 32);
    checkLayout->setSpacing(24);
    
    // Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    m_checkTitle = new QLabel("Carregando...");
    m_checkTitle->setStyleSheet("font-size: 24px; font-weight: 700; color: #1f2937; background: white;");
    m_checkTitle->setAlignment(Qt::AlignCenter);
    
    m_severityLabel = new QLabel();
    m_severityLabel->setAlignment(Qt::AlignCenter);
    
    headerLayout->addStretch();
    headerLayout->addWidget(m_checkTitle);
    headerLayout->addStretch();
    
    checkLayout->addLayout(headerLayout);
    checkLayout->addWidget(m_severityLabel);
    
    // Description
    m_descriptionLabel = new QLabel();
    m_descriptionLabel->setObjectName("description");
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setAlignment(Qt::AlignCenter);
    
    checkLayout->addWidget(m_descriptionLabel);
    
    // Impact
    m_impactLabel = new QLabel();
    m_impactLabel->setObjectName("description");
    m_impactLabel->setWordWrap(true);
    m_impactLabel->setAlignment(Qt::AlignCenter);
    
    checkLayout->addWidget(m_impactLabel);
    
    // Fix command
    m_fixCommandEdit = new QTextEdit();
    m_fixCommandEdit->setMaximumHeight(120);
    m_fixCommandEdit->setReadOnly(true);
    m_fixCommandEdit->hide();
    
    checkLayout->addWidget(m_fixCommandEdit);
    
    // Result frame
    m_resultFrame = new QFrame();
    m_resultFrame->setObjectName("resultFrame");
    m_resultFrame->hide();
    
    QVBoxLayout *resultLayout = new QVBoxLayout(m_resultFrame);
    resultLayout->setContentsMargins(24, 24, 24, 24);
    resultLayout->setAlignment(Qt::AlignCenter);
    
    m_resultIcon = new QLabel();
    m_resultIcon->setAlignment(Qt::AlignCenter);
    
    m_resultText = new QLabel();
    m_resultText->setObjectName("resultText");
    m_resultText->setAlignment(Qt::AlignCenter);
    m_resultText->setWordWrap(true);
    
    resultLayout->addWidget(m_resultIcon);
    resultLayout->addWidget(m_resultText);
    
    checkLayout->addWidget(m_resultFrame);
    
    m_mainLayout->addWidget(m_checkFrame);
}

void SecurityChecker::createActionButtons()
{
    QFrame *buttonFrame = new QFrame();
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonFrame);
    buttonLayout->setContentsMargins(32, 24, 32, 32);
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(16);
    
    m_startCheckButton = new QPushButton("🛡️ Iniciar Verificação");
    m_startCheckButton->setObjectName("primaryButton");
    connect(m_startCheckButton, &QPushButton::clicked, this, &SecurityChecker::onStartCheckClicked);
    
    m_fixButton = new QPushButton("🔧 Corrigir e Continuar");
    m_fixButton->setObjectName("successButton");
    m_fixButton->hide();
    connect(m_fixButton, &QPushButton::clicked, this, &SecurityChecker::onFixClicked);
    
    m_skipButton = new QPushButton("⏭️ Pular sem Corrigir");
    m_skipButton->setObjectName("secondaryButton");
    m_skipButton->hide();
    connect(m_skipButton, &QPushButton::clicked, this, &SecurityChecker::onSkipClicked);
    
    m_nextButton = new QPushButton("➡️ Continuar");
    m_nextButton->setObjectName("primaryButton");
    m_nextButton->hide();
    connect(m_nextButton, &QPushButton::clicked, this, &SecurityChecker::onNextClicked);
    
    buttonLayout->addWidget(m_startCheckButton);
    buttonLayout->addWidget(m_fixButton);
    buttonLayout->addWidget(m_skipButton);
    buttonLayout->addWidget(m_nextButton);
    
    m_mainLayout->addWidget(buttonFrame);
}

void SecurityChecker::createResultsSection()
{
    m_resultsFrame = new QFrame();
    m_resultsFrame->setObjectName("resultsFrame");
    m_resultsFrame->hide();
    
    m_resultsLayout = new QVBoxLayout(m_resultsFrame);
    m_resultsLayout->setContentsMargins(32, 32, 32, 32);
    m_resultsLayout->setSpacing(16);
    
    QLabel *resultsTitle = new QLabel("Resumo da Verificação");
    resultsTitle->setObjectName("titleLabel");
    resultsTitle->setAlignment(Qt::AlignCenter);
    
    m_resultsLayout->addWidget(resultsTitle);
    
    m_mainLayout->addWidget(m_resultsFrame);
}

void SecurityChecker::loadVulnerabilities()
{
    if (m_scanMode == LandingPage::ScanMode::Ollama) {
        startOllamaAnalysis();
        return;
    }
    
    // Modo local - usar JSON
    QString filePath = QCoreApplication::applicationDirPath() + "/vulnerabilities.json";
    if (!m_vulnerabilityManager->loadDefinitions(filePath)) {
        emit errorOccurred("Não foi possível carregar as definições de vulnerabilidades");
        return;
    }
    
    m_currentOS = m_vulnerabilityManager->getCurrentOS();
    updateOSDisplay();
    
    QVector<VulnerabilityDefinition> definitions = m_vulnerabilityManager->getDefinitionsForOS(m_currentOS);
    
    m_currentVulnerabilities.clear();
    for (const auto &def : definitions) {
        m_currentVulnerabilities.append(def);
    }
    
    m_checkResults.clear();
    m_checkResults.resize(m_currentVulnerabilities.size());
    
    updateProgress();
    updateCurrentCheck();
}

void SecurityChecker::startOllamaAnalysis()
{
    if (m_selectedModel.isEmpty()) {
        emit errorOccurred("Nenhum modelo Ollama selecionado");
        return;
    }
    
    // Mostrar status de carregamento
    m_checkTitle->setText("Analisando sistema com IA...");
    m_descriptionLabel->setText("Coletando informações do sistema e enviando para análise de IA");
    m_impactLabel->clear();
    m_severityLabel->clear();
    
    // Coletar informações do sistema
    SystemInfo systemInfo = collectSystemInfo();
    
    // Enviar para Ollama
    m_ollamaClient->analyzeSystemSecurity(systemInfo, m_selectedModel);
}

SystemInfo SecurityChecker::collectSystemInfo() const
{
    SystemInfo info;
    
    // Informações básicas do sistema
    info.osType = QSysInfo::kernelType();
    info.osVersion = QSysInfo::productVersion();
    info.kernelVersion = QSysInfo::kernelVersion();
    info.architecture = QSysInfo::currentCpuArchitecture();
    
    // Coletar serviços em execução (limitado para não sobrecarregar)
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    
#ifdef Q_OS_WIN
    process.start("sc", QStringList() << "query" << "state=" << "all");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.contains("SERVICE_NAME:")) {
                QString serviceName = line.split(':').last().trimmed();
                if (!serviceName.isEmpty() && info.runningServices.size() < 20) {
                    info.runningServices.append(serviceName);
                }
            }
        }
    }
    
    // Portas abertas
    process.start("netstat", QStringList() << "-an");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.contains("LISTENING") && info.openPorts.size() < 15) {
                info.openPorts.append(line.trimmed());
            }
        }
    }
#elif defined(Q_OS_LINUX)
    process.start("systemctl", QStringList() << "list-units" << "--type=service" << "--state=running" << "--no-pager");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.contains(".service") && line.contains("running") && info.runningServices.size() < 20) {
                QString serviceName = line.split(' ').first().trimmed();
                if (!serviceName.isEmpty()) {
                    info.runningServices.append(serviceName);
                }
            }
        }
    }
    
    // Portas abertas
    process.start("ss", QStringList() << "-tuln");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if ((line.contains("LISTEN") || line.contains("UNCONN")) && info.openPorts.size() < 15) {
                info.openPorts.append(line.trimmed());
            }
        }
    }
#elif defined(Q_OS_MACOS)
    process.start("launchctl", QStringList() << "list");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (!line.isEmpty() && info.runningServices.size() < 20) {
                QStringList parts = line.split('\t');
                if (parts.size() >= 3) {
                    QString serviceName = parts.last().trimmed();
                    if (!serviceName.isEmpty()) {
                        info.runningServices.append(serviceName);
                    }
                }
            }
        }
    }
    
    // Portas abertas
    process.start("netstat", QStringList() << "-an");
    if (process.waitForFinished(5000)) {
        QString output = process.readAllStandardOutput();
        QStringList lines = output.split('\n');
        for (const QString &line : lines) {
            if (line.contains("LISTEN") && info.openPorts.size() < 15) {
                info.openPorts.append(line.trimmed());
            }
        }
    }
#endif
    
    return info;
}

void SecurityChecker::updateProgress()
{
    if (m_currentVulnerabilities.isEmpty()) {
        m_progressBar->setValue(0);
        m_progressLabel->setText("0 de 0");
        return;
    }
    
    int total = m_currentVulnerabilities.size();
    int current = m_currentCheckIndex + 1;
    
    m_progressBar->setMaximum(total);
    m_progressBar->setValue(current);
    m_progressLabel->setText(QString("%1 de %2").arg(current).arg(total));
}

void SecurityChecker::updateCurrentCheck()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        showResults();
        return;
    }
    
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    
    m_checkTitle->setText(vuln.name);
    m_descriptionLabel->setText(QString("Descrição: %1").arg(vuln.description));
    m_impactLabel->setText(QString("Impacto: %1").arg(vuln.impact));
    
    QString severityText;
    QString severityClass;
    switch (vuln.severity) {
        case Severity::Alta:
            severityText = "Alta";
            severityClass = "severityHigh";
            break;
        case Severity::Media:
            severityText = "Média";
            severityClass = "severityMedium";
            break;
        case Severity::Baixa:
            severityText = "Baixa";
            severityClass = "severityLow";
            break;
    }
    
    m_severityLabel->setText(QString("Severidade: %1").arg(severityText));
    m_severityLabel->setObjectName(severityClass);
    m_severityLabel->style()->unpolish(m_severityLabel);
    m_severityLabel->style()->polish(m_severityLabel);
    
    m_fixCommandEdit->setPlainText(vuln.fix);
    m_fixCommandEdit->hide();
    m_resultFrame->hide();
    
    updateActionButtons();
    updateProgress();
}

void SecurityChecker::updateActionButtons()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    const CheckResult &result = m_checkResults[m_currentCheckIndex];
    
    m_startCheckButton->setVisible(result.status == CheckStatus::Pending);
    m_fixButton->setVisible(result.status == CheckStatus::Vulnerable);
    m_skipButton->setVisible(result.status == CheckStatus::Vulnerable);
    m_nextButton->setVisible(result.status == CheckStatus::Safe || result.status == CheckStatus::Fixed || result.status == CheckStatus::Skipped);
}

void SecurityChecker::updateOSDisplay()
{
    QString osText;
    if (m_currentOS == "windows") {
        osText = "Windows";
    } else if (m_currentOS == "linux") {
        osText = "Linux";
    } else if (m_currentOS == "macos") {
        osText = "macOS";
    } else {
        osText = "Sistema Desconhecido";
    }
    
    if (m_scanMode == LandingPage::ScanMode::Ollama) {
        osText += QString(" (IA: %1)").arg(m_selectedModel);
    }
    
    m_osDisplay->setText(osText);
}

void SecurityChecker::showResults()
{
    m_isCompleted = true;
    
    // Ocultar seções de verificação
    m_checkFrame->hide();
    
    // Mostrar resultados
    m_resultsFrame->show();
    
    // Calcular estatísticas
    int total = m_checkResults.size();
    int vulnerable = 0;
    int fixed = 0;
    int skipped = 0;
    int safe = 0;
    
    for (const CheckResult &result : m_checkResults) {
        switch (result.status) {
            case CheckStatus::Vulnerable: vulnerable++; break;
            case CheckStatus::Fixed: fixed++; break;
            case CheckStatus::Skipped: skipped++; break;
            case CheckStatus::Safe: safe++; break;
            default: break;
        }
    }
    
    // Criar resumo
    QFrame *summaryFrame = new QFrame();
    summaryFrame->setStyleSheet("background: white; border: 1px solid #e5e7eb; border-radius: 8px; padding: 16px;");
    
    QGridLayout *summaryLayout = new QGridLayout(summaryFrame);
    
    auto createStatCard = [](const QString &value, const QString &label, const QString &color) {
        QFrame *card = new QFrame();
        card->setStyleSheet(QString("background: %1; border-radius: 8px; padding: 16px;").arg(color));
        
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setAlignment(Qt::AlignCenter);
        
        QLabel *valueLabel = new QLabel(value);
        valueLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
        valueLabel->setAlignment(Qt::AlignCenter);
        
        QLabel *labelLabel = new QLabel(label);
        labelLabel->setStyleSheet("font-size: 12px; color: white;");
        labelLabel->setAlignment(Qt::AlignCenter);
        
        cardLayout->addWidget(valueLabel);
        cardLayout->addWidget(labelLabel);
        
        return card;
    };
    
    summaryLayout->addWidget(createStatCard(QString::number(total), "Total", "#3b82f6"), 0, 0);
    summaryLayout->addWidget(createStatCard(QString::number(vulnerable), "Vulneráveis", "#ef4444"), 0, 1);
    summaryLayout->addWidget(createStatCard(QString::number(fixed), "Corrigidas", "#10b981"), 0, 2);
    summaryLayout->addWidget(createStatCard(QString::number(skipped), "Ignoradas", "#6b7280"), 0, 3);
    
    m_resultsLayout->addWidget(summaryFrame);
    
    // Botão para voltar
    QPushButton *backButton = new QPushButton("🏠 Voltar ao Início");
    backButton->setObjectName("primaryButton");
    connect(backButton, &QPushButton::clicked, this, &SecurityChecker::backRequested);
    
    QHBoxLayout *backLayout = new QHBoxLayout();
    backLayout->setAlignment(Qt::AlignCenter);
    backLayout->addWidget(backButton);
    
    m_resultsLayout->addLayout(backLayout);
}

void SecurityChecker::resetChecker()
{
    m_currentCheckIndex = 0;
    m_isCompleted = false;
    m_checkResults.clear();
    m_currentVulnerabilities.clear();
    
    m_checkFrame->show();
    m_resultsFrame->hide();
    
    // Limpar layout de resultados
    QLayoutItem *item;
    while ((item = m_resultsLayout->takeAt(1)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

QString SecurityChecker::getSeverityColor(Severity severity) const
{
    switch (severity) {
        case Severity::Alta: return "#dc2626";
        case Severity::Media: return "#d97706";
        case Severity::Baixa: return "#ca8a04";
        default: return "#6b7280";
    }
}

QString SecurityChecker::getStatusText(CheckStatus status) const
{
    switch (status) {
        case CheckStatus::Pending: return "Pendente";
        case CheckStatus::Checking: return "Verificando...";
        case CheckStatus::Vulnerable: return "Vulnerável";
        case CheckStatus::Safe: return "Seguro";
        case CheckStatus::Skipped: return "Ignorado";
        case CheckStatus::Fixed: return "Corrigido";
        default: return "Desconhecido";
    }
}

QString SecurityChecker::getStatusColor(CheckStatus status) const
{
    switch (status) {
        case CheckStatus::Vulnerable: return "#dc2626";
        case CheckStatus::Safe: return "#10b981";
        case CheckStatus::Fixed: return "#2563eb";
        case CheckStatus::Skipped: return "#6b7280";
        default: return "#9ca3af";
    }
}

void SecurityChecker::onBackClicked()
{
    emit backRequested();
}

void SecurityChecker::onStartCheckClicked()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    
    m_checkResults[m_currentCheckIndex].status = CheckStatus::Checking;
    updateActionButtons();
    
    m_resultFrame->show();
    m_resultIcon->setText("🔍");
    m_resultText->setText("Verificando...");
    
    // Iniciar verificação
    m_systemChecker->checkVulnerability(vuln);
}

void SecurityChecker::onFixClicked()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    
    m_resultIcon->setText("🔧");
    m_resultText->setText("Aplicando correção...");
    
    // Iniciar correção
    m_systemChecker->fixVulnerability(vuln);
}

void SecurityChecker::onSkipClicked()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    m_checkResults[m_currentCheckIndex].status = CheckStatus::Skipped;
    m_checkResults[m_currentCheckIndex].isVulnerable = true; // Era vulnerável mas foi ignorado
    
    m_resultIcon->setText("⏭️");
    m_resultText->setText("Vulnerabilidade ignorada");
    
    updateActionButtons();
}

void SecurityChecker::onNextClicked()
{
    m_currentCheckIndex++;
    updateCurrentCheck();
}

void SecurityChecker::onCheckCompleted(const QString &id, bool isVulnerable)
{
    // Encontrar o índice da verificação
    int index = -1;
    for (int i = 0; i < m_currentVulnerabilities.size(); i++) {
        if (m_currentVulnerabilities[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1 || index != m_currentCheckIndex) {
        return;
    }
    
    m_checkResults[index].status = isVulnerable ? CheckStatus::Vulnerable : CheckStatus::Safe;
    m_checkResults[index].isVulnerable = isVulnerable;
    
    if (isVulnerable) {
        m_resultIcon->setText("⚠️");
        m_resultText->setText("Vulnerabilidade encontrada!");
        m_fixCommandEdit->show();
    } else {
        m_resultIcon->setText("✅");
        m_resultText->setText("Sistema seguro");
        m_fixCommandEdit->hide();
    }
    
    updateActionButtons();
}

void SecurityChecker::onFixCompleted(const QString &id, bool success)
{
    // Encontrar o índice da correção
    int index = -1;
    for (int i = 0; i < m_currentVulnerabilities.size(); i++) {
        if (m_currentVulnerabilities[i].id == id) {
            index = i;
            break;
        }
    }
    
    if (index == -1 || index != m_currentCheckIndex) {
        return;
    }
    
    if (success) {
        m_checkResults[index].status = CheckStatus::Fixed;
        m_resultIcon->setText("✅");
        m_resultText->setText("Vulnerabilidade corrigida com sucesso!");
    } else {
        m_resultIcon->setText("❌");
        m_resultText->setText("Falha ao aplicar correção");
    }
    
    updateActionButtons();
}

void SecurityChecker::onErrorOccurred(const QString &error)
{
    // Mostrar erro na interface
    m_resultFrame->show();
    m_resultIcon->setText("❌");
    m_resultText->setText(QString("Erro: %1").arg(error));
    
    // Reemitir o sinal para outros componentes se necessário
    emit errorOccurred(error);
}

void SecurityChecker::onSaveReportClicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Salvar Relatório",
        QString("SecurityCheck_Report_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "Arquivos de Texto (*.txt)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erro", "Não foi possível salvar o relatório");
        return;
    }
    
    QTextStream out(&file);
    out << "=== RELATÓRIO DE SEGURANÇA ===" << Qt::endl;
    out << "Data: " << QDateTime::currentDateTime().toString() << Qt::endl;
    out << "Sistema: " << m_currentOS << Qt::endl;
    out << Qt::endl;
    
    for (int i = 0; i < m_checkResults.size(); i++) {
        const CheckResult &result = m_checkResults[i];
        const VulnerabilityDefinition &vuln = m_currentVulnerabilities[i];
        
        out << QString("[%1] %2").arg(getStatusText(result.status), vuln.name) << Qt::endl;
        out << "Descrição: " << vuln.description << Qt::endl;
        out << "Impacto: " << vuln.impact << Qt::endl;
        out << "Correção: " << vuln.fix << Qt::endl;
        out << Qt::endl;
    }
    
    QMessageBox::information(this, "Sucesso", "Relatório salvo com sucesso!");
}

void SecurityChecker::onOllamaVulnerabilitiesReceived(const QVector<VulnerabilityDefinition> &vulnerabilities)
{
    m_currentVulnerabilities = vulnerabilities;
    m_checkResults.clear();
    m_checkResults.resize(vulnerabilities.size());
    
    // Inicializar resultados
    for (int i = 0; i < m_checkResults.size(); i++) {
        m_checkResults[i].status = CheckStatus::Pending;
        m_checkResults[i].isVulnerable = false;
    }
    
    m_currentCheckIndex = 0;
    updateOSDisplay();
    updateProgress();
    updateCurrentCheck();
}

void SecurityChecker::onOllamaError(const QString &error)
{
    m_checkTitle->setText("Erro na Análise de IA");
    m_descriptionLabel->setText(QString("Erro ao comunicar com o Ollama: %1").arg(error));
    m_impactLabel->setText("Tente novamente ou use a verificação local.");
    m_severityLabel->clear();
    
    m_resultFrame->show();
    m_resultIcon->setText("❌");
    m_resultText->setText("Falha na análise de IA");
}
    
    // Header