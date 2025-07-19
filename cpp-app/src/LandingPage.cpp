#include "LandingPage.h"
#include <QDesktopServices>
#include <QUrl>
#include <QPixmap>
#include <QIcon>

LandingPage::LandingPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
{
    setupUI();
}

void LandingPage::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // Criar seções
    createHeader();
    createHeroSection();
    
    // Aplicar estilos
    setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                stop:0 #f0f4f8, stop:1 #e2e8f0);
        }
        
        QFrame#headerFrame {
            background-color: white;
            border: none;
            border-bottom: 2px solid #e2e8f0;
        }
        
        QFrame#heroFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                stop:0 #667eea, stop:1 #764ba2);
            color: white;
            border: none;
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
            border: none;
        }
        
        QPushButton#secondaryButton:hover {
            background-color: #f7fafc;
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
    
    m_mainLayout->addWidget(headerFrame);
}

void LandingPage::createHeroSection()
{
    QFrame *heroFrame = new QFrame();
    heroFrame->setObjectName("heroFrame");
    heroFrame->setMinimumHeight(400);
    
    QVBoxLayout *heroLayout = new QVBoxLayout(heroFrame);
    heroLayout->setContentsMargins(40, 60, 40, 60);
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
    
    // Botão de ação
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignCenter);
    
    m_startScanButton = new QPushButton("🛡️ Iniciar Verificação");
    m_startScanButton->setObjectName("primaryButton");
    m_startScanButton->setStyleSheet(
        "QPushButton { background-color: white; color: #4299e1; font-size: 16px; padding: 16px 32px; }"
        "QPushButton:hover { background-color: #f7fafc; }"
    );
    connect(m_startScanButton, &QPushButton::clicked, this, &LandingPage::onStartScanClicked);
    
    buttonLayout->addWidget(m_startScanButton);
    
    heroLayout->addWidget(titleLabel);
    heroLayout->addSpacing(20);
    heroLayout->addWidget(subtitleLabel, 0, Qt::AlignCenter);
    heroLayout->addSpacing(40);
    heroLayout->addLayout(buttonLayout);
    
    m_mainLayout->addWidget(heroFrame);
    m_mainLayout->addStretch(); // Adiciona espaço flexível no final
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