#ifndef LANDINGPAGE_H
#define LANDINGPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>

class LandingPage : public QWidget
{
    Q_OBJECT

public:
    explicit LandingPage(QWidget *parent = nullptr);

signals:
    void startScanRequested();

private slots:
    void onStartScanClicked();
    void onGitHubClicked();
    void onDownloadClicked();

private:
    void setupUI();
    void createHeader();
    void createHeroSection();
    
    QVBoxLayout *m_mainLayout;
    
    QPushButton *m_startScanButton;
    QPushButton *m_githubButton;
};

#endif // LANDINGPAGE_H