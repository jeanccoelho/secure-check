#include "SecurityChecker.h"
#include <QMessageBox>
#include <QApplication>
#include <QDir>

SecurityChecker::SecurityChecker(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(nullptr)
    , m_vulnerabilityManager(nullptr)
    , m_systemChecker(nullptr)
    , m_currentCheckIndex(0)
    , m_isCompleted(false)
{
    setupUI();
    
    // Inicializar managers
    m_vulnerabilityManager = new VulnerabilityManager(this);
    m_systemChecker = new SystemChecker(this);
    
    // Conectar sinais
    connect(m_systemChecker, &SystemChecker::checkCompleted,
            this, &SecurityChecker::onCheckCompleted);
    connect(m_systemChecker, &SystemChecker::fixCompleted,
            this, &SecurityChecker::onFixCompleted);
    connect(m_systemChecker, &SystemChecker::errorOccurred,
            this, &SecurityChecker::onErrorOccurred);
    
    // Carregar vulnerabilidades
    loadVulnerabilities();
}

void SecurityChecker::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);
    m_mainLayout->setSpacing(20);
    
    createHeader();
    createProgressSection();
    createCheckSection();
    createActionButtons();
    createResultsSection();
    
    // Aplicar estilos
    setStyleSheet(R"(
        QWidget {
            background-color: #f7fafc;
        }
        
        QFrame {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #e2e8f0;
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
        
        QPushButton#primaryButton:disabled {
            background-color: #a0aec0;
        }
        
        QPushButton#successButton {
            background-color: #48bb78;
            color: white;
        }
        
        QPushButton#successButton:hover {
            background-color: #38a169;
        }
        
        QPushButton#warningButton {
            background-color: #ed8936;
            color: white;
        }
        
        QPushButton#warningButton:hover {
            background-color: #dd6b20;
        }
        
        QPushButton#secondaryButton {
            background-color: #718096;
            color: white;
        }
        
        QPushButton#secondaryButton:hover {
            background-color: #4a5568;
        }
        
        QLabel#titleLabel {
            font-size: 24px;
            font-weight: bold;
            color: #2d3748;
        }
        
        QLabel#severityHigh {
            background-color: #fed7d7;
            color: #c53030;
            padding: 4px 12px;
            border-radius: 12px;
            font-weight: bold;
        }
        
        QLabel#severityMedium {
            background-color: #feebc8;
            color: #dd6b20;
            padding: 4px 12px;
            border-radius: 12px;
            font-weight: bold;
        }
        
        QLabel#severityLow {
            background-color: #fefcbf;
            color: #d69e2e;
            padding: 4px 12px;
            border-radius: 12px;
            font-weight: bold;
        }
        
        QTextEdit {
            background-color: #2d3748;
            color: #e2e8f0;
            border: 1px solid #4a5568;
            border-radius: 8px;
            font-family: 'Courier New', monospace;
            font-size: 12px;
        }
        
        QProgressBar {
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            text-align: center;
            background-color: #edf2f7;
        }
        
        QProgressBar::chunk {
            background-color: #4299e1;
            border-radius: 6px;
        }
        
        QComboBox {
            border: 2px solid #e2e8f0;
            border-radius: 8px;
            padding: 8px 12px;
            background-color: white;
        }
        
        QComboBox:focus {
            border-color: #4299e1;
        }
    )");
}

void SecurityChecker::createHeader()
{
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    // Botão voltar
    m_backButton = new QPushButton("← Voltar");
    m_backButton->setObjectName("secondaryButton");
    connect(m_backButton, &QPushButton::clicked, this, &SecurityChecker::onBackClicked);
    
    // Spacer
    headerLayout->addWidget(m_backButton);
    headerLayout->addStretch();
    
    // Seletor de OS
    QLabel *osLabel = new QLabel("Sistema Operacional:");
    osLabel->setStyleSheet("font-weight: bold; color: #4a5568;");
    
    m_osComboBox = new QComboBox();
    m_osComboBox->addItem("Windows", "windows");
    m_osComboBox->addItem("Linux", "linux");
    m_osComboBox->addItem("macOS", "macos");
    connect(m_osComboBox, &QComboBox::currentTextChanged, this, &SecurityChecker::onOSChanged);
    
    headerLayout->addWidget(osLabel);
    headerLayout->addWidget(m_osComboBox);
    
    m_mainLayout->addLayout(headerLayout);
}

