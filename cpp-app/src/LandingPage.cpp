#include "LandingPage.h"
#include <QDesktopServices>
#include <QUrl>
#include <QPixmap>
#include <QIcon>

LandingPage::LandingPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_scrollArea(nullptr)
    , m_contentWidget(nullptr)
{
    setupUI();
}

void LandingPage::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Criar área de scroll
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    // Widget de conteúdo
    m_contentWidget = new QWidget();
    m_contentWidget->setObjectName("contentWidget");
    
    // Layout principal do conteúdo
    QVBoxLayout *contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setSpacing(0);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    
    // Criar seções
    createHeader();
    createHeroSection();
    createFeaturesSection();
    createHowItWorksSection();
    createSupportedSystemsSection();
    createDownloadSection();
    createFooter();
    
    // Configurar scroll area
    m_scrollArea->setWidget(m_contentWidget);
    m_mainLayout->addWidget(m_scrollArea);
    
    // Aplicar estilos
    setStyleSheet(R"(
        QWidget#contentWidget {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                stop:0 #f0f4f8, stop:1 #e2e8f0);
        }
        
        QFrame#headerFrame {
            background-color: white;
            border-bottom: 1px solid #e2e8f0;
        }
        
        QFrame#heroFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                stop:0 #667eea, stop:1 #764ba2);
            color: white;
        }
        
        QFrame#featureFrame, QFrame#systemFrame {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #e2e8f0;
        }
        
        QFrame#downloadFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, 
                stop:0 #4299e1, stop:1 #667eea);
            color: white;
            border-radius: 12px;
        }
        
        QFrame#footerFrame {
            background-color: #2d3748;
            color: white;
        }
        
        QPushButton {
            border: none;
            border-radius: 8px;
            padding: 12px 24px;
            font-weight: bold;
            font-size: 14px;
        }
        
        QPushButton#primaryButton {
            background-color: #4299e1;
            color: white;
        }
        
        QPushButton#primaryButton:hover {
            background-color: #3182ce;
        }
        
        QPushButton#secondaryButton {
            background-color: white;
            color: #2d3748;
            border: 2px solid #e2e8f0;
        }
        
        QPushButton#secondaryButton:hover {
            border-color: #cbd5e0;
        }
        
        QLabel#titleLabel {
            font-size: 48px;
            font-weight: bold;
            color: white;
        }
        
        QLabel#subtitleLabel {
            font-size: 20px;
            color: rgba(255, 255, 255, 0.9);
        }
        
        QLabel#sectionTitle {
            font-size: 32px;
            font-weight: bold;
            color: #2d3748;
        }
        
        QLabel#featureTitle {
            font-size: 20px;
            font-weight: bold;
            color: #2d3748;
        }
        
        QLabel#featureText {
            font-size: 14px;
            color: #4a5568;
        }
    )");
}

