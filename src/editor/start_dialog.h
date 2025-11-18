#pragma once

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

class StartDialog : public QDialog {
    Q_OBJECT

   public:
    enum Mode { EditRace, CreateMap, Cancelled };

    explicit StartDialog(QWidget* parent = nullptr);
    Mode getSelectedMode() const { return selectedMode; }

   private slots:
    void onEditRace();
    void onCreateMap();

   private:
    Mode selectedMode;
    QPushButton* editRaceButton;
    QPushButton* createMapButton;
};
