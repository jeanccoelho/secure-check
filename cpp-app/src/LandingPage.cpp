#include "LandingPage.h"
#include <QDesktopServices>
#include <QUrl>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>

LandingPage::LandingPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
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
    
    // Criar seções
    createHeader();
    createHeroSection();
    
    // Aplicar estilos modernos compatíveis com Qt
    setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                stop:0 #667eea, stop:0.5 #764ba2, stop:1 #f093fb);
            font-family: "Segoe UI", "Roboto", "Inter", sans-serif;
        }
        
        QFrame#headerFrame {
            background: rgba(255, 255, 255, 240);
            border: none;
            border-radius: 0px;
        }
        
        QFrame#heroFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(74, 0, 224, 230), stop:1 rgba(142, 45, 226, 230));
            border-radius: 24px;
            color: white;
            margin: 20px;
        }
        
        QPushButton {
            border: none;
            border-radius: 12px;
            padding: 14px 28px;
            font-weight: 600;
            font-size: 15px;
            font-family: "Segoe UI", sans-serif;
        }
        
        QPushButton#primaryButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #4299e1, stop:1 #3182ce);
            color: white;
            font-size: 18px;
            padding: 18px 36px;
            border-radius: 16px;
            font-weight: 700;
        }
        
        QPushButton#primaryButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #3182ce, stop:1 #2c5aa0);
        }
        
        QPushButton#primaryButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                stop:0 #2c5aa0, stop:1 #2a4d8a);
        }
        
        QPushButton#githubButton {
            background: rgba(255, 255, 255, 25);
            color: #2d3748;
            border: 2px solid rgba(45, 55, 72, 25);
            font-weight: 600;
        }
        
        QPushButton#githubButton:hover {
            background: rgba(255, 255, 255, 50);
            border: 2px solid rgba(45, 55, 72, 50);
        }
        
        QLabel#logoLabel {
            font-size: 36px;
            margin-right: 8px;
        }
        
        QLabel#appNameLabel {
            font-size: 28px;
            font-weight: 700;
            color: #2d3748;
            letter-spacing: -0.5px;
        }
        
        QLabel#titleLabel {
            font-size: 48px;
            font-weight: 700;
            color: white;
            letter-spacing: -1px;
            font-family: "Segoe UI", sans-serif;
        }
        
        QLabel#subtitleLabel {
            font-size: 20px;
            color: rgba(255, 255, 255, 240);
            font-weight: 400;
            font-family: "Segoe UI", sans-serif;
        }
        
        QLabel#versionLabel {
            font-size: 12px;
            color: #718096;
            font-weight: 500;
        }
    )");
}

void LandingPage::createHeader()
{
    QFrame *headerFrame = new QFrame();
    headerFrame->setObjectName("headerFrame");
    headerFrame->setFixedHeight(90);
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(40, 0, 40, 0);
    
    // Logo e título
    QHBoxLayout *logoLayout = new QHBoxLayout();
    logoLayout->setSpacing(12);
    
    QLabel *logoLabel = new QLabel("🛡️");
    logoLabel->setObjectName("logoLabel");
    
    QLabel *appNameLabel = new QLabel("SecureFix");
    appNameLabel->setObjectName("appNameLabel");
    
    QLabel *versionLabel = new QLabel("v1.0.0");
    versionLabel->setObjectName("versionLabel");
    
    logoLayout->addWidget(logoLabel);
    logoLayout->addWidget(appNameLabel);
    logoLayout->addWidget(versionLabel);
    logoLayout->addStretch();
    
    // Botão GitHub
    m_githubButton = new QPushButton("⭐ GitHub");
    m_githubButton->setObjectName("githubButton");
    m_githubButton->setToolTip("Ver projeto no GitHub - Dê uma estrela! ⭐");
    m_githubButton->setCursor(Qt::PointingHandCursor);
    connect(m_githubButton, &QPushButton::clicked, this, &LandingPage::onGitHubClicked);
    
    headerLayout->addLayout(logoLayout);
    headerLayout->addStretch();
    headerLayout->addWidget(m_githubButton);
    
    m_mainLayout->addWidget(headerFrame);
}

void LandingPage::createHeroSection()
{
    m_heroFrame = new QFrame();
    m_heroFrame->setObjectName("heroFrame");
    m_heroFrame->setMinimumHeight(500);
    
    QVBoxLayout *heroLayout = new QVBoxLayout(m_heroFrame);
    heroLayout->setContentsMargins(60, 80, 60, 80);
    heroLayout->setAlignment(Qt::AlignCenter);
    heroLayout->setSpacing(30);
    
    // Título principal
    QLabel *titleLabel = new QLabel("Proteja seu sistema\ncom um clique");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    
    // Subtítulo
    QLabel *subtitleLabel = new QLabel(
        "SecureFix é uma ferramenta open-source que verifica e corrige automaticamente\n"
        "vulnerabilidades de segurança em sistemas Windows, Linux e macOS."
    );
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);
    subtitleLabel->setMaximumWidth(700);
    
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
    heroLayout->addWidget(titleLabel);
    heroLayout->addWidget(subtitleLabel, 0, Qt::AlignCenter);
    heroLayout->addLayout(buttonLayout);
    
    // Adicionar espaçamento flexível
    m_mainLayout->addWidget(m_heroFrame, 1);
    m_mainLayout->addStretch();
}

void LandingPage::animateEntrance()
{
    // Animação simples de opacidade para o hero frame
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