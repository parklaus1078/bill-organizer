#include "card_form_dialog.h"
#include "../models/payment_card_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

CardFormDialog::CardFormDialog(QWidget *parent)
    : QDialog(parent)
    , m_manager(PaymentCardManager::instance())
    , m_isEditMode(false)
{
    setWindowTitle("Add Payment Card");
    setupUI();
    setupConnections();
}

CardFormDialog::CardFormDialog(const PaymentCard &card, QWidget *parent)
    : QDialog(parent)
    , m_card(card)
    , m_manager(PaymentCardManager::instance())
    , m_isEditMode(true)
{
    setWindowTitle("Edit Payment Card");
    setupUI();
    loadCard(card);
    setupConnections();
}

void CardFormDialog::setupUI()
{
    setMinimumWidth(500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Form layout
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(15);

    // Card Nickname
    m_nicknameInput = new QLineEdit(this);
    m_nicknameInput->setPlaceholderText("e.g., My Visa, Work Card");
    m_nicknameInput->setMaxLength(50);
    QLabel *nicknameError = new QLabel(this);
    nicknameError->setStyleSheet("color: #F44336; font-size: 11px;");
    nicknameError->hide();
    m_errorLabels["nickname"] = nicknameError;

    QVBoxLayout *nicknameLayout = new QVBoxLayout();
    nicknameLayout->addWidget(m_nicknameInput);
    nicknameLayout->addWidget(nicknameError);
    formLayout->addRow("Card Nickname *:", nicknameLayout);

    // Issuer/Bank
    m_issuerInput = new QLineEdit(this);
    m_issuerInput->setPlaceholderText("e.g., Chase, Bank of America");
    m_issuerInput->setMaxLength(50);
    QLabel *issuerError = new QLabel(this);
    issuerError->setStyleSheet("color: #F44336; font-size: 11px;");
    issuerError->hide();
    m_errorLabels["issuer"] = issuerError;

    QVBoxLayout *issuerLayout = new QVBoxLayout();
    issuerLayout->addWidget(m_issuerInput);
    issuerLayout->addWidget(issuerError);
    formLayout->addRow("Issuer/Bank *:", issuerLayout);

    // Last 4 Digits
    m_last4Input = new QLineEdit(this);
    m_last4Input->setPlaceholderText("1234");
    m_last4Input->setMaxLength(4);
    QRegularExpressionValidator *digitValidator =
        new QRegularExpressionValidator(QRegularExpression("[0-9]{0,4}"), this);
    m_last4Input->setValidator(digitValidator);
    QLabel *last4Error = new QLabel(this);
    last4Error->setStyleSheet("color: #F44336; font-size: 11px;");
    last4Error->hide();
    m_errorLabels["last4"] = last4Error;

    QVBoxLayout *last4Layout = new QVBoxLayout();
    last4Layout->addWidget(m_last4Input);
    last4Layout->addWidget(last4Error);
    formLayout->addRow("Last 4 Digits *:", last4Layout);

    // Expiry Date (Month/Year only for credit cards)
    m_expiryInput = new QDateEdit(this);
    m_expiryInput->setDisplayFormat("MM/yy");
    m_expiryInput->setCalendarPopup(false); // No calendar popup - month/year only
    // Set minimum to first day of current month
    QDate today = QDate::currentDate();
    m_expiryInput->setMinimumDate(QDate(today.year(), today.month(), 1));
    QLabel *expiryError = new QLabel(this);
    expiryError->setStyleSheet("color: #F44336; font-size: 11px;");
    expiryError->hide();
    m_errorLabels["expiry"] = expiryError;

    QVBoxLayout *expiryLayout = new QVBoxLayout();
    expiryLayout->addWidget(m_expiryInput);
    expiryLayout->addWidget(expiryError);
    formLayout->addRow("Expiry Date *:", expiryLayout);

    // CVC (Optional)
    m_cvcInput = new QLineEdit(this);
    m_cvcInput->setPlaceholderText("•••");
    m_cvcInput->setEchoMode(QLineEdit::Password);
    m_cvcInput->setMaxLength(4);
    QRegularExpressionValidator *cvcValidator =
        new QRegularExpressionValidator(QRegularExpression("[0-9]{0,4}"), this);
    m_cvcInput->setValidator(cvcValidator);
    QLabel *cvcError = new QLabel(this);
    cvcError->setStyleSheet("color: #F44336; font-size: 11px;");
    cvcError->hide();
    m_errorLabels["cvc"] = cvcError;

    QLabel *cvcInfo = new QLabel("CVC will be encrypted and stored securely", this);
    cvcInfo->setStyleSheet("color: #666; font-size: 11px;");

    QVBoxLayout *cvcLayout = new QVBoxLayout();
    cvcLayout->addWidget(m_cvcInput);
    cvcLayout->addWidget(cvcInfo);
    cvcLayout->addWidget(cvcError);
    formLayout->addRow("CVC (Optional):", cvcLayout);

    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(20);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    if (m_isEditMode) {
        m_deleteButton = new QPushButton("Delete Card", this);
        m_deleteButton->setStyleSheet(
            "QPushButton { background-color: #F44336; color: white; padding: 8px 16px; "
            "border-radius: 4px; font-weight: bold; }"
            "QPushButton:hover { background-color: #D32F2F; }");
        buttonLayout->addWidget(m_deleteButton);
        buttonLayout->addStretch();
    } else {
        m_deleteButton = nullptr;
    }

    m_cancelButton = new QPushButton("Cancel", this);
    m_cancelButton->setStyleSheet(
        "QPushButton { background-color: #757575; color: white; padding: 8px 16px; "
        "border-radius: 4px; }"
        "QPushButton:hover { background-color: #616161; }");

    m_saveButton = new QPushButton("Save", this);
    m_saveButton->setStyleSheet(
        "QPushButton { background-color: #E91E63; color: white; padding: 8px 16px; "
        "border-radius: 4px; font-weight: bold; }"
        "QPushButton:hover { background-color: #C2185B; }"
        "QPushButton:disabled { background-color: #BDBDBD; }");
    m_saveButton->setEnabled(false);

    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_saveButton);

    mainLayout->addLayout(buttonLayout);
}

