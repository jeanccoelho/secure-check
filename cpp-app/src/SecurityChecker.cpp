#include "SecurityChecker.h"
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QScrollArea>
#include <QGridLayout>
#include <QCheckBox>
#include <QInputDialog>
#include <QGroupBox>
#include <QFileDialog>
#include <QTextStream>
#include <QDateTime>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>

SecurityChecker::SecurityChecker(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_vulnerabilityManager(nullptr)
    , m_systemChecker(nullptr)
    , m_currentCheckIndex(0)
    , m_isCompleted(false)
    , m_ollamaClient(nullptr)
    , m_isOllamaModeEnabled(false)
    , m_isOllamaAnalysisRunning(false)
{
    setupUI();
    
    // Inicializar managers
    m_vulnerabilityManager = new VulnerabilityManager(this);
    m_systemChecker = new SystemChecker(this);
    m_ollamaClient = new OllamaClient(this);
    
    // Conectar sinais
    m_isOllamaModeEnabled = false; // Será definido via setAIMode()
            this, &SecurityChecker::onCheckCompleted);
    connect(m_systemChecker, &SystemChecker::fixCompleted,
            this, &SecurityChecker::onFixCompleted);
    connect(m_systemChecker, &SystemChecker::errorOccurred,
            this, &SecurityChecker::onErrorOccurred);
    connect(m_systemChecker, &SystemChecker::systemInfoCollected,
            this, &SecurityChecker::onSystemInfoCollected);
    
    // Conectar sinais do Ollama
    connect(m_ollamaClient, &OllamaClient::vulnerabilitiesFound,
            this, &SecurityChecker::onOllamaVulnerabilitiesFound);
    connect(m_ollamaClient, &OllamaClient::analysisCompleted,
            this, &SecurityChecker::onOllamaAnalysisCompleted);
    connect(m_ollamaClient, &OllamaClient::connectionTestCompleted,
            this, &SecurityChecker::onOllamaConnectionTest);
    connect(m_ollamaClient, &OllamaClient::errorOccurred,
            this, &SecurityChecker::onOllamaError);
    connect(m_ollamaClient, &OllamaClient::progressUpdate,
            this, &SecurityChecker::onOllamaProgress);
    
    // Carregar vulnerabilidades
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
    setupOllamaUI();
    createCheckSection();
    createOllamaSection();
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
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(16);
    
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(32, 0, 32, 0);
    
    // Botão voltar
    m_backButton = new QPushButton("← Voltar");
    m_backButton->setObjectName("backButton");
    connect(m_backButton, &QPushButton::clicked, this, &SecurityChecker::onBackClicked);
    
    headerLayout->addWidget(m_backButton);
    QLabel *osLabel = new QLabel("Sistema Operacional:");
    headerLayout->addStretch();
    
    // Título da página (será atualizado baseado no modo)
    m_titleLabel = new QLabel("Verificação de Segurança");
    m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #1f2937;");
    
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();
    
    m_mainLayout->addWidget(headerFrame);
}

void SecurityChecker::setAIMode(bool enabled)
{
    m_isOllamaModeEnabled = enabled;
    
    // Atualizar título
    if (m_titleLabel) {
        m_titleLabel->setText(enabled ? "🤖 Verificação com IA" : "🔍 Verificação Estática");
    }
    
    // Se modo IA estiver ativado, iniciar coleta de dados automaticamente
    if (enabled && m_ollamaClient) {
        // Testar conexão com Ollama
        m_ollamaClient->testConnection();
    }
    
    // Recarregar vulnerabilidades baseado no modo
    loadVulnerabilities();
}
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

void SecurityChecker::setupOllamaUI()
{
    m_ollamaFrame = new QFrame();
    m_ollamaFrame->setObjectName("ollamaFrame");
    m_ollamaFrame->setStyleSheet(
        "QFrame#ollamaFrame { "
        "background: white; "
        "border: 1px solid #e5e7eb; "
        "border-radius: 12px; "
        "margin: 8px; "
        "}"
    );
    
    QVBoxLayout *ollamaMainLayout = new QVBoxLayout(m_ollamaFrame);
    ollamaMainLayout->setContentsMargins(32, 24, 32, 24);
    ollamaMainLayout->setSpacing(16);
    
    // Header do Ollama
    QLabel *ollamaTitle = new QLabel("Análise com IA (Ollama)");
    ollamaTitle->setStyleSheet("font-size: 16px; font-weight: 600; color: #1f2937; background: white;");
    
    m_ollamaCheckBox = new QCheckBox("Usar análise com IA");
    m_ollamaCheckBox->setStyleSheet(
        "QCheckBox { "
        "QProgressBar::chunk { "
        "background: #2563eb; "
        "border-radius: 6px; "
        "}"
    );
    m_ollamaProgressBar->setRange(0, 0); // Indeterminado
    
    ollamaMainLayout->addLayout(ollamaHeaderLayout);
    ollamaMainLayout->addLayout(ollamaControlsLayout);
    ollamaMainLayout->addWidget(m_ollamaProgressBar);
    
    m_mainLayout->addWidget(m_ollamaFrame);
    
    // Inicialmente oculto
    m_ollamaFrame->setVisible(false);
}

