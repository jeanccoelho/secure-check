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
#include <QRadioButton>
#include <QComboBox>
#include <QButtonGroup>
#include "OllamaClient.h"

class LandingPage : public QWidget
{
    Q_OBJECT

public:
    explicit LandingPage(QWidget *parent = nullptr);
    
    enum class ScanMode {
        Local,
        Ollama
    };
    
    ScanMode getScanMode() const;
    QString getSelectedModel() const;

signals:
    void startScanRequested();
    void startScanRequested(ScanMode mode, const QString &modelName);

private slots:
    void onStartScanClicked();
    void onGitHubClicked();
    void animateEntrance();
    void onScanModeChanged();
    void onModelsReceived(const QList<OllamaModel> &models);
    void onOllamaError(const QString &error);
    void onConnectionTestResult(bool success, const QString &message);

private:
    void setupUI();
    void createHeader();
    void createHeader(QVBoxLayout *parentLayout);
    void createHeroSection();
    void createHeroSection(QVBoxLayout *parentLayout);
    void createFeaturesSection(QVBoxLayout *parentLayout);
    void createScanModeSelection(QVBoxLayout *parentLayout);
    QFrame* createFeatureCard(const QString &icon, const QString &iconColor, 
                             const QString &title, const QString &description);
    void updateModelsList();
    void setOllamaControlsEnabled(bool enabled);
    
    QVBoxLayout *m_mainLayout;
    QScrollArea *m_scrollArea;
    QWidget *m_contentWidget;
    
    QFrame *m_heroFrame;
    QPushButton *m_startScanButton;
    QPushButton *m_githubButton;
    
    // Controles de modo de verificação
    QFrame *m_scanModeFrame;
    QRadioButton *m_localScanRadio;
    QRadioButton *m_ollamaScanRadio;
    QButtonGroup *m_scanModeGroup;
    QComboBox *m_modelComboBox;
    QLabel *m_modelStatusLabel;
    
    // Cliente Ollama
    OllamaClient *m_ollamaClient;
    QList<OllamaModel> m_availableModels;
};

#endif // LANDINGPAGE_H