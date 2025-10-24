#ifndef CREATE_GAME_DIALOG_H
#define CREATE_GAME_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QString>

class CreateGameDialog : public QDialog {
    Q_OBJECT

public:
    struct GameConfig {
        QString name;
        int maxPlayers;
        int raceCount;
        QString city;
    };

    explicit CreateGameDialog(QWidget* parent = nullptr);
    ~CreateGameDialog();
    
    GameConfig getConfig() const;

private slots:
    void validateInput();
    void applyTheme();
    
private:
    void setupUI();
    
    // Widgets
    QLineEdit* nameEdit;
    QSpinBox* playersSpin;
    QSpinBox* racesSpin;
    QComboBox* cityCombo;
    QPushButton* createButton;
    QPushButton* cancelButton;
};

#endif // CREATE_GAME_DIALOG_H
