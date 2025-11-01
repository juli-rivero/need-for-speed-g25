#include "create_game_dialog.h"
#include "theme_manager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QMessageBox>

CreateGameDialog::CreateGameDialog(QWidget* parent)
    : QWidget(parent) {  
    setupUI();
    
    // Conectar al sistema de temas
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &CreateGameDialog::applyTheme);
    applyTheme();  // Aplicar tema inicial
}

CreateGameDialog::~CreateGameDialog() {
    // Qt maneja la limpieza automáticamente
}

void CreateGameDialog::setupUI() {
    // Layout principal
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 30, 50, 30);

    // Título
    QLabel* titleLabel = new QLabel("🎮 Crear Nueva Partida", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(20);

    // Formulario de configuración
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setContentsMargins(20, 20, 20, 20);

    // Nombre de la partida
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("Ej: Carrera Épica");
    nameEdit->setMaxLength(30);
    nameEdit->setMinimumHeight(35);
    QLabel* nameLabel = new QLabel("📝 Nombre de la partida:", this);
    formLayout->addRow(nameLabel, nameEdit);

    // Número de jugadores
    playersSpin = new QSpinBox(this);
    playersSpin->setMinimum(2);
    playersSpin->setMaximum(8);
    playersSpin->setValue(4);
    playersSpin->setSuffix(" jugadores");
    playersSpin->setMinimumHeight(35);
    QLabel* playersLabel = new QLabel("👥 Cantidad de jugadores:", this);
    formLayout->addRow(playersLabel, playersSpin);

    // Número de carreras
    racesSpin = new QSpinBox(this);
    racesSpin->setMinimum(1);
    racesSpin->setMaximum(10);
    racesSpin->setValue(3);
    racesSpin->setSuffix(" carreras");
    racesSpin->setMinimumHeight(35);
    QLabel* racesLabel = new QLabel("🏁 Cantidad de carreras:", this);
    formLayout->addRow(racesLabel, racesSpin);

    // Número de vueltas
    lapsSpin = new QSpinBox(this);
    lapsSpin->setMinimum(1);
    lapsSpin->setMaximum(10);
    lapsSpin->setValue(3);
    lapsSpin->setSuffix(" vueltas");
    lapsSpin->setMinimumHeight(35);
    QLabel* lapsLabel = new QLabel("🔄 Vueltas por carrera:", this);
    formLayout->addRow(lapsLabel, lapsSpin);

    // Ciudad/Mapa
    cityCombo = new QComboBox(this);
    cityCombo->addItem("🏙️  Liberty City", "city");
    cityCombo->addItem("🏙️  San Andreas", "city");
    cityCombo->addItem("🏙️  Vice City", "city");
    cityCombo->setMinimumHeight(35);
    QLabel* cityLabel = new QLabel("🗺️  Mapa/Ciudad:", this);
    formLayout->addRow(cityLabel, cityCombo);

    mainLayout->addLayout(formLayout);

    mainLayout->addStretch();

    // Botones
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();

    cancelButton = new QPushButton("❌ Cancelar", this);
    cancelButton->setMinimumHeight(45);
    cancelButton->setMinimumWidth(150);
    connect(cancelButton, &QPushButton::clicked, this, &CreateGameDialog::onCancelClicked);
    buttonsLayout->addWidget(cancelButton);

    createButton = new QPushButton("✅ Crear Partida", this);
    createButton->setMinimumHeight(45);
    createButton->setMinimumWidth(150);
    createButton->setEnabled(false); // Deshabilitado hasta que haya nombre
    connect(createButton, &QPushButton::clicked, this, &CreateGameDialog::onSubmitClicked);
    buttonsLayout->addWidget(createButton);

    mainLayout->addLayout(buttonsLayout);

    // Conectar validación
    connect(nameEdit, &QLineEdit::textChanged, this, &CreateGameDialog::validateInput);
    
    // Foco inicial en el nombre
    nameEdit->setFocus();
}

void CreateGameDialog::validateInput() {
    // Habilitar botón crear solo si hay nombre
    QString name = nameEdit->text().trimmed();
    createButton->setEnabled(!name.isEmpty());
}

CreateGameDialog::GameConfig CreateGameDialog::getConfig() const {
    GameConfig config;
    config.name = nameEdit->text().trimmed();
    config.maxPlayers = playersSpin->value();
    config.raceCount = racesSpin->value();
    config.lapCount = lapsSpin->value();  
    config.city = cityCombo->currentData().toString();
    return config;
}

void CreateGameDialog::reset() {
    nameEdit->clear();
    playersSpin->setValue(4);
    racesSpin->setValue(3);
    lapsSpin->setValue(3);
    cityCombo->setCurrentIndex(0);
    createButton->setEnabled(false);
}

void CreateGameDialog::onSubmitClicked() {
    emit submitRequested();
}

void CreateGameDialog::onCancelClicked() {
    emit cancelRequested();
}

void CreateGameDialog::applyTheme() {
    ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();
    
    // Aplicar estilo al widget
    setStyleSheet(QString(
        "QWidget {"
        "  background-color: %1;"
        "}"
        "QLabel {"
        "  color: %2;"
        "}"
        "QLineEdit, QSpinBox, QComboBox {"
        "  background-color: %3;"
        "  color: %2;"
        "  border: 1px solid %4;"
        "  border-radius: 4px;"
        "  padding: 5px;"
        "}"
        "QLineEdit:focus, QSpinBox:focus, QComboBox:focus {"
        "  border: 2px solid %5;"
        "}"
    )
    .arg(palette.cardBackgroundHover)
    .arg(palette.textPrimary)
    .arg(palette.cardBackground)
    .arg(palette.borderColor)
    .arg(palette.primaryColor));
    
    // Aplicar estilos a los botones
    createButton->setStyleSheet(theme.buttonPrimaryStyle());
    cancelButton->setStyleSheet(theme.buttonSecondaryStyle());
}
