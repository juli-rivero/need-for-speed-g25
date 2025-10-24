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
    : QDialog(parent) {
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
    setWindowTitle("🎮 Crear Nueva Partida");
    setModal(true);
    setMinimumWidth(400);

    // Layout principal
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Título
    QLabel* titleLabel = new QLabel("Configuración de la Partida", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Separador
    QFrame* line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainLayout->addWidget(line);

    // Formulario de configuración
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(15);
    formLayout->setContentsMargins(20, 20, 20, 20);

    // Nombre de la partida
    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText("Ej: Carrera Épica");
    nameEdit->setMaxLength(30);
    QLabel* nameLabel = new QLabel("Nombre de la partida:", this);
    formLayout->addRow(nameLabel, nameEdit);

    // Número de jugadores
    playersSpin = new QSpinBox(this);
    playersSpin->setMinimum(2);
    playersSpin->setMaximum(8);
    playersSpin->setValue(4);
    playersSpin->setSuffix(" jugadores");
    QLabel* playersLabel = new QLabel("Cantidad de jugadores:", this);
    formLayout->addRow(playersLabel, playersSpin);

    // Número de carreras
    racesSpin = new QSpinBox(this);
    racesSpin->setMinimum(1);
    racesSpin->setMaximum(10);
    racesSpin->setValue(3);
    racesSpin->setSuffix(" carreras");
    QLabel* racesLabel = new QLabel("Cantidad de carreras:", this);
    formLayout->addRow(racesLabel, racesSpin);

    // Ciudad/Mapa
    cityCombo = new QComboBox(this);
    cityCombo->addItem("🏙️  Liberty City", "city");
    cityCombo->addItem("🏙️  San Andreas", "city");
    cityCombo->addItem("🏙️  Vice City", "city");
    QLabel* cityLabel = new QLabel("Mapa/Ciudad:", this);
    formLayout->addRow(cityLabel, cityCombo);

    mainLayout->addLayout(formLayout);

    // Información adicional
    QGroupBox* infoBox = new QGroupBox("ℹ️ Información", this);
    QVBoxLayout* infoLayout = new QVBoxLayout(infoBox);
    QLabel* infoLabel = new QLabel(
        "• La partida comenzará cuando todos los jugadores estén listos\n"
        "• Puedes invitar amigos \n",
        this
    );
    infoLabel->setWordWrap(true);
    infoLayout->addWidget(infoLabel);
    mainLayout->addWidget(infoBox);

    // Botones
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();

    cancelButton = new QPushButton("❌ Cancelar", this);
    cancelButton->setMinimumHeight(35);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonsLayout->addWidget(cancelButton);

    createButton = new QPushButton("✅ Crear Partida", this);
    createButton->setMinimumHeight(35);
    createButton->setEnabled(false); // Deshabilitado hasta que haya nombre
    connect(createButton, &QPushButton::clicked, this, &QDialog::accept);
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
    config.city = cityCombo->currentData().toString();
    return config;
}

void CreateGameDialog::applyTheme() {
    ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();
    
    // Aplicar estilo al diálogo
    setStyleSheet(QString(
        "QDialog {"
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
        "QGroupBox {"
        "  color: %2;"
        "  border: 1px solid %4;"
        "  border-radius: 5px;"
        "  margin-top: 10px;"
        "  padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "  subcontrol-origin: margin;"
        "  subcontrol-position: top left;"
        "  padding: 0 5px;"
        "}"
    )
    .arg(palette.cardBackground)
    .arg(palette.textPrimary)
    .arg(palette.cardBackgroundHover)
    .arg(palette.borderColor)
    .arg(palette.primaryColor));
    
    // Aplicar estilos a los botones
    createButton->setStyleSheet(theme.buttonPrimaryStyle());
    cancelButton->setStyleSheet(theme.buttonSecondaryStyle());
}
