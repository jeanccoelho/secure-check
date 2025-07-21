#include "LandingPage.h"
#include <QDesktopServices>
#include <QUrl>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>
#include <QScrollArea>
#include <QGridLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QComboBox>

LandingPage::LandingPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
    , m_scanModeFrame(nullptr)
    , m_localScanRadio(nullptr)
    , m_ollamaScanRadio(nullptr)
    , m_scanModeGroup(nullptr)
    , m_modelComboBox(nullptr)
    , m_modelStatusLabel(nullptr)
    , m_ollamaClient(nullptr)
{
    // Inicializar cliente Ollama
    m_ollamaClient = new OllamaClient(this);
    connect(m_ollamaClient, &OllamaClient::modelsReceived, this, &LandingPage::onModelsReceived);
    connect(m_ollamaClient, &OllamaClient::errorOccurred, this, &LandingPage::onOllamaError);
    connect(m_ollamaClient, &OllamaClient::connectionTestResult, this, &LandingPage::onConnectionTestResult);
    
    setupUI();
    
    // Adicionar animação de entrada após a criação
    QTimer::singleShot(200, this, &LandingPage::animateEntrance);
}

void LandingPage::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Criar scroll area para conteúdo
    m_scrollArea = new QScrollArea();
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    
    m_contentWidget = new QWidget();
    m_scrollArea->setWidget(m_contentWidget);
    
    QVBoxLayout *contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    
    // Criar seções
    createHeader(contentLayout);
    createHeroSection(contentLayout);
    createScanModeSelection(contentLayout);
    createFeaturesSection(contentLayout);
    
    m_mainLayout->addWidget(m_scrollArea);
    
    // Aplicar estilos modernos
    setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                stop:0 #dbeafe, stop:1 #e0e7ff);
            font-family: "Segoe UI", "Roboto", "Inter", sans-serif;
        }
        
        QScrollArea {
            border: none;
            background: transparent;
        }
        
        QScrollBar:vertical {
            background: rgba(0, 0, 0, 0.1);
            width: 8px;
            border-radius: 4px;
        }
        
        QScrollBar::handle:vertical {
            background: rgba(0, 0, 0, 0.3);
            border-radius: 4px;
            min-height: 20px;
        }
        
        QScrollBar::handle:vertical:hover {
            background: rgba(0, 0, 0, 0.5);
        }
        
        QFrame#headerFrame {
            background: white;
            border: none;
            border-radius: 0px;
        }
        
        QFrame#heroFrame {
            background: transparent;
            border: none;
            color: #1f2937;
        }
        
        QFrame#featureCard {
            background: white;
            border: 1px solid #e5e7eb;
            border-radius: 12px;
            padding: 20px;
        }
        
        QFrame#featureCard:hover {
            border: 1px solid #d1d5db;
        }
        
        QPushButton {
            border: none;
            border-radius: 8px;
            padding: 12px 24px;
            font-weight: 600;
            font-size: 14px;
            font-family: "Segoe UI", sans-serif;
        }
        
        QPushButton#primaryButton {
            background: #2563eb;
            color: white;
            font-size: 16px;
            padding: 16px 32px;
            border-radius: 8px;
            font-weight: 600;
        }
        
        QPushButton#primaryButton:hover {
            background: #1d4ed8;
        }
        
        QPushButton#primaryButton:pressed {
            background: #1e40af;
        }
        
        QPushButton#githubButton {
            background: #1f2937;
            color: white;
            font-weight: 600;
            padding: 8px 16px;
        }
        
        QPushButton#githubButton:hover {
            background: #374151;
        }
        
        QLabel#logoIcon {
            font-size: 28px;
            color: #1f2937;
            background-color: white;
        }
        
        QLabel#appNameLabel {
            font-size: 24px;
            font-weight: 700;
            color: #1f2937;
            letter-spacing: -0.5px;
            background-color: white;
        }
        
        QLabel#heroTitle {
            font-size: 48px;
            font-weight: 700;
            color: #1f2937;
            letter-spacing: -1px;
            font-family: "Segoe UI", sans-serif;
        }
        
        QLabel#heroSubtitle {
            font-size: 20px;
            color: #4b5563;
            font-weight: 400;
            font-family: "Segoe UI", sans-serif;
        }
        
        QLabel#featureIcon {
            font-size: 48px;
            margin-bottom: 16px;
            background-color: white;
        }
        
        QLabel#featureTitle {
            font-size: 20px;
            font-weight: 600;
            color: #1f2937;
            margin-bottom: 12px;
            background-color: white;
        }
        
        QLabel#featureDescription {
            font-size: 14px;
            color: #6b7280;
            line-height: 1.5;
            background-color: white;
        }
    )");
}

