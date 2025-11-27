#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QWidget>
#include <vector>

#include "client/connexion/connexion.h"

class CreatingWindow final : public QWidget, Connexion::Responder {
    Q_OBJECT

    Api& api;

   public:
    explicit CreatingWindow(QWidget* parent, Connexion&);
    ~CreatingWindow() override;

    void on_join_response(const SessionInfo&,
                          const std::vector<CarInfo>&) override;

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