void SecurityChecker::createOllamaSection()
{
    // Frame para opções do Ollama
    QGroupBox *ollamaGroup = new QGroupBox("🤖 Análise com Inteligência Artificial");
    ollamaGroup->setStyleSheet(R"(
        QGroupBox {
            font-weight: bold;
            border: 2px solid #e5e7eb;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
    )");
    
    QVBoxLayout *ollamaLayout = new QVBoxLayout(ollamaGroup);
    
    // Checkbox para ativar modo IA
    m_ollamaCheckBox = new QCheckBox("Usar análise com IA (Ollama)");
    m_ollamaCheckBox->setToolTip("Ativar análise inteligente de vulnerabilidades usando IA");
    connect(m_ollamaCheckBox, &QCheckBox::toggled, this, &SecurityChecker::onOllamaModeToggled);
    
    // Layout horizontal para controles
    QHBoxLayout *controlsLayout = new QHBoxLayout();
    
    // Botão de configuração
    m_ollamaConfigButton = new QPushButton("⚙️ Configurar");
    m_ollamaConfigButton->setEnabled(false);
    m_ollamaConfigButton->setToolTip("Configurar endpoint e modelo do Ollama");
    connect(m_ollamaConfigButton, &QPushButton::clicked, this, &SecurityChecker::onOllamaConfigClicked);
    
    // Botão de análise
    m_ollamaAnalysisButton = new QPushButton("🔍 Analisar com IA");
    m_ollamaAnalysisButton->setEnabled(false);
    m_ollamaAnalysisButton->setToolTip("Iniciar análise de vulnerabilidades com IA");
    connect(m_ollamaAnalysisButton, &QPushButton::clicked, this, &SecurityChecker::onOllamaAnalysisClicked);
    
    controlsLayout->addWidget(m_ollamaConfigButton);
    controlsLayout->addWidget(m_ollamaAnalysisButton);
    controlsLayout->addStretch();
    
    // Status do Ollama
    m_ollamaStatusLabel = new QLabel("Status: Desabilitado");
    m_ollamaStatusLabel->setStyleSheet("color: #6b7280; font-size: 12px;");
    
    // Barra de progresso do Ollama
    m_ollamaProgressBar = new QProgressBar();
    m_ollamaProgressBar->setVisible(false);
    m_ollamaProgressBar->setRange(0, 0); // Indeterminado
    
    ollamaLayout->addWidget(m_ollamaCheckBox);
    ollamaLayout->addLayout(controlsLayout);
    ollamaLayout->addWidget(m_ollamaStatusLabel);
    ollamaLayout->addWidget(m_ollamaProgressBar);
    
    m_mainLayout->addWidget(ollamaGroup);
}

void SecurityChecker::createCheckSection()
{
    m_checkFrame = new QFrame();
    m_checkFrame->setObjectName("checkFrame");
    
    QVBoxLayout *checkLayout = new QVBoxLayout(m_checkFrame);
    checkLayout->setContentsMargins(32, 32, 32, 32);
    checkLayout->setSpacing(24);
    
    // Header com ícone e título
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setAlignment(Qt::AlignCenter);
    titleLayout->setSpacing(16);
    
    QLabel *iconLabel = new QLabel("🛡️");
    iconLabel->setStyleSheet("font-size: 48px; background: white;");
    
    m_checkTitle = new QLabel("Carregando...");
    m_checkTitle->setObjectName("titleLabel");
    m_checkTitle->setAlignment(Qt::AlignCenter);
    
    titleLayout->addWidget(iconLabel);
    titleLayout->addWidget(m_checkTitle);
    
    // Severidade
    QHBoxLayout *severityLayout = new QHBoxLayout();
    severityLayout->setAlignment(Qt::AlignCenter);
    
    m_severityLabel = new QLabel();
    m_severityLabel->setAlignment(Qt::AlignCenter);
    
    severityLayout->addWidget(m_severityLabel);
    
    // Seções de informação
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(16);
    
    // Descrição
    QLabel *descLabel = new QLabel("Descrição:");
    descLabel->setObjectName("sectionTitle");
    
    m_descriptionLabel = new QLabel();
    m_descriptionLabel->setObjectName("description");
    m_descriptionLabel->setWordWrap(true);
    
    // Impacto
    QLabel *impactLabel = new QLabel("Impacto:");
    impactLabel->setObjectName("sectionTitle");
    
    m_impactLabel = new QLabel();
    m_impactLabel->setObjectName("description");
    m_impactLabel->setWordWrap(true);
    
    infoLayout->addWidget(descLabel);
    infoLayout->addWidget(m_descriptionLabel);
    infoLayout->addSpacing(8);
    infoLayout->addWidget(impactLabel);
    infoLayout->addWidget(m_impactLabel);
    
    checkLayout->addLayout(titleLayout);
    checkLayout->addLayout(severityLayout);
    checkLayout->addLayout(infoLayout);
    
    m_mainLayout->addWidget(m_checkFrame);
    
    // Frame de resultado (inicialmente oculto)
    m_resultFrame = new QFrame();
    m_resultFrame->setObjectName("resultFrame");
    m_resultFrame->setVisible(false);
    
    QVBoxLayout *resultLayout = new QVBoxLayout(m_resultFrame);
    resultLayout->setContentsMargins(32, 24, 32, 24);
    resultLayout->setSpacing(16);
    
    // Header do resultado
    QHBoxLayout *resultHeaderLayout = new QHBoxLayout();
    resultHeaderLayout->setAlignment(Qt::AlignCenter);
    resultHeaderLayout->setSpacing(12);
    
    m_resultIcon = new QLabel();
    m_resultIcon->setStyleSheet("font-size: 32px; background: white;");
    
    m_resultText = new QLabel();
    m_resultText->setObjectName("resultText");
    
    resultHeaderLayout->addWidget(m_resultIcon);
    resultHeaderLayout->addWidget(m_resultText);
    
    resultLayout->addLayout(resultHeaderLayout);
    
    m_mainLayout->addWidget(m_resultFrame);
    
    // Campo de comando de correção (inicialmente oculto)
    m_fixCommandEdit = new QTextEdit();
    m_fixCommandEdit->setVisible(false);
    m_fixCommandEdit->setMaximumHeight(120);
    m_fixCommandEdit->setReadOnly(true);
    
    m_mainLayout->addWidget(m_fixCommandEdit);
}

void SecurityChecker::createActionButtons()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(16);
    buttonLayout->setContentsMargins(32, 16, 32, 32);
    
    m_startCheckButton = new QPushButton("🛡️ Iniciar Verificação");
    m_startCheckButton->setObjectName("primaryButton");
    connect(m_startCheckButton, &QPushButton::clicked, this, &SecurityChecker::onStartCheckClicked);
    
    m_fixButton = new QPushButton("🔧 Corrigir e Continuar");
    m_fixButton->setObjectName("successButton");
    m_fixButton->setVisible(false);
    connect(m_fixButton, &QPushButton::clicked, this, &SecurityChecker::onFixClicked);
    
    m_skipButton = new QPushButton("⏭️ Pular sem Corrigir");
    m_skipButton->setObjectName("warningButton");
    m_skipButton->setVisible(false);
    connect(m_skipButton, &QPushButton::clicked, this, &SecurityChecker::onSkipClicked);
    
    m_nextButton = new QPushButton("Continuar");
    m_nextButton->setObjectName("primaryButton");
    m_nextButton->setVisible(false);
    connect(m_nextButton, &QPushButton::clicked, this, &SecurityChecker::onNextClicked);
    
    buttonLayout->addWidget(m_startCheckButton);
    buttonLayout->addWidget(m_fixButton);
    buttonLayout->addWidget(m_skipButton);
    buttonLayout->addWidget(m_nextButton);
    
    m_mainLayout->addLayout(buttonLayout);
}