void LandingPage::createHeader(QVBoxLayout *parentLayout)
{
    QFrame *headerFrame = new QFrame();
    headerFrame->setObjectName("headerFrame");
    headerFrame->setFixedHeight(80);
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(32, 0, 32, 0);
    
    // Logo e título
    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->setSpacing(12);
    
    QLabel *logoIcon = new QLabel("🛡️");
    logoIcon->setObjectName("logoIcon");
    
    QLabel *appNameLabel = new QLabel("SecureCheck");
    appNameLabel->setObjectName("appNameLabel");
    
    logoLayout->addWidget(logoIcon);
    logoLayout->addWidget(appNameLabel);
    logoLayout->addStretch();
    
    // Botão GitHub
    m_githubButton = new QPushButton("⭐ GitHub");
    m_githubButton->setObjectName("githubButton");
    m_githubButton->setToolTip("Ver projeto no GitHub");
    m_githubButton->setCursor(Qt::PointingHandCursor);
    connect(m_githubButton, &QPushButton::clicked, this, &LandingPage::onGitHubClicked);
    
    headerLayout->addLayout(logoLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(m_githubButton);
    
    parentLayout->addWidget(headerFrame);
}

void LandingPage::createHeroSection(QVBoxLayout *parentLayout)
{
    m_heroFrame = new QFrame();
    m_heroFrame->setObjectName("heroFrame");
    
    QVBoxLayout *heroLayout = new QVBoxLayout(m_heroFrame);
    heroLayout->setContentsMargins(32, 64, 32, 64);
    heroLayout->setAlignment(Qt::AlignCenter);
    heroLayout->setSpacing(24);
    
    // Título principal
    QLabel *heroTitle = new QLabel("Proteja seu sistema com um clique");
    heroTitle->setObjectName("heroTitle");
    heroTitle->setAlignment(Qt::AlignCenter);
    heroTitle->setWordWrap(true);
    
    // Subtítulo
    QLabel *heroSubtitle = new QLabel(
        "SecureCheck é uma ferramenta open-source que verifica e corrige automaticamente\n"
        "vulnerabilidades de segurança em sistemas Windows, Linux e macOS."
    );
    heroSubtitle->setObjectName("heroSubtitle");
    heroSubtitle->setAlignment(Qt::AlignCenter);
    heroSubtitle->setWordWrap(true);
    
    // Botão de ação principal
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignCenter);
    
    m_startScanButton = new QPushButton("🛡️ Iniciar Verificação");
    m_startScanButton->setObjectName("primaryButton");
    m_startScanButton->setCursor(Qt::PointingHandCursor);
    m_startScanButton->setToolTip("Começar verificação de segurança do sistema");
    connect(m_startScanButton, &QPushButton::clicked, this, &LandingPage::onStartScanClicked);
    
    buttonLayout->addWidget(m_startScanButton);
    
    // Adicionar elementos ao layout
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addStretch(1);
    titleLayout->addWidget(heroTitle, 6);
    titleLayout->addStretch(1);
    
    QHBoxLayout *subtitleLayout = new QHBoxLayout();
    subtitleLayout->addStretch(1);
    subtitleLayout->addWidget(heroSubtitle, 7);
    subtitleLayout->addStretch(1);
    
    heroLayout->addLayout(titleLayout);
    heroLayout->addLayout(subtitleLayout);
    heroLayout->addSpacing(16);
    
    parentLayout->addWidget(m_heroFrame);
}