void CardFormDialog::setupConnections()
{
    connect(m_saveButton, &QPushButton::clicked, this, &CardFormDialog::onSaveClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &CardFormDialog::onCancelClicked);

    if (m_deleteButton) {
        connect(m_deleteButton, &QPushButton::clicked, this, &CardFormDialog::onDeleteClicked);
    }

    // Field validation
    connect(m_nicknameInput, &QLineEdit::textChanged, this, &CardFormDialog::onFieldChanged);
    connect(m_issuerInput, &QLineEdit::textChanged, this, &CardFormDialog::onFieldChanged);
    connect(m_last4Input, &QLineEdit::textChanged, this, &CardFormDialog::onFieldChanged);
    connect(m_expiryInput, &QDateEdit::dateChanged, this, &CardFormDialog::onFieldChanged);
}

void CardFormDialog::loadCard(const PaymentCard &card)
{
    m_nicknameInput->setText(card.getNickname());
    m_issuerInput->setText(card.getIssuer());
    m_last4Input->setText(card.getLast4Digits());

    // Load expiry date (always use first day of month for display consistency)
    QDate expiry = card.getExpiryDate();
    m_expiryInput->setDate(QDate(expiry.year(), expiry.month(), 1));

    if (card.hasCVC()) {
        m_originalCVC = "•••";
        m_cvcInput->setText("");
        m_cvcInput->setPlaceholderText("••• (saved, leave empty to keep)");
    }

    onFieldChanged(); // Enable save button
}

void CardFormDialog::onFieldChanged()
{
    clearFieldErrors();

    // Enable save button if required fields are filled
    bool hasNickname = !m_nicknameInput->text().simplified().isEmpty();
    bool hasIssuer = !m_issuerInput->text().simplified().isEmpty();
    bool hasLast4 = m_last4Input->text().length() == 4;
    bool hasExpiry = m_expiryInput->date().isValid();

    m_saveButton->setEnabled(hasNickname && hasIssuer && hasLast4 && hasExpiry);
}

void CardFormDialog::onSaveClicked()
{
    if (!validateForm()) {
        return;
    }

    // Create/update card object
    m_card.setNickname(m_nicknameInput->text().simplified());
    m_card.setIssuer(m_issuerInput->text().simplified());
    m_card.setLast4Digits(m_last4Input->text());

    // Store expiry date as first day of the month (credit cards expire on MM/YY)
    QDate expiry = m_expiryInput->date();
    m_card.setExpiryDate(QDate(expiry.year(), expiry.month(), 1));

    accept();
}

void CardFormDialog::onCancelClicked()
{
    reject();
}

void CardFormDialog::onDeleteClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Delete Card?",
        QString("Are you sure you want to delete \"%1\"?\n\n"
                "This action cannot be undone.")
            .arg(m_card.getNickname()),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        if (m_manager->deleteCard(m_card.getId())) {
            accept();
        } else {
            QMessageBox::critical(this, "Error", "Failed to delete card. Please try again.");
        }
    }
}

bool CardFormDialog::validateForm()
{
    bool isValid = true;

    // Validate nickname
    if (m_nicknameInput->text().simplified().isEmpty()) {
        showFieldError("nickname", "Card nickname is required");
        isValid = false;
    }

    // Validate issuer
    if (m_issuerInput->text().simplified().isEmpty()) {
        showFieldError("issuer", "Issuer/Bank is required");
        isValid = false;
    }

    // Validate last 4 digits
    if (m_last4Input->text().length() != 4) {
        showFieldError("last4", "Must be exactly 4 digits");
        isValid = false;
    }

    // Validate expiry
    QDate expiry = m_expiryInput->date();
    QDate today = QDate::currentDate();
    QDate cardExpiry(expiry.year(), expiry.month(), expiry.daysInMonth());
    if (cardExpiry < today) {
        showFieldError("expiry", "Expiry date must be in the future");
        isValid = false;
    }

    // Validate CVC if provided
    QString cvc = m_cvcInput->text();
    if (!cvc.isEmpty() && (cvc.length() < 3 || cvc.length() > 4)) {
        showFieldError("cvc", "CVC must be 3-4 digits");
        isValid = false;
    }

    // Check for duplicates
    if (m_manager->isDuplicate(m_last4Input->text(), expiry, m_card.getId())) {
        QMessageBox::StandardButton reply = QMessageBox::warning(
            this,
            "Duplicate Card",
            "A card with these details already exists. Do you want to add it anyway?",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

        if (reply != QMessageBox::Yes) {
            isValid = false;
        }
    }

    return isValid;
}

void CardFormDialog::showFieldError(const QString &fieldName, const QString &error)
{
    if (m_errorLabels.contains(fieldName)) {
        m_errorLabels[fieldName]->setText(error);
        m_errorLabels[fieldName]->show();
    }
}

void CardFormDialog::clearFieldErrors()
{
    for (QLabel *label : m_errorLabels) {
        label->hide();
    }
}

PaymentCard CardFormDialog::getCard() const
{
    return m_card;
}
