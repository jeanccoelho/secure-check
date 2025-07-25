#ifndef SECURITYCHECKER_H
#define SECURITYCHECKER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QComboBox>
#include <QTextEdit>
#include <QFrame>
#include <QTimer>
#include <QFileDialog>
#include <QDateTime>
#include <QScrollArea>
#include "VulnerabilityDefinition.h"
#include "VulnerabilityManager.h"
#include "SystemChecker.h"
#include "OllamaClient.h"
#include "LandingPage.h"

class SecurityChecker : public QWidget
{
    Q_OBJECT

public:
    explicit SecurityChecker(QWidget *parent = nullptr);
    
    void setScanMode(LandingPage::ScanMode mode, const QString &modelName = QString());

signals:
    void backRequested();
    void errorOccurred(const QString &error);

private slots:
    void onBackClicked();
    void onStartCheckClicked();
    void onFixClicked();
    void onSkipClicked();
    void onNextClicked();
    void onCheckCompleted(const QString &id, bool isVulnerable);
    void onFixCompleted(const QString &id, bool success);
    void onErrorOccurred(const QString &error);
    void onSaveReportClicked();
    void onOllamaVulnerabilitiesReceived(const QVector<VulnerabilityDefinition> &vulnerabilities);
    void onOllamaError(const QString &error);

private:
    void setupUI();
    void createHeader();
    void createProgressSection();
    void createCheckSection();
    void createActionButtons();
    void createResultsSection();
    
    void loadVulnerabilities();
    void updateProgress();
    void updateCurrentCheck();
    void updateActionButtons();
    void updateOSDisplay();
    void showResults();
    void resetChecker();
    
    QString getSeverityColor(Severity severity) const;
    QString getStatusText(CheckStatus status) const;
    QString getStatusColor(CheckStatus status) const;
    void startOllamaAnalysis();
    SystemInfo collectSystemInfo() const;
    
    // UI Components
    QVBoxLayout *m_mainLayout;
    QPushButton *m_backButton;
    QLabel *m_osDisplay;
    QProgressBar *m_progressBar;
    QLabel *m_progressLabel;
    
    QFrame *m_checkFrame;
    QLabel *m_checkTitle;
    QLabel *m_severityLabel;
    QLabel *m_descriptionLabel;
    QLabel *m_impactLabel;
    QTextEdit *m_fixCommandEdit;
    
    QFrame *m_resultFrame;
    QLabel *m_resultIcon;
    QLabel *m_resultText;
    
    QPushButton *m_startCheckButton;
    QPushButton *m_fixButton;
    QPushButton *m_skipButton;
    QPushButton *m_nextButton;
    
    QFrame *m_resultsFrame;
    QVBoxLayout *m_resultsLayout;
    
    // Data
    VulnerabilityManager *m_vulnerabilityManager;
    SystemChecker *m_systemChecker;
    OllamaClient *m_ollamaClient;
    QVector<VulnerabilityDefinition> m_currentVulnerabilities;
    QVector<CheckResult> m_checkResults;
    int m_currentCheckIndex;
    QString m_currentOS;
    bool m_isCompleted;
    
    // Modo de verificação
    LandingPage::ScanMode m_scanMode;
    QString m_selectedModel;
};

#endif // SECURITYCHECKER_H