void SecurityChecker::createProgressSection()
{
    QFrame *progressFrame = new QFrame();
    QVBoxLayout *progressLayout = new QVBoxLayout(progressFrame);
    progressLayout->setContentsMargins(20, 20, 20, 20);
    
    // Label de progresso
    m_progressLabel = new QLabel("Progresso: 0 de 0");
    m_progressLabel->setStyleSheet("font-weight: bold; color: #4a5568;");
    
    // Barra de progresso
    m_progressBar = new QProgressBar();
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    m_progressBar->setFixedHeight(20);
    
    progressLayout->addWidget(m_progressLabel);
    progressLayout->addWidget(m_progressBar);
    
    m_mainLayout->addWidget(progressFrame);
}

void SecurityChecker::createCheckSection()
{
    m_checkFrame = new QFrame();
    QVBoxLayout *checkLayout = new QVBoxLayout(m_checkFrame);
    checkLayout->setContentsMargins(30, 30, 30, 30);
    checkLayout->setAlignment(Qt::AlignCenter);
    
    // Ícone e título
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->setAlignment(Qt::AlignCenter);
    
    QLabel *iconLabel = new QLabel("🛡️");
    iconLabel->setStyleSheet("font-size: 48px;");
    
    m_checkTitle = new QLabel("Carregando...");
    m_checkTitle->setObjectName("titleLabel");
    m_checkTitle->setAlignment(Qt::AlignCenter);
    
    titleLayout->addWidget(iconLabel);
    titleLayout->addSpacing(20);
    titleLayout->addWidget(m_checkTitle);
    
    // Severidade
    m_severityLabel = new QLabel();
    m_severityLabel->setAlignment(Qt::AlignCenter);
    
    // Descrição
    QLabel *descLabel = new QLabel("Descrição:");
    descLabel->setStyleSheet("font-weight: bold; color: #2d3748; font-size: 16px;");
    
    m_descriptionLabel = new QLabel();
    m_descriptionLabel->setWordWrap(true);
    m_descriptionLabel->setStyleSheet("color: #4a5568; font-size: 14px; line-height: 1.5;");
    
    // Impacto
    QLabel *impactLabel = new QLabel("Impacto:");
    impactLabel->setStyleSheet("font-weight: bold; color: #2d3748; font-size: 16px;");
    
    m_impactLabel = new QLabel();
    m_impactLabel->setWordWrap(true);
    m_impactLabel->setStyleSheet("color: #4a5568; font-size: 14px; line-height: 1.5;");
    
    checkLayout->addLayout(titleLayout);
    checkLayout->addSpacing(10);
    checkLayout->addWidget(m_severityLabel);
    checkLayout->addSpacing(20);
    checkLayout->addWidget(descLabel);
    checkLayout->addWidget(m_descriptionLabel);
    checkLayout->addSpacing(15);
    checkLayout->addWidget(impactLabel);
    checkLayout->addWidget(m_impactLabel);
    
    m_mainLayout->addWidget(m_checkFrame);
    
    // Frame de resultado (inicialmente oculto)
    m_resultFrame = new QFrame();
    m_resultFrame->setVisible(false);
    QHBoxLayout *resultLayout = new QHBoxLayout(m_resultFrame);
    resultLayout->setContentsMargins(30, 20, 30, 20);
    resultLayout->setAlignment(Qt::AlignCenter);
    
    m_resultIcon = new QLabel();
    m_resultIcon->setStyleSheet("font-size: 32px;");
    
    m_resultText = new QLabel();
    m_resultText->setStyleSheet("font-size: 18px; font-weight: bold;");
    
    resultLayout->addWidget(m_resultIcon);
    resultLayout->addSpacing(15);
    resultLayout->addWidget(m_resultText);
    
    m_mainLayout->addWidget(m_resultFrame);
    
    // Campo de comando de correção (inicialmente oculto)
    m_fixCommandEdit = new QTextEdit();
    m_fixCommandEdit->setVisible(false);
    m_fixCommandEdit->setMaximumHeight(100);
    m_fixCommandEdit->setReadOnly(true);
    
    m_mainLayout->addWidget(m_fixCommandEdit);
}

