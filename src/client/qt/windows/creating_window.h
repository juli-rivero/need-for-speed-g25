#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QWidget>

#include "client/connexion/connexion.h"

class CreatingWindow final : public QWidget, Connexion::Responder {
    Q_OBJECT

    Api& api;

   public:
    explicit CreatingWindow(QWidget* parent, Connexion&);

    // metodos que usa el controlador
    void createSession();

    void reset();
   signals:
    void sessionCreated();
    void createCanceled();

   private slots:
    void validateInput();
    void applyTheme();
    void onSubmitClicked();
    void onCancelClicked();

   private:
    void setupUI();

    // Widgets
    QLineEdit* nameEdit;
    QSpinBox* playersSpin;
    QSpinBox* racesSpin;
    QSpinBox* lapsSpin;
    QComboBox* cityCombo;
    QPushButton* createButton;
    QPushButton* cancelButton;
};