void LandingPage::createHeader()
{
    QFrame *headerFrame = new QFrame();
    headerFrame->setObjectName("headerFrame");
    headerFrame->setFixedHeight(80);
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(40, 0, 40, 0);
    
    // Logo e título
    QHBoxLayout *logoLayout = new QHBoxLayout();
    QLabel *logoLabel = new QLabel("🛡️");
    logoLabel->setStyleSheet("font-size: 32px;");
    QLabel *titleLabel = new QLabel("SecurityChecker");
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2d3748;");
    
    logoLayout->addWidget(logoLabel);
    logoLayout->addWidget(titleLabel);
    logoLayout->addStretch();
    
    // Botão GitHub
    m_githubButton = new QPushButton("GitHub");
    m_githubButton->setObjectName("secondaryButton");
    connect(m_githubButton, &QPushButton::clicked, this, &LandingPage::onGitHubClicked);
    
    headerLayout->addLayout(logoLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(m_githubButton);
    
    m_contentWidget->layout()->addWidget(headerFrame);
}

void LandingPage::createHeroSection()
{
    QFrame *heroFrame = new QFrame();
    heroFrame->setObjectName("heroFrame");
    heroFrame->setMinimumHeight(500);
    
    QVBoxLayout *heroLayout = new QVBoxLayout(heroFrame);
    heroLayout->setContentsMargins(40, 80, 40, 80);
    heroLayout->setAlignment(Qt::AlignCenter);
    
    // Título principal
    QLabel *titleLabel = new QLabel("Proteja seu sistema com um clique");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    
    // Subtítulo
    QLabel *subtitleLabel = new QLabel(
        "SecurityChecker é uma ferramenta open-source que verifica e corrige automaticamente "
        "vulnerabilidades de segurança em sistemas Windows, Linux e macOS."
    );
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);
    subtitleLabel->setMaximumWidth(800);
    
    // Botões de ação
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(20);
    
    m_startScanButton = new QPushButton("🛡️ Iniciar Verificação");
    m_startScanButton->setObjectName("primaryButton");
    m_startScanButton->setStyleSheet(
        "QPushButton { background-color: white; color: #4299e1; font-size: 16px; padding: 16px 32px; }"
        "QPushButton:hover { background-color: #f7fafc; }"
    );
    connect(m_startScanButton, &QPushButton::clicked, this, &LandingPage::onStartScanClicked);
    
    QPushButton *downloadButton = new QPushButton("⬇️ Download");
    downloadButton->setObjectName("secondaryButton");
    downloadButton->setStyleSheet(
        "QPushButton { background-color: transparent; color: white; border: 2px solid white; font-size: 16px; padding: 16px 32px; }"
        "QPushButton:hover { background-color: rgba(255, 255, 255, 0.1); }"
    );
    connect(downloadButton, &QPushButton::clicked, this, &LandingPage::onDownloadClicked);
    
    buttonLayout->addWidget(m_startScanButton);
    buttonLayout->addWidget(downloadButton);
    
    heroLayout->addWidget(titleLabel);
    heroLayout->addSpacing(20);
    heroLayout->addWidget(subtitleLabel, 0, Qt::AlignCenter);
    heroLayout->addSpacing(40);
    heroLayout->addLayout(buttonLayout);
    
    m_contentWidget->layout()->addWidget(heroFrame);
}