void LandingPage::createScanModeSelection(QVBoxLayout *parentLayout)
{
    m_scanModeFrame = new QFrame();
    m_scanModeFrame->setStyleSheet(
        "QFrame { "
        "background: white; "
        "border: 1px solid #e5e7eb; "
        "border-radius: 12px; "
        "margin: 16px 32px; "
        "}"
    );
    
    QVBoxLayout *modeLayout = new QVBoxLayout(m_scanModeFrame);
    modeLayout->setContentsMargins(32, 24, 32, 24);
    modeLayout->setSpacing(20);
    
    // Título da seção
    QLabel *modeTitle = new QLabel("Escolha o Modo de Verificação");
    modeTitle->setStyleSheet(
        "font-size: 24px; "
        "font-weight: 700; "
        "color: #1f2937; "
        "background: transparent; "
        "border: none; "
        "margin-bottom: 8px;"
    );
    modeTitle->setAlignment(Qt::AlignCenter);
    
    QLabel *modeSubtitle = new QLabel("Selecione como deseja verificar as vulnerabilidades do seu sistema");
    modeSubtitle->setStyleSheet(
        "font-size: 16px; "
        "color: #6b7280; "
        "background: transparent; "
        "border: none; "
        "margin-bottom: 16px;"
    );
    modeSubtitle->setAlignment(Qt::AlignCenter);
    
    modeLayout->addWidget(modeTitle);
    modeLayout->addWidget(modeSubtitle);
    
    // Opções de modo
    QHBoxLayout *optionsLayout = new QHBoxLayout();
    optionsLayout->setSpacing(24);
    optionsLayout->setAlignment(Qt::AlignCenter);
    
    // Grupo de botões de rádio
    m_scanModeGroup = new QButtonGroup(this);
    
    // Opção 1: Verificação Local
    QFrame *localFrame = new QFrame();
    localFrame->setStyleSheet(
        "QFrame { "
        "background: #f8fafc; "
        "border: 2px solid #e2e8f0; "
        "border-radius: 12px; "
        "padding: 20px; "
        "} "
        "QFrame:hover { "
        "border-color: #3b82f6; "
        "background: #eff6ff; "
        "}"
    );
    localFrame->setFixedSize(280, 180);
    
    QVBoxLayout *localLayout = new QVBoxLayout(localFrame);
    localLayout->setAlignment(Qt::AlignCenter);
    localLayout->setSpacing(12);
    
    m_localScanRadio = new QRadioButton("Verificação Local");
    m_localScanRadio->setStyleSheet(
        "QRadioButton { "
        "font-size: 18px; "
        "font-weight: 600; "
        "color: #1f2937; "
        "background: transparent; "
        "} "
        "QRadioButton::indicator { "
        "width: 20px; "
        "height: 20px; "
        "}"
    );
    m_localScanRadio->setChecked(true); // Padrão
    
    QLabel *localIcon = new QLabel("📋");
    localIcon->setStyleSheet("font-size: 48px; background: transparent;");
    localIcon->setAlignment(Qt::AlignCenter);
    
    QLabel *localDesc = new QLabel("Usa definições pré-configuradas\nbaseadas em vulnerabilidades conhecidas");
    localDesc->setStyleSheet(
        "font-size: 14px; "
        "color: #6b7280; "
        "background: transparent; "
        "text-align: center;"
    );
    localDesc->setAlignment(Qt::AlignCenter);
    localDesc->setWordWrap(true);
    
    localLayout->addWidget(localIcon);
    localLayout->addWidget(m_localScanRadio);
    localLayout->addWidget(localDesc);
    
    // Opção 2: Verificação com IA
    QFrame *ollamaFrame = new QFrame();
    ollamaFrame->setStyleSheet(
        "QFrame { "
        "background: #f8fafc; "
        "border: 2px solid #e2e8f0; "
        "border-radius: 12px; "
        "padding: 20px; "
        "} "
        "QFrame:hover { "
        "border-color: #10b981; "
        "background: #ecfdf5; "
        "}"
    );
    ollamaFrame->setFixedSize(280, 180);
    
    QVBoxLayout *ollamaLayout = new QVBoxLayout(ollamaFrame);
    ollamaLayout->setAlignment(Qt::AlignCenter);
    ollamaLayout->setSpacing(12);
    
    m_ollamaScanRadio = new QRadioButton("Verificação com IA");
    m_ollamaScanRadio->setStyleSheet(
        "QRadioButton { "
        "font-size: 18px; "
        "font-weight: 600; "
        "color: #1f2937; "
        "background: transparent; "
        "} "
        "QRadioButton::indicator { "
        "width: 20px; "
        "height: 20px; "
        "}"
    );
    
    QLabel *ollamaIcon = new QLabel("🤖");
    ollamaIcon->setStyleSheet("font-size: 48px; background: transparent;");
    ollamaIcon->setAlignment(Qt::AlignCenter);
    
    QLabel *ollamaDesc = new QLabel("Análise dinâmica usando\ninteligência artificial (Ollama)");
    ollamaDesc->setStyleSheet(
        "font-size: 14px; "
        "color: #6b7280; "
        "background: transparent; "
        "text-align: center;"
    );
    ollamaDesc->setAlignment(Qt::AlignCenter);
    ollamaDesc->setWordWrap(true);
    
    ollamaLayout->addWidget(ollamaIcon);
    ollamaLayout->addWidget(m_ollamaScanRadio);
    ollamaLayout->addWidget(ollamaDesc);
    
    // Adicionar ao grupo
    m_scanModeGroup->addButton(m_localScanRadio, 0);
    m_scanModeGroup->addButton(m_ollamaScanRadio, 1);
    
    optionsLayout->addWidget(localFrame);
    optionsLayout->addWidget(ollamaFrame);
    
    modeLayout->addLayout(optionsLayout);
    
    // Seleção de modelo Ollama
    QHBoxLayout *modelLayout = new QHBoxLayout();
    modelLayout->setAlignment(Qt::AlignCenter);
    modelLayout->setSpacing(12);
    
    QLabel *modelLabel = new QLabel("Modelo IA:");
    modelLabel->setStyleSheet(
        "font-size: 14px; "
        "font-weight: 600; "
        "color: #374151; "
        "background: transparent;"
    );
    
    m_modelComboBox = new QComboBox();
    m_modelComboBox->setStyleSheet(
        "QComboBox { "
        "border: 1px solid #d1d5db; "
        "border-radius: 6px; "
        "padding: 8px 12px; "
        "background: white; "
        "color: #1f2937; "
        "font-weight: 500; "
        "min-width: 200px; "
        "} "
        "QComboBox:focus { "
        "border-color: #3b82f6; "
        "} "
        "QComboBox::drop-down { "
        "border: none; "
        "width: 20px; "
        "} "
        "QComboBox::down-arrow { "
        "image: none; "
        "border-left: 5px solid transparent; "
        "border-right: 5px solid transparent; "
        "border-top: 5px solid #6b7280; "
        "margin-right: 5px; "
        "}"
    );
    m_modelComboBox->setEnabled(false);
    
    m_modelStatusLabel = new QLabel("Carregando modelos...");
    m_modelStatusLabel->setStyleSheet(
        "font-size: 12px; "
        "color: #6b7280; "
        "background: transparent;"
    );
    
    modelLayout->addWidget(modelLabel);
    modelLayout->addWidget(m_modelComboBox);
    modelLayout->addWidget(m_modelStatusLabel);
    
    modeLayout->addLayout(modelLayout);
    
    // Botão de iniciar verificação
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignCenter);
    
    m_startScanButton = new QPushButton("🛡️ Iniciar Verificação");
    m_startScanButton->setObjectName("primaryButton");
    m_startScanButton->setCursor(Qt::PointingHandCursor);
    m_startScanButton->setToolTip("Começar verificação de segurança do sistema");
    connect(m_startScanButton, &QPushButton::clicked, this, &LandingPage::onStartScanClicked);
    
    buttonLayout->addWidget(m_startScanButton);
    modeLayout->addLayout(buttonLayout);
    
    parentLayout->addWidget(m_scanModeFrame);
    
    // Conectar sinais
    connect(m_scanModeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &LandingPage::onScanModeChanged);
    
    // Inicializar estado
    onScanModeChanged();
}

