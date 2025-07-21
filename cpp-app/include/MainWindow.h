#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include "LandingPage.h"
#include "SecurityChecker.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showLandingPage();
    void showSecurityChecker();
    void showSecurityChecker(bool aiMode);
    void showAbout();
    void checkAdminPrivileges();

private:
    void setupUI();
    void createMenuBar();
    void createStatusBar();
    
    QStackedWidget *m_stackedWidget;
    LandingPage *m_landingPage;
    SecurityChecker *m_securityChecker;
    
    QAction *m_aboutAction;
    QAction *m_exitAction;
};

#endif // MAINWINDOW_H