void LandingPage::createFeaturesSection()
{
    QWidget *sectionWidget = new QWidget();
    QVBoxLayout *sectionLayout = new QVBoxLayout(sectionWidget);
    sectionLayout->setContentsMargins(40, 80, 40, 80);
    
    // Título da seção
    QLabel *sectionTitle = new QLabel("Características Principais");
    sectionTitle->setObjectName("sectionTitle");
    sectionTitle->setAlignment(Qt::AlignCenter);
    
    // Grid de características
    QHBoxLayout *featuresLayout = new QHBoxLayout();
    featuresLayout->setSpacing(30);
    
    // Feature 1
    QFrame *feature1 = new QFrame();
    feature1->setObjectName("featureFrame");
    QVBoxLayout *f1Layout = new QVBoxLayout(feature1);
    f1Layout->setContentsMargins(30, 30, 30, 30);
    f1Layout->setAlignment(Qt::AlignCenter);
    
    QLabel *f1Icon = new QLabel("✅");
    f1Icon->setStyleSheet("font-size: 48px;");
    f1Icon->setAlignment(Qt::AlignCenter);
    
    QLabel *f1Title = new QLabel("Verificação Automática");
    f1Title->setObjectName("featureTitle");
    f1Title->setAlignment(Qt::AlignCenter);
    
    QLabel *f1Text = new QLabel("Detecta automaticamente vulnerabilidades conhecidas em seu sistema operacional.");
    f1Text->setObjectName("featureText");
    f1Text->setAlignment(Qt::AlignCenter);
    f1Text->setWordWrap(true);
    
    f1Layout->addWidget(f1Icon);
    f1Layout->addWidget(f1Title);
    f1Layout->addWidget(f1Text);
    
    // Feature 2
    QFrame *feature2 = new QFrame();
    feature2->setObjectName("featureFrame");
    QVBoxLayout *f2Layout = new QVBoxLayout(feature2);
    f2Layout->setContentsMargins(30, 30, 30, 30);
    f2Layout->setAlignment(Qt::AlignCenter);
    
    QLabel *f2Icon = new QLabel("⚠️");
    f2Icon->setStyleSheet("font-size: 48px;");
    f2Icon->setAlignment(Qt::AlignCenter);
    
    QLabel *f2Title = new QLabel("Correção Guiada");
    f2Title->setObjectName("featureTitle");
    f2Title->setAlignment(Qt::AlignCenter);
    
    QLabel *f2Text = new QLabel("Interface intuitiva que guia você através do processo de correção passo a passo.");
    f2Text->setObjectName("featureText");
    f2Text->setAlignment(Qt::AlignCenter);
    f2Text->setWordWrap(true);
    
    f2Layout->addWidget(f2Icon);
    f2Layout->addWidget(f2Title);
    f2Layout->addWidget(f2Text);
    
    // Feature 3
    QFrame *feature3 = new QFrame();
    feature3->setObjectName("featureFrame");
    QVBoxLayout *f3Layout = new QVBoxLayout(feature3);
    f3Layout->setContentsMargins(30, 30, 30, 30);
    f3Layout->setAlignment(Qt::AlignCenter);
    
    QLabel *f3Icon = new QLabel("🔓");
    f3Icon->setStyleSheet("font-size: 48px;");
    f3Icon->setAlignment(Qt::AlignCenter);
    
    QLabel *f3Title = new QLabel("Open Source");
    f3Title->setObjectName("featureTitle");
    f3Title->setAlignment(Qt::AlignCenter);
    
    QLabel *f3Text = new QLabel("Código aberto e transparente, auditado pela comunidade de segurança.");
    f3Text->setObjectName("featureText");
    f3Text->setAlignment(Qt::AlignCenter);
    f3Text->setWordWrap(true);
    
    f3Layout->addWidget(f3Icon);
    f3Layout->addWidget(f3Title);
    f3Layout->addWidget(f3Text);
    
    featuresLayout->addWidget(feature1);
    featuresLayout->addWidget(feature2);
    featuresLayout->addWidget(feature3);
    
    sectionLayout->addWidget(sectionTitle);
    sectionLayout->addSpacing(40);
    sectionLayout->addLayout(featuresLayout);
    
    m_contentWidget->layout()->addWidget(sectionWidget);
}

void LandingPage::createHowItWorksSection()
{
    QWidget *sectionWidget = new QWidget();
    QVBoxLayout *sectionLayout = new QVBoxLayout(sectionWidget);
    sectionLayout->setContentsMargins(40, 80, 40, 80);
    
    // Título da seção
    QLabel *sectionTitle = new QLabel("Como Funciona");
    sectionTitle->setObjectName("sectionTitle");
    sectionTitle->setAlignment(Qt::AlignCenter);
    
    // Frame com fundo
    QFrame *howItWorksFrame = new QFrame();
    howItWorksFrame->setObjectName("featureFrame");
    QVBoxLayout *frameLayout = new QVBoxLayout(howItWorksFrame);
    frameLayout->setContentsMargins(40, 40, 40, 40);
    
    // Grid de passos
    QHBoxLayout *stepsLayout = new QHBoxLayout();
    stepsLayout->setSpacing(40);
    
    QStringList stepTitles = {
        "Execute como Admin",
        "Verificação Step-by-Step", 
        "Escolha a Ação",
        "Sistema Protegido"
    };
    
    QStringList stepTexts = {
        "O programa precisa de privilégios administrativos para verificar e corrigir vulnerabilidades.",
        "Cada vulnerabilidade é verificada individualmente e o resultado é exibido na tela.",
        "Para cada vulnerabilidade encontrada, você pode escolher corrigir ou pular.",
        "Ao final, seu sistema estará protegido contra as principais ameaças conhecidas."
    };
    
    for (int i = 0; i < 4; ++i) {
        QVBoxLayout *stepLayout = new QVBoxLayout();
        stepLayout->setAlignment(Qt::AlignCenter);
        
        // Número do passo
        QLabel *stepNumber = new QLabel(QString::number(i + 1));
        stepNumber->setStyleSheet(
            "background-color: #4299e1; color: white; border-radius: 32px; "
            "font-size: 24px; font-weight: bold; min-width: 64px; min-height: 64px;"
        );
        stepNumber->setAlignment(Qt::AlignCenter);
        
        // Título do passo
        QLabel *stepTitle = new QLabel(stepTitles[i]);
        stepTitle->setStyleSheet("font-size: 16px; font-weight: bold; color: #2d3748;");
        stepTitle->setAlignment(Qt::AlignCenter);
        stepTitle->setWordWrap(true);
        
        // Texto do passo
        QLabel *stepText = new QLabel(stepTexts[i]);
        stepText->setStyleSheet("font-size: 12px; color: #4a5568;");
        stepText->setAlignment(Qt::AlignCenter);
        stepText->setWordWrap(true);
        stepText->setMaximumWidth(200);
        
        stepLayout->addWidget(stepNumber);
        stepLayout->addSpacing(10);
        stepLayout->addWidget(stepTitle);
        stepLayout->addSpacing(5);
        stepLayout->addWidget(stepText);
        
        stepsLayout->addLayout(stepLayout);
    }
    
    frameLayout->addLayout(stepsLayout);
    
    sectionLayout->addWidget(sectionTitle);
    sectionLayout->addSpacing(40);
    sectionLayout->addWidget(howItWorksFrame);
    
    m_contentWidget->layout()->addWidget(sectionWidget);
}