void LandingPage::createFeaturesSection(QVBoxLayout *parentLayout)
{
    // Container para features
    QWidget *featuresContainer = new QWidget();
    QVBoxLayout *featuresMainLayout = new QVBoxLayout(featuresContainer);
    featuresMainLayout->setContentsMargins(32, 32, 32, 64);
    featuresMainLayout->setSpacing(32);
    
    // Grid layout para os 3 cards
    QGridLayout *featuresGrid = new QGridLayout();
    featuresGrid->setSpacing(24);
    
    // Feature 1: Verificação Automática
    QFrame *feature1 = createFeatureCard(
        "✅",
        "#10b981",
        "Verificação Automática",
        "Detecta automaticamente vulnerabilidades conhecidas em seu sistema operacional."
    );
    
    // Feature 2: Correção Guiada
    QFrame *feature2 = createFeatureCard(
        "⚠️",
        "#f59e0b",
        "Correção Guiada",
        "Interface intuitiva que guia você através do processo de correção passo a passo."
    );
    
    // Feature 3: Open Source
    QFrame *feature3 = createFeatureCard(
        "🔒",
        "#2563eb",
        "Open Source",
        "Código aberto e transparente, auditado pela comunidade de segurança."
    );
    
    // Adicionar cards ao grid (responsivo)
    featuresGrid->addWidget(feature1, 0, 0);
    featuresGrid->addWidget(feature2, 0, 1);
    featuresGrid->addWidget(feature3, 0, 2);
    
    // Configurar colunas para serem iguais
    featuresGrid->setColumnStretch(0, 1);
    featuresGrid->setColumnStretch(1, 1);
    featuresGrid->setColumnStretch(2, 1);
    
    featuresMainLayout->addLayout(featuresGrid);
    
    parentLayout->addWidget(featuresContainer);
}

