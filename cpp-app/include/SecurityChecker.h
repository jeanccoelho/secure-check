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
#include <QCheckBox>
#include "VulnerabilityDefinition.h"
#include "VulnerabilityManager.h"
#include "SystemChecker.h"
#include "OllamaClient.h"

class SecurityChecker : public QWidget
{
    Q_OBJECT

public:
    explicit SecurityChecker(QWidget *parent = nullptr);

signals:
    void backRequested();

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
    
    // Novos slots para Ollama
    void onOllamaModeToggled(bool enabled);
    void onOllamaConfigClicked();
    void onOllamaAnalysisClicked();
    void onOllamaVulnerabilitiesFound(const QVector<VulnerabilityDefinition> &vulnerabilities);
    void onOllamaAnalysisCompleted(bool success);
    void onOllamaConnectionTest(bool success, const QString &message);
    void onOllamaError(const QString &error);
    void onOllamaProgress(const QString &status);
    void onSystemInfoCollected(const SystemInfo &info);

private:
    void setupUI();
    void createHeader();
    void createProgressSection();
    void createOllamaSection();
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
    void updateOllamaStatus();
    void showOllamaConfigDialog();
    
    QString getSeverityColor(Severity severity) const;
    QString getStatusText(CheckStatus status) const;
    QString getStatusColor(CheckStatus status) const;
    
    // Novos métodos para Ollama
    void setupOllamaUI();
    void updateOllamaStatus();
    void showOllamaConfigDialog();
    
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
    
    // Novos componentes UI para Ollama
    QFrame *m_ollamaFrame;
    QCheckBox *m_ollamaCheckBox;
    QPushButton *m_ollamaConfigButton;
    QPushButton *m_ollamaAnalysisButton;
    QLabel *m_ollamaStatusLabel;
    QProgressBar *m_ollamaProgressBar;
    
    // Data
    VulnerabilityManager *m_vulnerabilityManager;
    SystemChecker *m_systemChecker;
    QVector<VulnerabilityDefinition> m_currentVulnerabilities;
    QVector<CheckResult> m_checkResults;
    int m_currentCheckIndex;
    QString m_currentOS;
    bool m_isCompleted;
    
    // Novos membros para Ollama
    OllamaClient *m_ollamaClient;
    bool m_isOllamaModeEnabled;
    bool m_isOllamaAnalysisRunning;
};

#endif // SECURITYCHECKER_H