void SecurityChecker::createResultsSection()
{
    m_resultsFrame = new QFrame();
    m_resultsFrame->setObjectName("resultsFrame");
    m_resultsFrame->setVisible(false);
    
    QVBoxLayout *resultsMainLayout = new QVBoxLayout(m_resultsFrame);
    resultsMainLayout->setContentsMargins(32, 32, 32, 32);
    resultsMainLayout->setSpacing(24);
    
    // Header dos resultados
    QHBoxLayout *resultsHeaderLayout = new QHBoxLayout();
    resultsHeaderLayout->setAlignment(Qt::AlignCenter);
    resultsHeaderLayout->setSpacing(12);
    
    QLabel *resultsIcon = new QLabel("✅");
    resultsIcon->setStyleSheet("font-size: 48px; background: white;");
    
    QLabel *resultsTitle = new QLabel("Verificação Concluída!");
    resultsTitle->setObjectName("titleLabel");
    
    resultsHeaderLayout->addWidget(resultsIcon);
    resultsHeaderLayout->addWidget(resultsTitle);
    
    QLabel *resultsSubtitle = new QLabel("Resumo da verificação de segurança");
    resultsSubtitle->setStyleSheet("font-size: 16px; color: #6b7280; background: white;");
    resultsSubtitle->setAlignment(Qt::AlignCenter);
    
    m_resultsLayout = new QVBoxLayout();
    
    resultsMainLayout->addLayout(resultsHeaderLayout);
    resultsMainLayout->addWidget(resultsSubtitle);
    resultsMainLayout->addSpacing(16);
    resultsMainLayout->addLayout(m_resultsLayout);
    
    m_mainLayout->addWidget(m_resultsFrame);
}

void SecurityChecker::loadVulnerabilities()
{
    QString filePath = QApplication::applicationDirPath() + "/vulnerabilities.json";
    
    if (!m_vulnerabilityManager->loadDefinitions(filePath)) {
        QMessageBox::critical(this, "Erro", 
            "Não foi possível carregar as definições de vulnerabilidades.\n"
            "Verifique se o arquivo 'vulnerabilities.json' está presente.");
        return;
    }
    
    // Detectar OS atual e configurar
    m_currentOS = m_vulnerabilityManager->getCurrentOS();
    qDebug() << "OS carregado:" << m_currentOS;
    
    // Atualizar display do OS no header
    updateOSDisplay();
    
    // Verificar se o OS é suportado
    if (m_currentOS == "unknown" || m_currentOS.isEmpty()) {
        QMessageBox::critical(this, "Sistema Não Suportado", 
            "Não foi possível detectar o sistema operacional ou ele não é suportado.\n\n"
            "Sistemas suportados:\n"
            "• Windows\n"
            "• Linux\n"
            "• macOS\n\n"
            "Verifique se está executando em um sistema compatível.");
        return;
    }
    
    // Carregar vulnerabilidades para o OS detectado
    m_currentVulnerabilities = m_vulnerabilityManager->getDefinitionsForOS(m_currentOS);
    
    resetChecker();
    updateProgress();
    updateCurrentCheck();
}

void SecurityChecker::resetChecker()
{
    m_currentCheckIndex = 0;
    m_checkResults.clear();
    m_isCompleted = false;
    m_isOllamaAnalysisRunning = false;
    
    // Resetar UI
    m_resultFrame->setVisible(false);
    m_fixCommandEdit->setVisible(false);
    m_resultsFrame->setVisible(false);
    m_checkFrame->setVisible(true);
    
    updateActionButtons();
    updateOllamaStatus();
}

void SecurityChecker::updateProgress()
{
    int total = m_currentVulnerabilities.size();
    int current = m_currentCheckIndex;
    
    if (total > 0) {
        m_progressLabel->setText(QString("%1 de %2").arg(current).arg(total));
        m_progressBar->setValue((current * 100) / total);
    } else {
        m_progressLabel->setText("Nenhuma verificação disponível");
        m_progressBar->setValue(0);
    }
}

void SecurityChecker::updateCurrentCheck()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        showResults();
        return;
    }
    
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    
    m_checkTitle->setText(vuln.name);
    m_descriptionLabel->setText(vuln.description);
    m_impactLabel->setText(vuln.impact);
    
    // Configurar label de severidade
    QString severityText = QString("Severidade: %1").arg(
        vuln.severity == Severity::Alta ? "Alta" :
        vuln.severity == Severity::Media ? "Média" : "Baixa"
    );
    
    QString severityStyle = getSeverityColor(vuln.severity);
    m_severityLabel->setText(severityText);
    m_severityLabel->setObjectName(severityStyle);
    m_severityLabel->style()->unpolish(m_severityLabel);
    m_severityLabel->style()->polish(m_severityLabel);
    
    updateActionButtons();
}

