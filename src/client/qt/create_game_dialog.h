#ifndef CREATE_GAME_DIALOG_H
#define CREATE_GAME_DIALOG_H

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QString>

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

#endif // CREATE_GAME_DIALOG_H