void SecurityChecker::createActionButtons()
{
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignCenter);
    buttonLayout->setSpacing(20);
    
    m_startCheckButton = new QPushButton("🛡️ Iniciar Verificação");
    m_startCheckButton->setObjectName("primaryButton");
    m_startCheckButton->setStyleSheet("font-size: 16px; padding: 16px 32px;");
    connect(m_startCheckButton, &QPushButton::clicked, this, &SecurityChecker::onStartCheckClicked);
    
    m_fixButton = new QPushButton("🔧 Corrigir e Continuar");
    m_fixButton->setObjectName("successButton");
    m_fixButton->setVisible(false);
    connect(m_fixButton, &QPushButton::clicked, this, &SecurityChecker::onFixClicked);
    
    m_skipButton = new QPushButton("⏭️ Pular sem Corrigir");
    m_skipButton->setObjectName("warningButton");
    m_skipButton->setVisible(false);
    connect(m_skipButton, &QPushButton::clicked, this, &SecurityChecker::onSkipClicked);
    
    m_nextButton = new QPushButton("Continuar");
    m_nextButton->setObjectName("primaryButton");
    m_nextButton->setVisible(false);
    connect(m_nextButton, &QPushButton::clicked, this, &SecurityChecker::onNextClicked);
    
    buttonLayout->addWidget(m_startCheckButton);
    buttonLayout->addWidget(m_fixButton);
    buttonLayout->addWidget(m_skipButton);
    buttonLayout->addWidget(m_nextButton);
    
    m_mainLayout->addLayout(buttonLayout);
}

void SecurityChecker::createResultsSection()
{
    m_resultsFrame = new QFrame();
    m_resultsFrame->setVisible(false);
    
    QVBoxLayout *resultsMainLayout = new QVBoxLayout(m_resultsFrame);
    resultsMainLayout->setContentsMargins(30, 30, 30, 30);
    
    QLabel *resultsTitle = new QLabel("Resumo da Verificação");
    resultsTitle->setObjectName("titleLabel");
    resultsTitle->setAlignment(Qt::AlignCenter);
    
    m_resultsLayout = new QVBoxLayout();
    
    resultsMainLayout->addWidget(resultsTitle);
    resultsMainLayout->addSpacing(20);
    resultsMainLayout->addLayout(m_resultsLayout);
    
    m_mainLayout->addWidget(m_resultsFrame);
}

void SecurityChecker::loadVulnerabilities()
{
    QString filePath = QApplication::applicationDirPath() + "/vulnerabilities.json";
    
    if (!m_vulnerabilityManager->loadDefinitions(filePath)) {
        QMessageBox::critical(this, "Erro", 
            "Não foi possível carregar as definições de vulnerabilidades.\n"
            "Verifique se o arquivo 'vulnerabilities.json' está presente.");
        return;
    }
    
    // Detectar OS atual e configurar
    m_currentOS = m_vulnerabilityManager->getCurrentOS();
    
    // Configurar combo box
    for (int i = 0; i < m_osComboBox->count(); ++i) {
        if (m_osComboBox->itemData(i).toString() == m_currentOS) {
            m_osComboBox->setCurrentIndex(i);
            break;
        }
    }
    
    onOSChanged(m_currentOS);
}

void SecurityChecker::onOSChanged(const QString &os)
{
    m_currentOS = m_osComboBox->currentData().toString();
    m_currentVulnerabilities = m_vulnerabilityManager->getDefinitionsForOS(m_currentOS);
    
    resetChecker();
    updateProgress();
    updateCurrentCheck();
}

void SecurityChecker::resetChecker()
{
    m_currentCheckIndex = 0;
    m_checkResults.clear();
    m_isCompleted = false;
    
    // Resetar UI
    m_resultFrame->setVisible(false);
    m_fixCommandEdit->setVisible(false);
    m_resultsFrame->setVisible(false);
    m_checkFrame->setVisible(true);
    
    updateActionButtons();
}

void SecurityChecker::updateProgress()
{
    int total = m_currentVulnerabilities.size();
    int current = m_currentCheckIndex + 1;
    
    if (total > 0) {
        m_progressLabel->setText(QString("Progresso: %1 de %2").arg(current).arg(total));
        m_progressBar->setValue((current * 100) / total);
    } else {
        m_progressLabel->setText("Nenhuma verificação disponível");
        m_progressBar->setValue(0);
    }
}

