#pragma once

#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QWidget>

class CreateGameDialog : public QWidget {
    Q_OBJECT

   public:
    struct GameConfig {
        QString name;
        int maxPlayers;
        int raceCount;
        int lapCount;
        QString city;
    };

    explicit CreateGameDialog(QWidget* parent = nullptr);
    ~CreateGameDialog();

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
