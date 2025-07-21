#ifndef LANDINGPAGE_H
#define LANDINGPAGE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QGridLayout>

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
    void animateEntrance();

private:
    void setupUI();
    void createHeader();
    void createHeader(QVBoxLayout *parentLayout);
    void createHeroSection();
    void createHeroSection(QVBoxLayout *parentLayout);
    void createFeaturesSection(QVBoxLayout *parentLayout);
    QFrame* createFeatureCard(const QString &icon, const QString &iconColor, 
                             const QString &title, const QString &description);
    
    QVBoxLayout *m_mainLayout;
    QScrollArea *m_scrollArea;
    QWidget *m_contentWidget;
    
    QFrame *m_heroFrame;
    QPushButton *m_startScanButton;
    QPushButton *m_githubButton;
};

#endif // LANDINGPAGE_H