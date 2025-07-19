#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_stackedWidget(nullptr)
    , m_landingPage(nullptr)
    , m_securityChecker(nullptr)
{
    setupUI();
    createMenuBar();
    createStatusBar();
    
    // Conectar sinais
    connect(m_landingPage, &LandingPage::startScanRequested, 
            this, &MainWindow::showSecurityChecker);
    connect(m_securityChecker, &SecurityChecker::backRequested, 
            this, &MainWindow::showLandingPage);
    
    // Mostrar página inicial
    showLandingPage();
    
    // Verificar privilégios administrativos
    checkAdminPrivileges();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("Security Checker - Verificador de Vulnerabilidades");
    setMinimumSize(1000, 700);
    resize(1200, 800);
    
    // Widget central com stack
    m_stackedWidget = new QStackedWidget(this);
    setCentralWidget(m_stackedWidget);
    
    // Criar páginas
    m_landingPage = new LandingPage(this);
    m_securityChecker = new SecurityChecker(this);
    
    // Adicionar ao stack
    m_stackedWidget->addWidget(m_landingPage);
    m_stackedWidget->addWidget(m_securityChecker);
}

void MainWindow::createMenuBar()
{
    // Menu Arquivo
    QMenu *fileMenu = menuBar()->addMenu("&Arquivo");
    
    m_exitAction = new QAction("&Sair", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(m_exitAction);
    
    // Menu Ajuda
    QMenu *helpMenu = menuBar()->addMenu("&Ajuda");
    
    m_aboutAction = new QAction("&Sobre", this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    helpMenu->addAction(m_aboutAction);
    
    QAction *githubAction = new QAction("&GitHub", this);
    connect(githubAction, &QAction::triggered, []() {
        QDesktopServices::openUrl(QUrl("https://github.com/seu-usuario/securitychecker"));
    });
    helpMenu->addAction(githubAction);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage("Pronto - Security Checker v1.0.0");
}

void MainWindow::showLandingPage()
{
    m_stackedWidget->setCurrentWidget(m_landingPage);
    statusBar()->showMessage("Página Inicial");
}

void MainWindow::showSecurityChecker()
{
    m_stackedWidget->setCurrentWidget(m_securityChecker);
    statusBar()->showMessage("Verificação de Segurança");
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "Sobre Security Checker",
        "<h2>Security Checker v1.0.0</h2>"
        "<p>Ferramenta open-source para verificação e correção de vulnerabilidades de segurança.</p>"
        "<p><b>Características:</b></p>"
        "<ul>"
        "<li>Verificação automática de vulnerabilidades</li>"
        "<li>Correção guiada passo a passo</li>"
        "<li>Suporte para Windows, Linux e macOS</li>"
        "<li>Interface intuitiva para usuários domésticos</li>"
        "</ul>"
        "<p><b>Desenvolvido com:</b> Qt6, C++17</p>"
        "<p><b>Licença:</b> MIT License</p>"
        "<p><a href='https://github.com/seu-usuario/securitychecker'>GitHub Repository</a></p>"
    );
}

void MainWindow::checkAdminPrivileges()
{
    // Esta verificação já é feita no main.cpp
    // Aqui podemos adicionar indicadores visuais se necessário
}