QFrame* LandingPage::createFeatureCard(const QString &icon, const QString &iconColor, 
                                      const QString &title, const QString &description)
{
    QFrame *card = new QFrame();
    card->setObjectName("featureCard");
    card->setMinimumHeight(200);
    card->setMaximumWidth(300);
    
    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(24, 32, 24, 32);
    cardLayout->setAlignment(Qt::AlignCenter);
    cardLayout->setSpacing(16);
    
    // Ícone
    QLabel *iconLabel = new QLabel(icon);
    iconLabel->setObjectName("featureIcon");
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet(QString("color: %1;").arg(iconColor));
    
    // Título
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setObjectName("featureTitle");
    titleLabel->setAlignment(Qt::AlignCenter);
    
    // Descrição
    QLabel *descLabel = new QLabel(description);
    descLabel->setObjectName("featureDescription");
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    
    cardLayout->addWidget(iconLabel);
    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(descLabel);
    
    return card;
}

LandingPage::ScanMode LandingPage::getScanMode() const
{
    if (m_ollamaScanRadio && m_ollamaScanRadio->isChecked()) {
        return ScanMode::Ollama;
    }
    return ScanMode::Local;
}

QString LandingPage::getSelectedModel() const
{
    if (m_modelComboBox && m_modelComboBox->isEnabled()) {
        return m_modelComboBox->currentData().toString();
    }
    return QString();
}