void SecurityChecker::updateCurrentCheck()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        showResults();
        return;
    }
    
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    
    m_checkTitle->setText(vuln.name);
    m_descriptionLabel->setText(vuln.description);
    m_impactLabel->setText(vuln.impact);
    
    // Configurar label de severidade
    QString severityText = QString("Severidade: %1").arg(
        vuln.severity == Severity::Alta ? "Alta" :
        vuln.severity == Severity::Media ? "Média" : "Baixa"
    );
    
    QString severityStyle = getSeverityColor(vuln.severity);
    m_severityLabel->setText(severityText);
    m_severityLabel->setObjectName(severityStyle);
    m_severityLabel->style()->unpolish(m_severityLabel);
    m_severityLabel->style()->polish(m_severityLabel);
    
    updateActionButtons();
}

void SecurityChecker::updateActionButtons()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    // Verificar se já temos resultado para esta verificação
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    CheckResult *result = nullptr;
    
    for (auto &r : m_checkResults) {
        if (r.id == vuln.id) {
            result = &r;
            break;
        }
    }
    
    // Mostrar botões apropriados
    m_startCheckButton->setVisible(!result);
    m_fixButton->setVisible(result && result->status == CheckStatus::Vulnerable);
    m_skipButton->setVisible(result && result->status == CheckStatus::Vulnerable);
    m_nextButton->setVisible(result && result->status == CheckStatus::Safe);
    
    // Mostrar resultado se disponível
    if (result) {
        m_resultFrame->setVisible(true);
        
        if (result->status == CheckStatus::Vulnerable) {
            m_resultIcon->setText("⚠️");
            m_resultText->setText("Vulnerabilidade Encontrada!");
            m_resultText->setStyleSheet("color: #e53e3e;");
            
            // Mostrar comando de correção
            m_fixCommandEdit->setPlainText(vuln.fix);
            m_fixCommandEdit->setVisible(true);
        } else if (result->status == CheckStatus::Safe) {
            m_resultIcon->setText("✅");
            m_resultText->setText("Sistema Seguro");
            m_resultText->setStyleSheet("color: #38a169;");
            m_fixCommandEdit->setVisible(false);
        }
    } else {
        m_resultFrame->setVisible(false);
        m_fixCommandEdit->setVisible(false);
    }
}

void SecurityChecker::onStartCheckClicked()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    
    // Desabilitar botão durante verificação
    m_startCheckButton->setEnabled(false);
    m_startCheckButton->setText("Verificando...");
    
    // Iniciar verificação
    m_systemChecker->checkVulnerability(vuln);
}

void SecurityChecker::onFixClicked()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    
    // Desabilitar botões durante correção
    m_fixButton->setEnabled(false);
    m_skipButton->setEnabled(false);
    m_fixButton->setText("Corrigindo...");
    
    // Iniciar correção
    m_systemChecker->fixVulnerability(vuln);
}

void SecurityChecker::onSkipClicked()
{
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        return;
    }
    
    const VulnerabilityDefinition &vuln = m_currentVulnerabilities[m_currentCheckIndex];
    
    // Marcar como pulado
    CheckResult result;
    result.id = vuln.id;
    result.status = CheckStatus::Skipped;
    result.isVulnerable = true; // Era vulnerável mas foi pulado
    
    // Atualizar ou adicionar resultado
    bool found = false;
    for (auto &r : m_checkResults) {
        if (r.id == vuln.id) {
            r = result;
            found = true;
            break;
        }
    }
    if (!found) {
        m_checkResults.append(result);
    }
    
    onNextClicked();
}

void SecurityChecker::onNextClicked()
{
    m_currentCheckIndex++;
    updateProgress();
    
    if (m_currentCheckIndex >= m_currentVulnerabilities.size()) {
        showResults();
    } else {
        updateCurrentCheck();
    }
}

void SecurityChecker::onCheckCompleted(const QString &id, bool isVulnerable)
{
    // Reabilitar botão
    m_startCheckButton->setEnabled(true);
    m_startCheckButton->setText("🛡️ Iniciar Verificação");
    
    // Salvar resultado
    CheckResult result;
    result.id = id;
    result.status = isVulnerable ? CheckStatus::Vulnerable : CheckStatus::Safe;
    result.isVulnerable = isVulnerable;
    
    // Atualizar ou adicionar resultado
    bool found = false;
    for (auto &r : m_checkResults) {
        if (r.id == id) {
            r = result;
            found = true;
            break;
        }
    }
    if (!found) {
        m_checkResults.append(result);
    }
    
    updateActionButtons();
}

