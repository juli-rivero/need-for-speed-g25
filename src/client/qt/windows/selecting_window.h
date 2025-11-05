#pragma once

#include <QButtonGroup>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <vector>

#include "client/connexion/connexion_controller.h"

// Estructura para representar un tipo de auto
struct CarType {
    QString name;
    QString description;
    float maxSpeed;
    float acceleration;
    float health;
    float mass;
    float controllability;
    QString emoji;
};

// Estructura de configuración del auto seleccionado
struct CarConfig {
    QString carType;
    QString carName;
    float maxSpeed;
    float acceleration;
    float health;
    float mass;
    float controllability;
};

class SelectingWindow final : public QWidget {
    Q_OBJECT

    IConnexionController& connexionController;

   public:
    explicit SelectingWindow(QWidget* parent, IConnexionController&);
    ~SelectingWindow() override;

    CarConfig getSelectedCar() const;
    int getSelectedCarType() const { return selectedCarIndex; }
    void reset();

   signals:
    void carSelected(int carType);
    void confirmRequested();
    void cancelRequested();

   private slots:
    void onCarSelected(int carIndex);
    void onConfirmClicked();
    void onCancelClicked();
    void applyTheme();

   private:
    void initCarTypes();
    void setupUI();
    QWidget* createCarCard(const CarType& car, int index);
    QProgressBar* createStatBar(float value);
    void updateCarDetails(int carIndex);

    std::vector<CarType> carTypes;
    int selectedCarIndex;

    // UI Components
    QVBoxLayout* mainLayout;
    QWidget* carCardsContainer;
    QButtonGroup* carButtonGroup;

    // Panel de detalles
    QLabel* carNameLabel;
    QLabel* carDescLabel;
    QProgressBar* speedBar;
    QProgressBar* accelBar;
    QProgressBar* healthBar;
    QProgressBar* massBar;
    QProgressBar* controlBar;

    QPushButton* confirmButton;
    QPushButton* cancelButton;
};