void LandingPage::createSupportedSystemsSection()
{
    QWidget *sectionWidget = new QWidget();
    QVBoxLayout *sectionLayout = new QVBoxLayout(sectionWidget);
    sectionLayout->setContentsMargins(40, 80, 40, 80);
    
    // Título da seção
    QLabel *sectionTitle = new QLabel("Sistemas Suportados");
    sectionTitle->setObjectName("sectionTitle");
    sectionTitle->setAlignment(Qt::AlignCenter);
    
    // Grid de sistemas
    QHBoxLayout *systemsLayout = new QHBoxLayout();
    systemsLayout->setSpacing(30);
    
    // Windows
    QFrame *windowsFrame = new QFrame();
    windowsFrame->setObjectName("systemFrame");
    QVBoxLayout *winLayout = new QVBoxLayout(windowsFrame);
    winLayout->setContentsMargins(30, 30, 30, 30);
    winLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *winTitle = new QLabel("Windows");
    winTitle->setObjectName("featureTitle");
    winTitle->setAlignment(Qt::AlignCenter);
    
    QLabel *winText = new QLabel("10 verificações de segurança incluindo UAC, Firewall, SMBv1 e mais.");
    winText->setObjectName("featureText");
    winText->setAlignment(Qt::AlignCenter);
    winText->setWordWrap(true);
    
    QLabel *winBadge = new QLabel("10 verificações");
    winBadge->setStyleSheet(
        "background-color: #bee3f8; color: #2b6cb0; padding: 6px 12px; "
        "border-radius: 12px; font-size: 12px; font-weight: bold;"
    );
    winBadge->setAlignment(Qt::AlignCenter);
    
    winLayout->addWidget(winTitle);
    winLayout->addWidget(winText);
    winLayout->addSpacing(10);
    winLayout->addWidget(winBadge);
    
    // Linux
    QFrame *linuxFrame = new QFrame();
    linuxFrame->setObjectName("systemFrame");
    QVBoxLayout *linuxLayout = new QVBoxLayout(linuxFrame);
    linuxLayout->setContentsMargins(30, 30, 30, 30);
    linuxLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *linuxTitle = new QLabel("Linux");
    linuxTitle->setObjectName("featureTitle");
    linuxTitle->setAlignment(Qt::AlignCenter);
    
    QLabel *linuxText = new QLabel("10 verificações essenciais incluindo SSH, Firewall, Sudo e kernel.");
    linuxText->setObjectName("featureText");
    linuxText->setAlignment(Qt::AlignCenter);
    linuxText->setWordWrap(true);
    
    QLabel *linuxBadge = new QLabel("10 verificações");
    linuxBadge->setStyleSheet(
        "background-color: #c6f6d5; color: #276749; padding: 6px 12px; "
        "border-radius: 12px; font-size: 12px; font-weight: bold;"
    );
    linuxBadge->setAlignment(Qt::AlignCenter);
    
    linuxLayout->addWidget(linuxTitle);
    linuxLayout->addWidget(linuxText);
    linuxLayout->addSpacing(10);
    linuxLayout->addWidget(linuxBadge);
    
    // macOS
    QFrame *macosFrame = new QFrame();
    macosFrame->setObjectName("systemFrame");
    QVBoxLayout *macosLayout = new QVBoxLayout(macosFrame);
    macosLayout->setContentsMargins(30, 30, 30, 30);
    macosLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *macosTitle = new QLabel("macOS");
    macosTitle->setObjectName("featureTitle");
    macosTitle->setAlignment(Qt::AlignCenter);
    
    QLabel *macosText = new QLabel("10 verificações principais incluindo Gatekeeper, Firewall e FileVault.");
    macosText->setObjectName("featureText");
    macosText->setAlignment(Qt::AlignCenter);
    macosText->setWordWrap(true);
    
    QLabel *macosBadge = new QLabel("10 verificações");
    macosBadge->setStyleSheet(
        "background-color: #e9d8fd; color: #553c9a; padding: 6px 12px; "
        "border-radius: 12px; font-size: 12px; font-weight: bold;"
    );
    macosBadge->setAlignment(Qt::AlignCenter);
    
    macosLayout->addWidget(macosTitle);
    macosLayout->addWidget(macosText);
    macosLayout->addSpacing(10);
    macosLayout->addWidget(macosBadge);
    
    systemsLayout->addWidget(windowsFrame);
    systemsLayout->addWidget(linuxFrame);
    systemsLayout->addWidget(macosFrame);
    
    sectionLayout->addWidget(sectionTitle);
    sectionLayout->addSpacing(40);
    sectionLayout->addLayout(systemsLayout);
    
    m_contentWidget->layout()->addWidget(sectionWidget);
}

