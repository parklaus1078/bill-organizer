#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>
#include <QMap>
#include "../models/payment_card.h"

class PaymentCardManager;

/**
 * @brief Dialog for adding or editing payment cards
 *
 * Provides a form interface for entering card details with validation.
 * Can be used in both "Add" and "Edit" modes.
 */
class CardFormDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CardFormDialog(QWidget *parent = nullptr);
    explicit CardFormDialog(const PaymentCard &card, QWidget *parent = nullptr);

    PaymentCard getCard() const;
    QString getCVC() const { return m_cvcInput->text(); }

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void onDeleteClicked();
    void onFieldChanged();

private:
    void setupUI();
    void setupConnections();
    void loadCard(const PaymentCard &card);
    bool validateForm();
    void showFieldError(const QString &fieldName, const QString &error);
    void clearFieldErrors();

    // UI Components
    QLineEdit *m_nicknameInput;
    QLineEdit *m_issuerInput;
    QLineEdit *m_last4Input;
    QDateEdit *m_expiryInput;
    QLineEdit *m_cvcInput;

    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;
    QPushButton *m_deleteButton;

    QMap<QString, QLabel*> m_errorLabels;

    // State
    PaymentCard m_card;
    PaymentCardManager *m_manager;
    bool m_isEditMode;
    QString m_originalCVC; // Store "•••" for edit mode
};
