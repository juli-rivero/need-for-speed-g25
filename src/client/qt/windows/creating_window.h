#pragma once

#include <client/connexion/requesters/creating_requester.h>

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QWidget>

#include "client/connexion/connexion.h"

class CreatingWindow final : public QWidget {
    Q_OBJECT

    Connexion& connexion;
    MockCreatingRequester api;

   public:
    explicit CreatingWindow(QWidget* parent, Connexion&);
    ~CreatingWindow() override;

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