void LandingPage::createDownloadSection()
{
    QWidget *sectionWidget = new QWidget();
    QVBoxLayout *sectionLayout = new QVBoxLayout(sectionWidget);
    sectionLayout->setContentsMargins(40, 80, 40, 80);
    
    QFrame *downloadFrame = new QFrame();
    downloadFrame->setObjectName("downloadFrame");
    QVBoxLayout *downloadLayout = new QVBoxLayout(downloadFrame);
    downloadLayout->setContentsMargins(60, 60, 60, 60);
    downloadLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *downloadTitle = new QLabel("Pronto para começar?");
    downloadTitle->setStyleSheet("font-size: 32px; font-weight: bold; color: white;");
    downloadTitle->setAlignment(Qt::AlignCenter);
    
    QLabel *downloadText = new QLabel("Baixe o SecurityChecker e proteja seu sistema em minutos.");
    downloadText->setStyleSheet("font-size: 20px; color: rgba(255, 255, 255, 0.9);");
    downloadText->setAlignment(Qt::AlignCenter);
    
    QHBoxLayout *downloadButtonsLayout = new QHBoxLayout();
    downloadButtonsLayout->setAlignment(Qt::AlignCenter);
    downloadButtonsLayout->setSpacing(20);
    
    QPushButton *winDownload = new QPushButton("Download para Windows");
    QPushButton *linuxDownload = new QPushButton("Download para Linux");
    QPushButton *macosDownload = new QPushButton("Download para macOS");
    
    QString downloadButtonStyle = 
        "QPushButton { background-color: white; color: #4299e1; padding: 12px 24px; "
        "border-radius: 8px; font-weight: bold; }"
        "QPushButton:hover { background-color: #f7fafc; }";
    
    winDownload->setStyleSheet(downloadButtonStyle);
    linuxDownload->setStyleSheet(downloadButtonStyle);
    macosDownload->setStyleSheet(downloadButtonStyle);
    
    connect(winDownload, &QPushButton::clicked, this, &LandingPage::onDownloadClicked);
    connect(linuxDownload, &QPushButton::clicked, this, &LandingPage::onDownloadClicked);
    connect(macosDownload, &QPushButton::clicked, this, &LandingPage::onDownloadClicked);
    
    downloadButtonsLayout->addWidget(winDownload);
    downloadButtonsLayout->addWidget(linuxDownload);
    downloadButtonsLayout->addWidget(macosDownload);
    
    QLabel *githubLink = new QLabel("Código fonte disponível no <a href='#' style='color: white; text-decoration: underline;'>GitHub</a>");
    githubLink->setStyleSheet("font-size: 14px; color: rgba(255, 255, 255, 0.75);");
    githubLink->setAlignment(Qt::AlignCenter);
    githubLink->setOpenExternalLinks(false);
    connect(githubLink, &QLabel::linkActivated, this, &LandingPage::onGitHubClicked);
    
    downloadLayout->addWidget(downloadTitle);
    downloadLayout->addSpacing(10);
    downloadLayout->addWidget(downloadText);
    downloadLayout->addSpacing(30);
    downloadLayout->addLayout(downloadButtonsLayout);
    downloadLayout->addSpacing(20);
    downloadLayout->addWidget(githubLink);
    
    sectionLayout->addWidget(downloadFrame);
    m_contentWidget->layout()->addWidget(sectionWidget);
}