void SecurityChecker::updateActionButtons()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    // Verificar se já temos resultado para esta verificação
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    CheckResult *result = nullptr;
    
    for (auto &r : m_checkResults) {
        if (r.id == vuln.id) {
            result = &r;
            break;
        }
    }
    
    // Mostrar botões apropriados
    m_startCheckButton->setVisible(!result);
    m_fixButton->setVisible(result && result->status == CheckStatus::Vulnerable);
    m_skipButton->setVisible(result && result->status == CheckStatus::Vulnerable);
    m_nextButton->setVisible(result && result->status == CheckStatus::Safe);
    
    // Mostrar resultado se disponível
    if (result) {
        m_resultFrame->setVisible(true);
        
        if (result->status == CheckStatus::Vulnerable) {
            m_resultIcon->setText("⚠️");
            m_resultText->setText("Vulnerabilidade Encontrada!");
            m_resultText->setStyleSheet("color: #dc2626; font-size: 18px; font-weight: 600; background: white;");
            
            // Mostrar comando de correção
            m_fixCommandEdit->setPlainText(vuln.fix);
            m_fixCommandEdit->setVisible(true);
        } else if (result->status == CheckStatus::Safe) {
            m_resultIcon->setText("✅");
            m_resultText->setText("Sistema Seguro");
            m_resultText->setStyleSheet("color: #059669; font-size: 18px; font-weight: 600; background: white;");
            m_fixCommandEdit->setVisible(false);
        }
    } else {
        m_resultFrame->setVisible(false);
        m_fixCommandEdit->setVisible(false);
    }
}

    
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    
    // Desabilitar botão durante verificação
    m_startCheckButton->setEnabled(false);
    m_startCheckButton->setText("Verificando...");
    
    // Iniciar verificação
    m_systemChecker->checkVulnerability(vuln);
}

void SecurityChecker::onFixClicked()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    
    // Desabilitar botões durante correção
    m_fixButton->setEnabled(false);
    m_skipButton->setEnabled(false);
    m_fixButton->setText("Corrigindo...");
    
    // Iniciar correção
    m_systemChecker->fixVulnerability(vuln);
}

void SecurityChecker::onSkipClicked()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    
    // Marcar como pulado
    CheckResult result;
    result.id = vuln.id;
    result.status = CheckStatus::Skipped;
    result.isVulnerable = true; // Era vulnerável mas foi pulado
    
    // Atualizar ou adicionar resultado
    bool found = false;
    for (auto &r : m_checkResults) {
        if (r.id == vuln.id) {
            r = result;
            found = true;
            break;
        }
    }
    if (!found) {
        m_checkResults.append(result);
    }
    
    onNextClicked();
}

void SecurityChecker::onNextClicked()
{
    m_currentCheckIndex++;
    updateProgress();
    
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        showResults();
    } else {
        updateCurrentCheck();
    }
}

void SecurityChecker::onCheckCompleted(const QString &id, bool isVulnerable)
{
    // Reabilitar botão
    m_startCheckButton->setEnabled(true);
    m_startCheckButton->setText("🛡️ Iniciar Verificação");
    
    // Salvar resultado
    CheckResult result;
    result.id = id;
    result.status = isVulnerable ? CheckStatus::Vulnerable : CheckStatus::Safe;
    result.isVulnerable = isVulnerable;
    
    // Atualizar ou adicionar resultado
    bool found = false;
    for (auto &r : m_checkResults) {
        if (r.id == id) {
            r = result;
            found = true;
            break;
        }
    }
    if (!found) {
        m_checkResults.append(result);
    }
    
    updateActionButtons();
}

void SecurityChecker::onFixCompleted(const QString &id, bool success)
{
    // Reabilitar botões
    m_fixButton->setEnabled(true);
    m_skipButton->setEnabled(true);
    m_fixButton->setText("🔧 Corrigir e Continuar");
    
    if (success) {
        // Marcar como corrigido
        for (auto &r : m_checkResults) {
            if (r.id == id) {
                r.status = CheckStatus::Fixed;
                break;
            }
        }
        
        // Avançar automaticamente
        onNextClicked();
    } else {
        QMessageBox::warning(this, "Erro na Correção", 
            "Não foi possível corrigir a vulnerabilidade automaticamente. "
            "Você pode tentar executar o comando manualmente ou pular esta verificação.");
    }
}

void SecurityChecker::onErrorOccurred(const QString &error)
{
    QMessageBox::critical(this, "Erro", error);
    
    // Reabilitar botões
    m_startCheckButton->setEnabled(true);
    m_startCheckButton->setText("🛡️ Iniciar Verificação");
    m_fixButton->setEnabled(true);
    m_skipButton->setEnabled(true);
    m_fixButton->setText("🔧 Corrigir e Continuar");
}

void SecurityChecker::onOllamaModeToggled(bool enabled)
{
    m_isOllamaModeEnabled = enabled;
    
    if (enabled) {
        // Mostrar seção Ollama
        m_ollamaFrame->setVisible(true);
        
        // Ocultar verificação tradicional
        m_checkFrame->setVisible(false);
        m_resultFrame->setVisible(false);
        m_fixCommandEdit->setVisible(false);
        
        // Atualizar botões
        m_startCheckButton->setVisible(false);
        m_fixButton->setVisible(false);
        m_skipButton->setVisible(false);
        m_nextButton->setVisible(false);
        
        updateOllamaStatus();
    } else {
        // Ocultar seção Ollama
        m_ollamaFrame->setVisible(false);
        
        // Mostrar verificação tradicional
        m_checkFrame->setVisible(true);
        updateCurrentCheck();
        updateActionButtons();
    }
    
    m_ollamaConfigButton->setEnabled(enabled);
    m_ollamaAnalysisButton->setEnabled(enabled);
    
    if (enabled) {
        // Testar conexão quando ativado
        m_ollamaClient->testConnection();
    }
    
    updateOllamaStatus();
}

