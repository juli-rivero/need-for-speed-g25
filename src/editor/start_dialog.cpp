#include "editor/start_dialog.h"

#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

StartDialog::StartDialog(QWidget* parent)
    : QDialog(parent), selectedMode(Cancelled) {
    setWindowTitle("Need for Speed - Map Editor");
    setMinimumSize(500, 300);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(20);

    // Título
    QLabel* titleLabel = new QLabel("Bienvenido al Editor de Mapas", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Subtítulo
    QLabel* subtitleLabel = new QLabel("Selecciona el modo de edición:", this);
    QFont subtitleFont = subtitleLabel->font();
    subtitleFont.setPointSize(12);
    subtitleLabel->setFont(subtitleFont);
    subtitleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(subtitleLabel);

    mainLayout->addSpacing(20);

    // Botón: Editar Carrera
    editRaceButton = new QPushButton("🏁 Editar Carrera", this);
    editRaceButton->setMinimumHeight(60);
    QFont buttonFont = editRaceButton->font();
    buttonFont.setPointSize(14);
    editRaceButton->setFont(buttonFont);
    editRaceButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border-radius: 5px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #45a049;"
        "}");
    connect(editRaceButton, &QPushButton::clicked, this,
            &StartDialog::onEditRace);
    mainLayout->addWidget(editRaceButton);

    QLabel* editRaceDesc = new QLabel(
        "Carga mapas existentes y agrega checkpoints, inicio, fin y flechas de "
        "ayuda.",
        this);
    editRaceDesc->setWordWrap(true);
    editRaceDesc->setAlignment(Qt::AlignCenter);
    editRaceDesc->setStyleSheet("color: gray;");
    mainLayout->addWidget(editRaceDesc);

    mainLayout->addSpacing(10);

    // Botón: Crear Mapa
    createMapButton = new QPushButton("🏗️ Crear Mapa", this);
    createMapButton->setMinimumHeight(60);
    createMapButton->setFont(buttonFont);
    createMapButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border-radius: 5px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #0b7dda;"
        "}");
    connect(createMapButton, &QPushButton::clicked, this,
            &StartDialog::onCreateMap);
    mainLayout->addWidget(createMapButton);

    QLabel* createMapDesc = new QLabel(
        "Carga tu propia imagen PNG y utiliza todas las herramientas "
        "(edificios, "
        "checkpoints, etc.).",
        this);
    createMapDesc->setWordWrap(true);
    createMapDesc->setAlignment(Qt::AlignCenter);
    createMapDesc->setStyleSheet("color: gray;");
    mainLayout->addWidget(createMapDesc);

    mainLayout->addStretch();

    setLayout(mainLayout);
}

void StartDialog::onEditRace() {
    selectedMode = EditRace;
    accept();
}

void StartDialog::onCreateMap() {
    selectedMode = CreateMap;
    accept();
}