void LandingPage::animateEntrance()
{
    // Animação simples de opacidade
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "windowOpacity");
    opacityAnimation->setDuration(600);
    opacityAnimation->setStartValue(0.0);
    opacityAnimation->setEndValue(1.0);
    opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void LandingPage::onStartScanClicked()
{
    // Feedback visual simples
    m_startScanButton->setEnabled(false);
    m_startScanButton->setText("Iniciando...");
    
    // Restaurar após um breve delay
    QTimer::singleShot(300, [this]() {
        m_startScanButton->setEnabled(true);
        m_startScanButton->setText("🛡️ Iniciar Verificação");
        
        ScanMode mode = getScanMode();
        QString modelName = getSelectedModel();
        
        emit startScanRequested(mode, modelName);
    });
}

void LandingPage::onScanModeChanged()
{
    bool isOllamaMode = (m_ollamaScanRadio && m_ollamaScanRadio->isChecked());
    
    setOllamaControlsEnabled(isOllamaMode);
    
    if (isOllamaMode && m_availableModels.isEmpty()) {
        // Buscar modelos disponíveis
        updateModelsList();
    }
}

void LandingPage::onModelsReceived(const QList<OllamaModel> &models)
{
    m_availableModels = models;
    
    m_modelComboBox->clear();
    
    if (models.isEmpty()) {
        m_modelStatusLabel->setText("Nenhum modelo encontrado");
        m_modelStatusLabel->setStyleSheet("color: #dc2626; font-size: 12px; background: transparent;");
        m_modelComboBox->setEnabled(false);
    } else {
        for (const auto &model : models) {
            QString displayName = model.name;
            if (!model.size.isEmpty()) {
                displayName += QString(" (%1)").arg(model.size);
            }
            m_modelComboBox->addItem(displayName, model.name);
        }
        
        m_modelStatusLabel->setText(QString("%1 modelo(s) disponível(is)").arg(models.size()));
        m_modelStatusLabel->setStyleSheet("color: #059669; font-size: 12px; background: transparent;");
        m_modelComboBox->setEnabled(m_ollamaScanRadio->isChecked());
    }
}

void LandingPage::onOllamaError(const QString &error)
{
    m_modelStatusLabel->setText(QString("Erro: %1").arg(error));
    m_modelStatusLabel->setStyleSheet("color: #dc2626; font-size: 12px; background: transparent;");
    m_modelComboBox->setEnabled(false);
    m_modelComboBox->clear();
}

void LandingPage::onConnectionTestResult(bool success, const QString &message)
{
    if (success) {
        m_modelStatusLabel->setText("Conectado ao Ollama");
        m_modelStatusLabel->setStyleSheet("color: #059669; font-size: 12px; background: transparent;");
    } else {
        m_modelStatusLabel->setText(QString("Conexão falhou: %1").arg(message));
        m_modelStatusLabel->setStyleSheet("color: #dc2626; font-size: 12px; background: transparent;");
    }
}

void LandingPage::updateModelsList()
{
    m_modelStatusLabel->setText("Buscando modelos...");
    m_modelStatusLabel->setStyleSheet("color: #6b7280; font-size: 12px; background: transparent;");
    m_modelComboBox->clear();
    m_modelComboBox->setEnabled(false);
    
    // Buscar modelos disponíveis
    m_ollamaClient->getAvailableModels();
}

void LandingPage::setOllamaControlsEnabled(bool enabled)
{
    if (m_modelComboBox) {
        m_modelComboBox->setEnabled(enabled && !m_availableModels.isEmpty());
    }
    
    if (m_modelStatusLabel) {
        m_modelStatusLabel->setVisible(enabled);
    }
}

void LandingPage::onGitHubClicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/jeanccoelho/secure-check"));
}