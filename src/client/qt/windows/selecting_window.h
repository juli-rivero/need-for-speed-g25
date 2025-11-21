#pragma once

#include <QButtonGroup>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <string>
#include <unordered_map>
#include <vector>

#include "client/connexion/connexion.h"

class SelectingWindow final : public QWidget, Connexion::Responder {
    Q_OBJECT

    Api& api;

   public:
    explicit SelectingWindow(QWidget* parent, Connexion&);
    ~SelectingWindow() override;

    // CarConfig getSelectedCar() const;
    int getSelectedCarType() const { return selectedCarIndex; }
    void reset();

    void on_join_response(const SessionInfo&,
                          const std::vector<CarInfo>&) override;
    void on_leave_response() override;

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
    void setupUI();
    QWidget* createCarCard(const CarInfo& car, int index);
    QProgressBar* createStatBar(float value);
    void updateCarDetails(int carIndex);

    std::vector<CarInfo> carTypes;
    int selectedCarIndex;

    QLabel* speedValueLabel;
    QLabel* accelValueLabel;
    QLabel* healthValueLabel;
    QLabel* massValueLabel;
    QLabel* controlValueLabel;

    // UI Components
    QVBoxLayout* mainLayout;
    QWidget* carCardsContainer;
    QVBoxLayout* cardsLayout;
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

    QString getCarImagePath(CarType carType) {
        // Mapear tipos de auto a números de imagen
        std::unordered_map<CarType, int> carImageMap = {
            {CarType::Speedster, 1}, {CarType::Tank, 2},
            {CarType::Drifter, 3},   {CarType::Rocket, 4},
            {CarType::Classic, 5},   {CarType::Offroad, 6},
            {CarType::Ghost, 7}};

        int imageNumber = 1;  // Default

        auto it = carImageMap.find(carType);
        if (it != carImageMap.end()) {
            imageNumber = it->second;
        }

        // Ruta a las imágenes - asumiendo que están en assets/cars/
        return QString("assets/cars/car%1.png").arg(imageNumber);
    }

    QProgressBar* createMiniStatBar(float value);
    QLabel* carDetailImageLabel;
};
