#include "client/qt/windows/creating_window.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <vector>

#include "client/qt/theme_manager.h"

CreatingWindow::CreatingWindow(QWidget* parent, Connexion& connexion)
    : QWidget(parent), api(connexion.get_api()) {
    setupUI();

    // Conectar al sistema de temas
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this,
            &CreatingWindow::applyTheme);
    applyTheme();  // Aplicar tema inicial

    Responder::subscribe(connexion);
}
CreatingWindow::~CreatingWindow() { Responder::unsubscribe(); }

void CreatingWindow::on_join_response(const SessionInfo&,
                                      const std::vector<CarStaticInfo>&) {
    QMetaObject::invokeMethod(
        this, [this]() { emit sessionCreated(); }, Qt::QueuedConnection);
}

void CreatingWindow::setupUI() {
    // Layout principal
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 20, 40, 20);  // Menos márgenes

    // Título
    QLabel* titleLabel = new QLabel("🎮 Crear Nueva Partida", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(20);  // Título más pequeño
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(15);

    // Formulario de configuración
    QFormLayout* formLayout = new QFormLayout();
    formLayout->setSpacing(12);  // Menos spacing
    formLayout->setContentsMargins(15, 15, 15, 15);

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

    // Ciudad/Mapa
    cityCombo = new QComboBox(this);
    cityCombo->addItem("🏙️  Liberty City", "LibertyCity");
    cityCombo->addItem("🏙️  San Andreas", "SanAndreas");
    cityCombo->addItem("🏙️  Vice City", "ViceCity");
    cityCombo->setMinimumHeight(35);
    QLabel* cityLabel = new QLabel("🗺️  Mapa/Ciudad:", this);
    formLayout->addRow(cityLabel, cityCombo);

    mainLayout->addLayout(formLayout);
    mainLayout->addStretch();

    // Botones - más compactos
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();

    cancelButton = new QPushButton("❌ Cancelar", this);
    cancelButton->setMinimumHeight(40);  // Más compacto
    cancelButton->setMinimumWidth(120);
    connect(cancelButton, &QPushButton::clicked, this,
            &CreatingWindow::onCancelClicked);
    buttonsLayout->addWidget(cancelButton);

    createButton = new QPushButton("✅ Crear Partida", this);
    createButton->setMinimumHeight(40);
    createButton->setMinimumWidth(120);
    createButton->setEnabled(false);
    connect(createButton, &QPushButton::clicked, this,
            &CreatingWindow::onSubmitClicked);
    buttonsLayout->addWidget(createButton);

    mainLayout->addLayout(buttonsLayout);

    // Conectar validación
    connect(nameEdit, &QLineEdit::textChanged, this,
            &CreatingWindow::validateInput);

    nameEdit->setFocus();
}

void CreatingWindow::reset() {
    nameEdit->clear();
    playersSpin->setValue(4);
    racesSpin->setValue(3);
    cityCombo->setCurrentIndex(0);
    createButton->setEnabled(false);
}

void CreatingWindow::validateInput() {
    // Habilitar botón crear solo si hay nombre
    QString name = nameEdit->text().trimmed();
    createButton->setEnabled(!name.isEmpty());
}

void CreatingWindow::onSubmitClicked() {
    api.request_create_and_join_session({
        .name = nameEdit->text().trimmed().toUtf8().constData(),
        .maxPlayers = static_castuint8_t(playersSpin->value()),
        .raceCount = static_cast<uint8_t>(racesSpin->value()),
        .city = cityCombo->currentData().toString().toUtf8().constData(),
    });
}

void CreatingWindow::onCancelClicked() { emit createCanceled(); }

void CreatingWindow::applyTheme() {
    ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();

    // Aplicar estilo al widget
    setStyleSheet(QString("QWidget {"
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
                          "}")
                      .arg(palette.cardBackgroundHover)
                      .arg(palette.textPrimary)
                      .arg(palette.cardBackground)
                      .arg(palette.borderColor)
                      .arg(palette.primaryColor));

    // Aplicar estilos a los botones
    createButton->setStyleSheet(theme.buttonPrimaryStyle());
    cancelButton->setStyleSheet(theme.buttonSecondaryStyle());
}
