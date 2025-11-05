#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QWidget>

#include "client/connexion/connexion_controller.h"

class CreatingWindow final : public QWidget {
    Q_OBJECT

    IConnexionController& connexionController;

   public:
    struct GameConfig {
        QString name;
        int maxPlayers;
        int raceCount;
        int lapCount;
        QString city;
    };

    explicit CreatingWindow(QWidget* parent, IConnexionController&);
    ~CreatingWindow() override;

    GameConfig getConfig() const;
    void reset();
   signals:
    void submitRequested();
    void cancelRequested();

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
