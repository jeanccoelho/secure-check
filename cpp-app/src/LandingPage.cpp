#include "LandingPage.h"
#include <QDesktopServices>
#include <QUrl>
#include <QPixmap>
#include <QIcon>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsDropShadowEffect>
#include <QTimer>

LandingPage::LandingPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
{
    setupUI();
    
    // Adicionar animação de entrada após a criação
    QTimer::singleShot(100, this, &LandingPage::animateEntrance);
}

void LandingPage::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    
    // Criar seções
    createHeader();
    createHeroSection();
    
    // Aplicar estilos modernos
    setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                stop:0 #667eea, stop:0.5 #764ba2, stop:1 #f093fb);
            font-family: "Segoe UI", "Roboto", "Inter", sans-serif;
        }
        
        QFrame#headerFrame {
            background: rgba(255, 255, 255, 0.95);
            border: none;
            border-radius: 0px;
            backdrop-filter: blur(10px);
        }
        
        QFrame#heroFrame {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 rgba(74, 0, 224, 0.9), stop:1 rgba(142, 45, 226, 0.9));
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
            background: rgba(255, 255, 255, 0.1);
            color: #2d3748;
            border: 2px solid rgba(45, 55, 72, 0.1);
            backdrop-filter: blur(10px);
            font-weight: 600;
        }
        
        QPushButton#githubButton:hover {
            background: rgba(255, 255, 255, 0.2);
            border: 2px solid rgba(45, 55, 72, 0.2);
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
            line-height: 1.2;
            font-family: "Segoe UI", sans-serif;
        }
        
        QLabel#subtitleLabel {
            font-size: 20px;
            color: rgba(255, 255, 255, 0.95);
            line-height: 1.6;
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
    
    // Adicionar sombra ao header
    QGraphicsDropShadowEffect *headerShadow = new QGraphicsDropShadowEffect();
    headerShadow->setBlurRadius(20);
    headerShadow->setColor(QColor(0, 0, 0, 30));
    headerShadow->setOffset(0, 2);
    headerFrame->setGraphicsEffect(headerShadow);
    
    QHBoxLayout *headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(40, 0, 40, 0);
    
    // Logo e título com melhor tipografia
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
    
    // Botão GitHub melhorado
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
    
    // Adicionar sombra dramática ao hero
    QGraphicsDropShadowEffect *heroShadow = new QGraphicsDropShadowEffect();
    heroShadow->setBlurRadius(40);
    heroShadow->setColor(QColor(0, 0, 0, 50));
    heroShadow->setOffset(0, 10);
    m_heroFrame->setGraphicsEffect(heroShadow);
    
    QVBoxLayout *heroLayout = new QVBoxLayout(m_heroFrame);
    heroLayout->setContentsMargins(60, 80, 60, 80);
    heroLayout->setAlignment(Qt::AlignCenter);
    heroLayout->setSpacing(30);
    
    // Título principal mais impactante
    QLabel *titleLabel = new QLabel("Proteja seu sistema\ncom um clique");
    titleLabel->setObjectName("titleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    
    // Subtítulo melhorado
    QLabel *subtitleLabel = new QLabel(
        "SecureFix é uma ferramenta open-source que verifica e corrige automaticamente\n"
        "vulnerabilidades de segurança em sistemas Windows, Linux e macOS."
    );
    subtitleLabel->setObjectName("subtitleLabel");
    subtitleLabel->setAlignment(Qt::AlignCenter);
    subtitleLabel->setWordWrap(true);
    subtitleLabel->setMaximumWidth(700);
    
    // Botão de ação principal melhorado
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignCenter);
    
    m_startScanButton = new QPushButton("🛡️ Iniciar Verificação");
    m_startScanButton->setObjectName("primaryButton");
    m_startScanButton->setCursor(Qt::PointingHandCursor);
    m_startScanButton->setToolTip("Começar verificação de segurança do sistema");
    connect(m_startScanButton, &QPushButton::clicked, this, &LandingPage::onStartScanClicked);
    
    // Adicionar sombra ao botão
    QGraphicsDropShadowEffect *buttonShadow = new QGraphicsDropShadowEffect();
    buttonShadow->setBlurRadius(20);
    buttonShadow->setColor(QColor(0, 0, 0, 40));
    buttonShadow->setOffset(0, 8);
    m_startScanButton->setGraphicsEffect(buttonShadow);
    
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
    // Animação de entrada suave para o hero frame
    QPropertyAnimation *heroAnimation = new QPropertyAnimation(m_heroFrame, "geometry");
    heroAnimation->setDuration(800);
    
    QRect currentGeometry = m_heroFrame->geometry();
    QRect startGeometry = currentGeometry;
    startGeometry.moveTop(startGeometry.top() + 50);
    
    m_heroFrame->setGeometry(startGeometry);
    
    heroAnimation->setStartValue(startGeometry);
    heroAnimation->setEndValue(currentGeometry);
    heroAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Animação de opacidade
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect();
    m_heroFrame->setGraphicsEffect(opacityEffect);
    
    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    opacityAnimation->setDuration(800);
    opacityAnimation->setStartValue(0.0);
    opacityAnimation->setEndValue(1.0);
    opacityAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    heroAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void LandingPage::onStartScanClicked()
{
    // Animação de feedback no botão
    QPropertyAnimation *buttonAnimation = new QPropertyAnimation(m_startScanButton, "geometry");
    buttonAnimation->setDuration(150);
    
    QRect currentGeometry = m_startScanButton->geometry();
    QRect pressedGeometry = currentGeometry;
    pressedGeometry.adjust(2, 2, -2, -2);
    
    buttonAnimation->setStartValue(currentGeometry);
    buttonAnimation->setEndValue(pressedGeometry);
    buttonAnimation->setEasingCurve(QEasingCurve::OutCubic);
    
    // Voltar ao tamanho original
    QPropertyAnimation *buttonAnimationBack = new QPropertyAnimation(m_startScanButton, "geometry");
    buttonAnimationBack->setDuration(150);
    buttonAnimationBack->setStartValue(pressedGeometry);
    buttonAnimationBack->setEndValue(currentGeometry);
    buttonAnimationBack->setEasingCurve(QEasingCurve::OutCubic);
    
    connect(buttonAnimation, &QPropertyAnimation::finished, [=]() {
        buttonAnimationBack->start(QAbstractAnimation::DeleteWhenStopped);
        emit startScanRequested();
    });
    
    buttonAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void LandingPage::onGitHubClicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/jeanccoelho/secure-check"));
}