void SecurityChecker::onOllamaConfigClicked()
{
    showOllamaConfigDialog();
}

void SecurityChecker::showOllamaConfigDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Configuração do Ollama");
    dialog.setModal(true);
    dialog.resize(400, 200);
    
    QFormLayout *layout = new QFormLayout(&dialog);
    
    // Campos de configuração
    QLineEdit *endpointEdit = new QLineEdit();
    QLineEdit *modelEdit = new QLineEdit();
    QSpinBox *timeoutSpin = new QSpinBox();
    timeoutSpin->setRange(5000, 300000);
    timeoutSpin->setSuffix(" ms");
    
    // Carregar configuração atual
    OllamaConfig config = m_ollamaClient->getConfig();
    endpointEdit->setText(config.endpoint);
    modelEdit->setText(config.model);
    timeoutSpin->setValue(config.timeout);
    
    layout->addRow("Endpoint:", endpointEdit);
    layout->addRow("Modelo:", modelEdit);
    layout->addRow("Timeout:", timeoutSpin);
    
    // Botões
    QDialogButtonBox *buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addRow(buttons);
    
    if (dialog.exec() == QDialog::Accepted) {
        // Aplicar nova configuração
        config.endpoint = endpointEdit->text();
        config.model = modelEdit->text();
        config.timeout = timeoutSpin->value();
        config.enabled = m_isOllamaModeEnabled;
        
        m_ollamaClient->setConfig(config);
        
        // Testar nova conexão
        if (m_isOllamaModeEnabled) {
            m_ollamaClient->testConnection();
        }
    }
}

void SecurityChecker::onOllamaAnalysisClicked()
{
    if (m_isOllamaAnalysisRunning) {
        return;
    }
    
    m_isOllamaAnalysisRunning = true;
    m_ollamaProgressBar->setVisible(true);
    m_ollamaAnalysisButton->setEnabled(false);
    m_ollamaAnalysisButton->setText("Analisando...");
    
    updateOllamaStatus();
    
    // Coletar informações do sistema de forma assíncrona
    m_systemChecker->collectSystemInfoAsync();
}

void SecurityChecker::onSystemInfoCollected(const SystemInfo &info)
{
    // Enviar dados para análise do Ollama
    m_ollamaClient->analyzeSystem(info);
}

void SecurityChecker::onOllamaVulnerabilitiesFound(const QVector<VulnerabilityDefinition> &vulnerabilities)
{
    // Substituir vulnerabilidades atuais pelas encontradas pelo Ollama
    m_currentVulnerabilities = vulnerabilities;
    
    // Resetar resultados
    m_checkResults.clear();
    m_currentCheckIndex = 0;
    
    // Atualizar interface
    updateProgress();
    updateCurrentCheck();
    updateActionButtons();
    
    m_progressLabel->setText(QString("Ollama encontrou %1 vulnerabilidades para análise")
                            .arg(vulnerabilities.size()));
    
    // Marcar automaticamente como vulnerável (Ollama já identificou)
    CheckResult result;
    result.id = vulnerabilities[0].id;
    result.status = CheckStatus::Vulnerable;
    result.isVulnerable = true;
    m_checkResults.append(result);
    
    updateActionButtons();
}

void SecurityChecker::onOllamaAnalysisCompleted(bool success)
{
    m_isOllamaAnalysisRunning = false;
    m_ollamaProgressBar->setVisible(false);
    m_ollamaAnalysisButton->setEnabled(true);
    m_ollamaAnalysisButton->setText("🔍 Analisar com IA");
    
    updateOllamaStatus();
    
    if (success) {
        m_ollamaStatusLabel->setText("Análise concluída com sucesso");
        
        // Mostrar seção de verificação com resultados do Ollama
        m_checkFrame->setVisible(true);
        
        if (m_currentVulnerabilities.isEmpty()) {
            QMessageBox::information(this, "Análise IA", 
                "🎉 Parabéns! A IA não encontrou vulnerabilidades críticas em seu sistema.");
        }
        
        QMessageBox::information(this, "Análise Concluída", 
                                "Análise com IA concluída com sucesso!");
    } else {
        m_ollamaStatusLabel->setText("Erro na análise");
        QMessageBox::warning(this, "Erro na Análise", 
                            "Falha na análise com IA. Verifique a conexão com o Ollama.");
    }
}

void SecurityChecker::onOllamaConnectionTest(bool success, const QString &message)
{
    if (success) {
        m_ollamaStatusLabel->setText("✅ Ollama conectado");
        m_ollamaStatusLabel->setStyleSheet("color: #059669; font-size: 12px; background: white;");
        m_ollamaAnalysisButton->setEnabled(true);
    } else {
        m_ollamaStatusLabel->setText("❌ Ollama desconectado");
        m_ollamaStatusLabel->setStyleSheet("color: #dc2626; font-size: 12px; background: white;");
        m_ollamaAnalysisButton->setEnabled(false);
        
        QMessageBox::warning(this, "Erro de Conexão", 
            QString("Não foi possível conectar ao Ollama:\n%1").arg(message));
    }
    
    updateOllamaStatus();
    
    if (!success) {
        m_ollamaStatusLabel->setText(QString("Status: Erro - %1").arg(message));
        m_ollamaStatusLabel->setStyleSheet("color: #ef4444; font-size: 12px;");
    }
}

void SecurityChecker::onOllamaError(const QString &error)
{
    m_ollamaStatusLabel->setText("❌ Erro");
    m_ollamaStatusLabel->setStyleSheet("color: #dc2626; font-size: 12px; background: white;");
    
    QMessageBox::critical(this, "Erro Ollama", error);
    
    m_isOllamaAnalysisRunning = false;
    m_ollamaProgressBar->setVisible(false);
    m_ollamaAnalysisButton->setEnabled(true);
    m_ollamaAnalysisButton->setText("🔍 Analisar com IA");
    
    updateOllamaStatus();
    
    QMessageBox::critical(this, "Erro do Ollama", 
                         QString("Erro na comunicação com Ollama:\n%1").arg(error));
}