void LandingPage::createFooter()
{
    QFrame *footerFrame = new QFrame();
    footerFrame->setObjectName("footerFrame");
    footerFrame->setFixedHeight(120);
    
    QVBoxLayout *footerLayout = new QVBoxLayout(footerFrame);
    footerLayout->setContentsMargins(40, 20, 40, 20);
    footerLayout->setAlignment(Qt::AlignCenter);
    
    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *footerLogo = new QLabel("🛡️");
    footerLogo->setStyleSheet("font-size: 24px;");
    
    QLabel *footerTitle = new QLabel("SecurityChecker");
    footerTitle->setStyleSheet("font-size: 18px; font-weight: bold; color: white;");
    
    logoLayout->addWidget(footerLogo);
    logoLayout->addWidget(footerTitle);
    
    QLabel *footerText = new QLabel("Ferramenta open-source para verificação e correção de vulnerabilidades de segurança.");
    footerText->setStyleSheet("font-size: 14px; color: #a0aec0;");
    footerText->setAlignment(Qt::AlignCenter);
    
    QHBoxLayout *linksLayout = new QHBoxLayout();
    linksLayout->setAlignment(Qt::AlignCenter);
    linksLayout->setSpacing(30);
    
    QLabel *githubLink = new QLabel("<a href='#' style='color: #a0aec0;'>GitHub</a>");
    QLabel *docsLink = new QLabel("<a href='#' style='color: #a0aec0;'>Documentação</a>");
    QLabel *supportLink = new QLabel("<a href='#' style='color: #a0aec0;'>Suporte</a>");
    
    githubLink->setOpenExternalLinks(false);
    docsLink->setOpenExternalLinks(false);
    supportLink->setOpenExternalLinks(false);
    
    connect(githubLink, &QLabel::linkActivated, this, &LandingPage::onGitHubClicked);
    
    linksLayout->addWidget(githubLink);
    linksLayout->addWidget(docsLink);
    linksLayout->addWidget(supportLink);
    
    footerLayout->addLayout(logoLayout);
    footerLayout->addWidget(footerText);
    footerLayout->addSpacing(10);
    footerLayout->addLayout(linksLayout);
    
    m_contentWidget->layout()->addWidget(footerFrame);
}

void LandingPage::onStartScanClicked()
{
    emit startScanRequested();
}

void LandingPage::onGitHubClicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/seu-usuario/securitychecker"));
}

void LandingPage::onDownloadClicked()
{
    // Implementar lógica de download ou redirecionar para página de releases
    QDesktopServices::openUrl(QUrl("https://github.com/seu-usuario/securitychecker/releases"));
}