void SecurityChecker::onFixCompleted(const QString &id, bool success)
{
    // Reabilitar botões
    m_fixButton->setEnabled(true);
    m_skipButton->setEnabled(true);
    m_fixButton->setText("🔧 Corrigir e Continuar");
    
    if (success) {
        // Marcar como corrigido
        for (auto &r : m_checkResults) {
            if (r.id == id) {
                r.status = CheckStatus::Fixed;
                break;
            }
        }
        
        // Avançar automaticamente
        onNextClicked();
    } else {
        QMessageBox::warning(this, "Erro na Correção", 
            "Não foi possível corrigir a vulnerabilidade automaticamente. "
            "Você pode tentar executar o comando manualmente ou pular esta verificação.");
    }
}

void SecurityChecker::onErrorOccurred(const QString &error)
{
    QMessageBox::critical(this, "Erro", error);
    
    // Reabilitar botões
    m_startCheckButton->setEnabled(true);
    m_startCheckButton->setText("🛡️ Iniciar Verificação");
    m_fixButton->setEnabled(true);
    m_skipButton->setEnabled(true);
    m_fixButton->setText("🔧 Corrigir e Continuar");
}

void SecurityChecker::showResults()
{
    m_isCompleted = true;
    
    // Ocultar seção de verificação
    m_checkFrame->setVisible(false);
    m_resultFrame->setVisible(false);
    m_fixCommandEdit->setVisible(false);
    
    // Mostrar seção de resultados
    m_resultsFrame->setVisible(true);
    
    // Limpar layout anterior
    QLayoutItem *item;
    while ((item = m_resultsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    
    // Estatísticas
    int total = m_currentVulnerabilities.size();
    int vulnerable = 0;
    int fixed = 0;
    int skipped = 0;
    int safe = 0;
    
    for (const auto &result : m_checkResults) {
        switch (result.status) {
            case CheckStatus::Vulnerable: vulnerable++; break;
            case CheckStatus::Fixed: fixed++; break;
            case CheckStatus::Skipped: skipped++; break;
            case CheckStatus::Safe: safe++; break;
            default: break;
        }
    }
    
    // Grid de estatísticas
    QHBoxLayout *statsLayout = new QHBoxLayout();
    
    auto createStatCard = [](const QString &number, const QString &label, const QString &color) {
        QFrame *card = new QFrame();
        card->setStyleSheet(QString("background-color: %1; border-radius: 8px; padding: 20px;").arg(color));
        
        QVBoxLayout *cardLayout = new QVBoxLayout(card);
        cardLayout->setAlignment(Qt::AlignCenter);
        
        QLabel *numLabel = new QLabel(number);
        numLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
        numLabel->setAlignment(Qt::AlignCenter);
        
        QLabel *textLabel = new QLabel(label);
        textLabel->setStyleSheet("font-size: 12px; color: rgba(255, 255, 255, 0.9);");
        textLabel->setAlignment(Qt::AlignCenter);
        
        cardLayout->addWidget(numLabel);
        cardLayout->addWidget(textLabel);
        
        return card;
    };
    
    statsLayout->addWidget(createStatCard(QString::number(total), "Total", "#4299e1"));
    statsLayout->addWidget(createStatCard(QString::number(vulnerable), "Vulneráveis", "#e53e3e"));
    statsLayout->addWidget(createStatCard(QString::number(fixed), "Corrigidas", "#38a169"));
    statsLayout->addWidget(createStatCard(QString::number(skipped), "Ignoradas", "#ed8936"));
    
    m_resultsLayout->addLayout(statsLayout);
    m_resultsLayout->addSpacing(30);
    
    // Lista detalhada de resultados
    for (const auto &result : m_checkResults) {
        const VulnerabilityDefinition *vuln = nullptr;
        for (const auto &v : m_currentVulnerabilities) {
            if (v.id == result.id) {
                vuln = &v;
                break;
            }
        }
        
        if (!vuln) continue;
        
        QFrame *resultCard = new QFrame();
        resultCard->setStyleSheet("background-color: #f7fafc; border: 1px solid #e2e8f0; border-radius: 8px; padding: 15px;");
        
        QHBoxLayout *cardLayout = new QHBoxLayout(resultCard);
        
        // Ícone de status
        QLabel *statusIcon = new QLabel();
        statusIcon->setStyleSheet("font-size: 24px;");
        
        QString statusText;
        QString statusColor;
        
        switch (result.status) {
            case CheckStatus::Vulnerable:
                statusIcon->setText("⚠️");
                statusText = "Vulnerável";
                statusColor = "#e53e3e";
                break;
            case CheckStatus::Safe:
                statusIcon->setText("✅");
                statusText = "Seguro";
                statusColor = "#38a169";
                break;
            case CheckStatus::Fixed:
                statusIcon->setText("🔧");
                statusText = "Corrigido";
                statusColor = "#4299e1";
                break;
            case CheckStatus::Skipped:
                statusIcon->setText("⏭️");
                statusText = "Ignorado";
                statusColor = "#ed8936";
                break;
            default:
                statusIcon->setText("❓");
                statusText = "Desconhecido";
                statusColor = "#718096";
                break;
        }
        
        // Informações da vulnerabilidade
        QVBoxLayout *infoLayout = new QVBoxLayout();
        
        QLabel *nameLabel = new QLabel(vuln->name);
        nameLabel->setStyleSheet("font-weight: bold; font-size: 16px; color: #2d3748;");
        
        QLabel *descLabel = new QLabel(vuln->description);
        descLabel->setStyleSheet("color: #4a5568; font-size: 14px;");
        descLabel->setWordWrap(true);
        
        infoLayout->addWidget(nameLabel);
        infoLayout->addWidget(descLabel);
        
        // Status e severidade
        QVBoxLayout *statusLayout = new QVBoxLayout();
        statusLayout->setAlignment(Qt::AlignCenter);
        
        QLabel *statusLabel = new QLabel(statusText);
        statusLabel->setStyleSheet(QString("color: %1; font-weight: bold; font-size: 14px;").arg(statusColor));
        statusLabel->setAlignment(Qt::AlignCenter);
        
        QString severityText = vuln->severity == Severity::Alta ? "Alta" :
                              vuln->severity == Severity::Media ? "Média" : "Baixa";
        QLabel *severityLabel = new QLabel(severityText);
        severityLabel->setObjectName(getSeverityColor(vuln->severity));
        severityLabel->setAlignment(Qt::AlignCenter);
        severityLabel->style()->unpolish(severityLabel);
        severityLabel->style()->polish(severityLabel);
        
        statusLayout->addWidget(statusLabel);
        statusLayout->addWidget(severityLabel);
        
        cardLayout->addWidget(statusIcon);
        cardLayout->addLayout(infoLayout, 1);
        cardLayout->addLayout(statusLayout);
        
        m_resultsLayout->addWidget(resultCard);
    }
    
    // Botão para voltar ao início
    m_resultsLayout->addSpacing(30);
    
    QPushButton *homeButton = new QPushButton("🏠 Voltar ao Início");
    homeButton->setObjectName("primaryButton");
    homeButton->setStyleSheet("font-size: 16px; padding: 16px 32px;");
    connect(homeButton, &QPushButton::clicked, this, &SecurityChecker::onBackClicked);
    
    QHBoxLayout *homeLayout = new QHBoxLayout();
    homeLayout->setAlignment(Qt::AlignCenter);
    homeLayout->addWidget(homeButton);
    
    m_resultsLayout->addLayout(homeLayout);
}

void SecurityChecker::onBackClicked()
{
    emit backRequested();
}

QString SecurityChecker::getSeverityColor(Severity severity) const
{
    switch (severity) {
        case Severity::Alta: return "severityHigh";
        case Severity::Media: return "severityMedium";
        case Severity::Baixa: return "severityLow";
        default: return "severityMedium";
    }
}

QString SecurityChecker::getStatusText(CheckStatus status) const
{
    switch (status) {
        case CheckStatus::Pending: return "Pendente";
        case CheckStatus::Checking: return "Verificando";
        case CheckStatus::Vulnerable: return "Vulnerável";
        case CheckStatus::Safe: return "Seguro";
        case CheckStatus::Skipped: return "Ignorado";
        case CheckStatus::Fixed: return "Corrigido";
        default: return "Desconhecido";
    }
}

QString SecurityChecker::getStatusColor(CheckStatus status) const
{
    switch (status) {
        case CheckStatus::Vulnerable: return "#e53e3e";
        case CheckStatus::Safe: return "#38a169";
        case CheckStatus::Fixed: return "#4299e1";
        case CheckStatus::Skipped: return "#ed8936";
        default: return "#718096";
    }
}