void SecurityChecker::onOllamaProgress(const QString &status)
{
    m_ollamaStatusLabel->setText(status);
    m_progressLabel->setText(status);
}

void SecurityChecker::showResults()
{
    m_isCompleted = true;
    
    // Ocultar seção de verificação
    m_checkFrame->setVisible(false);
    m_resultFrame->setVisible(false);
    m_fixCommandEdit->setVisible(false);
    
    // Ocultar botões de ação também
    m_startCheckButton->setVisible(false);
    m_fixButton->setVisible(false);
    m_skipButton->setVisible(false);
    m_nextButton->setVisible(false);
    
    // Mostrar seção de resultados
    m_resultsFrame->setVisible(true);
    
    // Atualizar progresso para 100%
    m_progressBar->setValue(100);
    m_progressLabel->setText(QString("%1 de %1").arg(m_currentVulnerabilities.size()));
    
    // Limpar layout anterior
    QLayoutItem *item;
    while ((item = m_resultsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    
    // Estatísticas
    int total = m_currentVulnerabilities.size();
    int vulnerable = 0;
    int fixed = 0;
    int skipped = 0;
    int safe = 0;
    
    for (const auto &result : m_checkResults) {
        switch (result.status) {
            case CheckStatus::Vulnerable: vulnerable++; break;
            case CheckStatus::Fixed: fixed++; break;
            case CheckStatus::Skipped: skipped++; break;
            case CheckStatus::Safe: safe++; break;
            default: break;
        }
    }
    
    // === SEÇÃO DE ESTATÍSTICAS ===
    // Container para estatísticas com layout horizontal
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(20);
    statsLayout->setAlignment(Qt::AlignCenter);
    
    auto createStatCard = [](const QString &number, const QString &label, const QString &bgColor) {
        QFrame *card = new QFrame();
        card->setFixedSize(180, 120);
        card->setStyleSheet(QString(
            "QFrame { "
            "background: %1; "
            "border-radius: 12px; "
            "border: none; "
            "}"
        ).arg(bgColor));
        
        QVBoxLayout *layout = new QVBoxLayout(card);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->setAlignment(Qt::AlignCenter);
        
        QLabel *numLabel = new QLabel(number);
        numLabel->setStyleSheet(
            "font-size: 36px; "
            "font-weight: 700; "
            "color: white; "
            "background: transparent; "
            "border: none;"
        );
        numLabel->setAlignment(Qt::AlignCenter);
        
        QLabel *textLabel = new QLabel(label);
        textLabel->setStyleSheet(
            "font-size: 13px; "
            "color: rgba(255, 255, 255, 0.9); "
            "background: transparent; "
            "font-weight: 600; "
            "border: none;"
        );
        textLabel->setAlignment(Qt::AlignCenter);
        textLabel->setWordWrap(true);
        
        layout->addWidget(numLabel);
        layout->addWidget(textLabel);
        
        return card;
    };
    
    // Adicionar cards de estatísticas
    statsLayout->addWidget(createStatCard(QString::number(total), "Total de\nVerificações", "#3b82f6"));
    statsLayout->addWidget(createStatCard(QString::number(vulnerable + skipped), "Vulnerabilidades\nEncontradas", "#ef4444"));
    statsLayout->addWidget(createStatCard(QString::number(fixed), "Corrigidas", "#10b981"));
    statsLayout->addWidget(createStatCard(QString::number(skipped), "Ignoradas", "#6b7280"));
    
    m_resultsLayout->addLayout(statsLayout);
    m_resultsLayout->addSpacing(40);
    
    // === LISTA DETALHADA DE VERIFICAÇÕES ===
    if (!m_checkResults.isEmpty()) {
        QLabel *detailsTitle = new QLabel("📋 Detalhes das Verificações");
        detailsTitle->setStyleSheet(
            "font-size: 22px; "
            "font-weight: 700; "
            "color: #1f2937; "
            "background: transparent; "
            "border: none; "
            "margin: 0px; "
            "padding: 0px;"
        );
        detailsTitle->setAlignment(Qt::AlignCenter);
        m_resultsLayout->addWidget(detailsTitle);
        m_resultsLayout->addSpacing(20);
        
        // Scroll area para a lista de verificações
        QScrollArea *scrollArea = new QScrollArea();
        scrollArea->setWidgetResizable(true);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setMaximumHeight(300);
        scrollArea->setStyleSheet(
            "QScrollArea { "
            "border: 1px solid #e5e7eb; "
            "border-radius: 8px; "
            "background: white; "
            "} "
            "QScrollBar:vertical { "
            "background: #f3f4f6; "
            "width: 8px; "
            "border-radius: 4px; "
            "} "
            "QScrollBar::handle:vertical { "
            "background: #d1d5db; "
            "border-radius: 4px; "
            "min-height: 20px; "
            "} "
            "QScrollBar::handle:vertical:hover { "
            "background: #9ca3af; "
            "}"
        );
        
        QWidget *scrollWidget = new QWidget();
        QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);
        scrollLayout->setContentsMargins(16, 16, 16, 16);
        scrollLayout->setSpacing(12);
        
        for (const auto &result : m_checkResults) {
            const VulnerabilityDefinition *vuln = nullptr;
            for (const auto &v : m_currentVulnerabilities) {
                if (v.id == result.id) {
                    vuln = &v;
                    break;
                }
            }
            
            if (!vuln) continue;
            
            QFrame *resultCard = new QFrame();
            resultCard->setStyleSheet(
                "QFrame { "
                "background: #f9fafb; "
                "border: 1px solid #e5e7eb; "
                "border-radius: 8px; "
                "padding: 0px; "
                "margin: 0px; "
                "} "
                "QFrame:hover { "
                "border-color: #d1d5db; "
                "background: #f3f4f6; "
                "}"
            );
            
            QHBoxLayout *cardLayout = new QHBoxLayout(resultCard);
            cardLayout->setContentsMargins(16, 12, 16, 12);
            cardLayout->setSpacing(12);
            
            // Ícone de status
            QLabel *statusIcon = new QLabel();
            statusIcon->setStyleSheet("font-size: 24px; background: transparent; border: none;");
            statusIcon->setFixedSize(32, 32);
            statusIcon->setAlignment(Qt::AlignCenter);
            
            QString statusText;
            QString statusColor;
            
            switch (result.status) {
                case CheckStatus::Vulnerable:
                    statusIcon->setText("⚠️");
                    statusText = "Vulnerável";
                    statusColor = "#dc2626";
                    break;
                case CheckStatus::Safe:
                    statusIcon->setText("✅");
                    statusText = "Seguro";
                    statusColor = "#059669";
                    break;
                case CheckStatus::Fixed:
                    statusIcon->setText("🔧");
                    statusText = "Corrigido";
                    statusColor = "#2563eb";
                    break;
                case CheckStatus::Skipped:
                    statusIcon->setText("⏭️");
                    statusText = "Ignorado";
                    statusColor = "#d97706";
                    break;
                default:
                    statusIcon->setText("❓");
                    statusText = "Desconhecido";
                    statusColor = "#6b7280";
                    break;
            }
            
            // Nome da vulnerabilidade
            QLabel *nameLabel = new QLabel(vuln->name);
            nameLabel->setStyleSheet(
                "font-weight: 600; "
                "font-size: 15px; "
                "color: #1f2937; "
                "background: transparent; "
                "border: none;"
            );
            
            // Badge de severidade
            QString severityText = vuln->severity == Severity::Alta ? "Alta" :
                                  vuln->severity == Severity::Media ? "Média" : "Baixa";
            
            QString severityBg, severityColor;
            switch (vuln->severity) {
                case Severity::Alta:
                    severityBg = "#fef2f2";
                    severityColor = "#dc2626";
                    break;
                case Severity::Media:
                    severityBg = "#fffbeb";
                    severityColor = "#d97706";
                    break;
                case Severity::Baixa:
                    severityBg = "#fefce8";
                    severityColor = "#ca8a04";
                    break;
            }
            
            QLabel *severityLabel = new QLabel(severityText);
            severityLabel->setStyleSheet(QString(
                "background: %1; "
                "color: %2; "
                "padding: 4px 8px; "
                "border-radius: 12px; "
                "font-weight: 600; "
                "font-size: 11px; "
                "border: none;"
            ).arg(severityBg, severityColor));
            severityLabel->setAlignment(Qt::AlignCenter);
            severityLabel->setFixedHeight(24);
            
            // Status label
            QLabel *statusLabel = new QLabel(statusText);
            statusLabel->setStyleSheet(QString(
                "color: %1; "
                "font-weight: 600; "
                "font-size: 13px; "
                "background: transparent; "
                "border: none;"
            ).arg(statusColor));
            statusLabel->setAlignment(Qt::AlignCenter);
            
            cardLayout->addWidget(statusIcon);
            cardLayout->addWidget(nameLabel, 1);
            cardLayout->addWidget(severityLabel);
            cardLayout->addWidget(statusLabel);
            
            scrollLayout->addWidget(resultCard);
        }
        
        scrollArea->setWidget(scrollWidget);
        m_resultsLayout->addWidget(scrollArea);
        m_resultsLayout->addSpacing(30);
    }
    
    // === BOTÕES DE AÇÃO ===
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setAlignment(Qt::AlignCenter);
    actionLayout->setSpacing(16);
    
    QPushButton *saveReportButton = new QPushButton("💾 Salvar Relatório");
    saveReportButton->setStyleSheet(
        "QPushButton { "
        "background: #059669; "
        "color: white; "
        "border: none; "
        "border-radius: 8px; "
        "padding: 14px 28px; "
        "font-size: 15px; "
        "font-weight: 600; "
        "font-family: 'Segoe UI', sans-serif; "
        "} "
        "QPushButton:hover { "
        "background: #047857; "
        "} "
        "QPushButton:pressed { "
        "background: #065f46; "
        "}"
    );
    saveReportButton->setCursor(Qt::PointingHandCursor);
    connect(saveReportButton, &QPushButton::clicked, this, &SecurityChecker::onSaveReportClicked);
    
    QPushButton *homeButton = new QPushButton("🏠 Voltar ao Início");
    homeButton->setStyleSheet(
        "QPushButton { "
        "background: #2563eb; "
        "color: white; "
        "border: none; "
        "border-radius: 8px; "
        "padding: 14px 28px; "
        "font-size: 15px; "
        "font-weight: 600; "
        "font-family: 'Segoe UI', sans-serif; "
        "} "
        "QPushButton:hover { "
        "background: #1d4ed8; "
        "} "
        "QPushButton:pressed { "
        "background: #1e40af; "
        "}"
    );
    homeButton->setCursor(Qt::PointingHandCursor);
    connect(homeButton, &QPushButton::clicked, this, &SecurityChecker::onBackClicked);
    
    QPushButton *closeButton = new QPushButton("✅ Concluir");
    closeButton->setStyleSheet(
        "QPushButton { "
        "background: #dc2626; "
        "color: white; "
        "border: none; "
        "border-radius: 8px; "
        "padding: 14px 28px; "
        "font-size: 15px; "
        "font-weight: 600; "
        "font-family: 'Segoe UI', sans-serif; "
        "} "
        "QPushButton:hover { "
        "background: #b91c1c; "
        "} "
        "QPushButton:pressed { "
        "background: #991b1b; "
        "}"
    );
    closeButton->setCursor(Qt::PointingHandCursor);
    connect(closeButton, &QPushButton::clicked, []() {
        QApplication::quit();
    });
    
    actionLayout->addWidget(saveReportButton);
    actionLayout->addWidget(homeButton);
    actionLayout->addWidget(closeButton);
    m_resultsLayout->addLayout(actionLayout);
    
    // Forçar atualização do layout
    m_resultsFrame->updateGeometry();
    update();
}

void SecurityChecker::onBackClicked()
{
    emit backRequested();
}

QString SecurityChecker::getSeverityColor(Severity severity) const
{
    switch (severity) {
        case Severity::Alta: return "severityHigh";
        case Severity::Media: return "severityMedium";
        case Severity::Baixa: return "severityLow";
        default: return "severityMedium";
    }
}

QString SecurityChecker::getStatusText(CheckStatus status) const
{
    switch (status) {
        case CheckStatus::Pending: return "Pendente";
        case CheckStatus::Checking: return "Verificando";
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
        case CheckStatus::Safe: return "#059669";
        case CheckStatus::Fixed: return "#2563eb";
        case CheckStatus::Skipped: return "#d97706";
        default: return "#6b7280";
    }
}

void SecurityChecker::updateOSDisplay()
{
    if (!m_vulnerabilityManager) {
        qDebug() << "VulnerabilityManager não inicializado";
        return;
    }
    
    QString currentOS = m_vulnerabilityManager->getCurrentOS();
    QString prettyName = QSysInfo::prettyProductName();
    
    qDebug() << "=== ATUALIZANDO DISPLAY DO OS ===";
    qDebug() << "OS detectado:" << currentOS;
    qDebug() << "Pretty name:" << prettyName;
    
    QString displayText;
    QString emoji;
    
    if (currentOS == "linux") {
        emoji = "🐧";
        displayText = QString("Linux\n%1").arg(prettyName);
    } else if (currentOS == "windows") {
        emoji = "🪟";
        displayText = QString("Windows\n%1").arg(prettyName);
    } else if (currentOS == "macos") {
        emoji = "🍎";
        displayText = QString("macOS\n%1").arg(prettyName);
    } else {
        emoji = "❓";
        displayText = QString("Sistema Desconhecido\n%1").arg(prettyName);
    }
    
    QString finalText = QString("%1 %2").arg(emoji, displayText);
    
    qDebug() << "Texto final para display:" << finalText;
    
    m_osDisplay->setText(finalText);
    
    qDebug() << "Display atualizado com sucesso!";
}

void SecurityChecker::onSaveReportClicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Salvar Relatório de Segurança",
        QString("relatorio_seguranca_%1.txt").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss")),
        "Arquivos de Texto (*.txt);;Todos os Arquivos (*)"
    );
    
    if (fileName.isEmpty()) {
        return;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erro", "Não foi possível salvar o arquivo.");
        return;
    }
    
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    
    // Cabeçalho do relatório
    out << "========================================\n";
    out << "    RELATÓRIO DE SEGURANÇA - SECURECHECK\n";
    out << "========================================\n\n";
    
    out << "Data/Hora: " << QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm:ss") << "\n";
    out << "Sistema: " << QSysInfo::prettyProductName() << "\n";
    out << "Kernel: " << QSysInfo::kernelType() << " " << QSysInfo::kernelVersion() << "\n";
    out << "Arquitetura: " << QSysInfo::currentCpuArchitecture() << "\n\n";
    
    // Estatísticas
    int total = m_currentVulnerabilities.size();
    int vulnerable = 0;
    int fixed = 0;
    int skipped = 0;
    int safe = 0;
    
    for (const auto &result : m_checkResults) {
        switch (result.status) {
            case CheckStatus::Vulnerable: vulnerable++; break;
            case CheckStatus::Fixed: fixed++; break;
            case CheckStatus::Skipped: skipped++; break;
            case CheckStatus::Safe: safe++; break;
            default: break;
        }
    }
    
    out << "RESUMO EXECUTIVO:\n";
    out << "================\n";
    out << "Total de verificações: " << total << "\n";
    out << "Vulnerabilidades encontradas: " << (vulnerable + skipped) << "\n";
    out << "Vulnerabilidades corrigidas: " << fixed << "\n";
    out << "Vulnerabilidades ignoradas: " << skipped << "\n";
    out << "Verificações seguras: " << safe << "\n\n";
    
    // Detalhes das verificações
    out << "DETALHES DAS VERIFICAÇÕES:\n";
    out << "==========================\n\n";
    
    for (const auto &result : m_checkResults) {
        const VulnerabilityDefinition *vuln = nullptr;
        for (const auto &v : m_currentVulnerabilities) {
            if (v.id == result.id) {
                vuln = &v;
                break;
            }
        }
        
        if (!vuln) continue;
        
        QString statusText;
        switch (result.status) {
            case CheckStatus::Vulnerable: statusText = "VULNERÁVEL"; break;
            case CheckStatus::Safe: statusText = "SEGURO"; break;
            case CheckStatus::Fixed: statusText = "CORRIGIDO"; break;
            case CheckStatus::Skipped: statusText = "IGNORADO"; break;
            default: statusText = "DESCONHECIDO"; break;
        }
        
        QString severityText = vuln->severity == Severity::Alta ? "ALTA" :
                              vuln->severity == Severity::Media ? "MÉDIA" : "BAIXA";
        
        out << "----------------------------------------\n";
        out << "ID: " << vuln->id << "\n";
        out << "Nome: " << vuln->name << "\n";
        out << "Status: " << statusText << "\n";
        out << "Severidade: " << severityText << "\n";
        out << "Descrição: " << vuln->description << "\n";
        out << "Impacto: " << vuln->impact << "\n";
        
        if (result.status == CheckStatus::Vulnerable || result.status == CheckStatus::Skipped) {
            out << "Comando de correção: " << vuln->fix << "\n";
        }
        
        out << "\n";
    }
    
    out << "========================================\n";
    out << "Relatório gerado pelo SecureCheck v1.0.0\n";
    out << "https://github.com/jeanccoelho/secure-check\n";
    out << "========================================\n";
    
    file.close();
    
    QMessageBox::information(this, "Sucesso", 
        QString("Relatório salvo com sucesso em:\n%1").arg(fileName));
}