#pragma once

#include <QButtonGroup>
#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <string>
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
};
