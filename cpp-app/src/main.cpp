#include <QApplication>
#include <QMessageBox>
#include <QDir>
#include <QStandardPaths>
#include <QStyleFactory>
#include <QFont>
#include <unistd.h>
#include "MainWindow.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>

bool isRunningAsAdmin() {
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    
    return isAdmin == TRUE;
}

void requestAdminPrivileges() {
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    
    ShellExecuteW(NULL, L"runas", exePath, NULL, NULL, SW_SHOWNORMAL);
}
#else
bool isRunningAsAdmin() {
    return geteuid() == 0;
}

void requestAdminPrivileges() {
    // Para Linux/macOS, mostrar instruções para executar com sudo
}
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Configurar informações da aplicação
    app.setApplicationName("SecurityChecker");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("OpenSource Security Tools");
    app.setApplicationDisplayName("Security Checker");
    
    // Configurar estilo da aplicação
    app.setStyle(QStyleFactory::create("Fusion"));
    
    // Configurar fonte padrão
    QFont font("Segoe UI", 10);
    if (!font.exactMatch()) {
        font = QFont("Roboto", 10);
        if (!font.exactMatch()) {
            font = QFont("Inter", 10);
            if (!font.exactMatch()) {
                font = app.font();
                font.setPointSize(10);
            }
        }
    }
    app.setFont(font);
    
    // Aplicar tema escuro moderno
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    app.setPalette(darkPalette);
    
    // Verificar privilégios administrativos
    if (!isRunningAsAdmin()) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            nullptr,
            "Privilégios Administrativos Necessários",
            "Esta aplicação precisa ser executada como administrador para verificar e corrigir vulnerabilidades do sistema.\n\n"
            "Deseja executar como administrador agora?",
            QMessageBox::Yes | QMessageBox::No
        );
        
        if (reply == QMessageBox::Yes) {
#ifdef Q_OS_WIN
            requestAdminPrivileges();
            return 0;
#else
            QMessageBox::information(
                nullptr,
                "Instruções",
                "Por favor, execute a aplicação com privilégios de administrador:\n\n"
                "Linux/macOS: sudo ./SecurityChecker"
            );
            return 1;
#endif
        } else {
            QMessageBox::warning(
                nullptr,
                "Aviso",
                "A aplicação continuará em modo limitado. Algumas verificações e correções podem não funcionar corretamente."
            );
        }
    }
    
    // Criar e mostrar janela principal
    MainWindow window;
    window.show();
    
    return app.exec();
}