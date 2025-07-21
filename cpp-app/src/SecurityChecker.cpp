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