#include "LandingPage.h"
#include <QDesktopServices>
#include <QUrl>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>
#include <QScrollArea>
#include <QGridLayout>

LandingPage::LandingPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
{
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
    
    // Seleção de modo de verificação
    QFrame *modeFrame = new QFrame();
    modeFrame->setStyleSheet("QFrame { background: rgba(255, 255, 255, 0.9); border-radius: 12px; padding: 20px; }");
    QVBoxLayout *modeLayout = new QVBoxLayout(modeFrame);
    modeLayout->setSpacing(12);
    
    QLabel *modeTitle = new QLabel("Escolha o modo de verificação:");
    modeTitle->setStyleSheet("font-size: 16px; font-weight: 600; color: #1f2937; margin-bottom: 8px;");
    modeTitle->setAlignment(Qt::AlignCenter);
    
    m_modeButtonGroup = new QButtonGroup(this);
    
    m_staticModeRadio = new QRadioButton("🔍 Modo Estático - Verificações pré-definidas (rápido e confiável)");
    m_staticModeRadio->setStyleSheet("font-size: 14px; color: #374151; padding: 8px;");
    m_staticModeRadio->setChecked(true); // Padrão
    
    m_aiModeRadio = new QRadioButton("🤖 Modo Dinâmico com IA - Análise inteligente personalizada");
    m_aiModeRadio->setStyleSheet("font-size: 14px; color: #374151; padding: 8px;");
    
    m_modeButtonGroup->addButton(m_staticModeRadio, 0);
    m_modeButtonGroup->addButton(m_aiModeRadio, 1);
    
    modeLayout->addWidget(modeTitle);
    modeLayout->addWidget(m_staticModeRadio);
    modeLayout->addWidget(m_aiModeRadio);
    
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
    heroLayout->addWidget(modeFrame);
    heroLayout->addSpacing(16);
    heroLayout->addLayout(buttonLayout);
    
    parentLayout->addWidget(m_heroFrame);
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
        emit startScanRequested();
    });
}

void LandingPage::onGitHubClicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/jeanccoelho/secure-check"));
}

bool LandingPage::isAIModeSelected() const
{
    return m_aiModeRadio && m_aiModeRadio->isChecked();
}