#include "client/qt/windows/creating_window.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

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
    // TODO(nico): borrar, el numero vueltas en un circuito siempre es 1
    lapsSpin = new QSpinBox(this);
    lapsSpin->setMinimum(1);
    lapsSpin->setMaximum(10);
    lapsSpin->setValue(3);
    lapsSpin->setSuffix(" vueltas");
    lapsSpin->setMinimumHeight(35);
    QLabel* lapsLabel = new QLabel("🔄 Vueltas por carrera:", this);
    formLayout->addRow(lapsLabel, lapsSpin);

    // Ciudad/Mapa
    // TODO(juli): hacer que el cliente obtenga los mapas del servidor
    cityCombo = new QComboBox(this);
    cityCombo->addItem("🏙️  Liberty City", "LibertyCity");
    cityCombo->addItem("🏙️  San Andreas", "SanAndreas");
    cityCombo->addItem("🏙️  Vice City", "ViceCity");
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
    connect(cancelButton, &QPushButton::clicked, this,
            &CreatingWindow::onCancelClicked);
    buttonsLayout->addWidget(cancelButton);

    createButton = new QPushButton("✅ Crear Partida", this);
    createButton->setMinimumHeight(45);
    createButton->setMinimumWidth(150);
    createButton->setEnabled(false);  // Deshabilitado hasta que haya nombre
    connect(createButton, &QPushButton::clicked, this,
            &CreatingWindow::onSubmitClicked);
    buttonsLayout->addWidget(createButton);

    mainLayout->addLayout(buttonsLayout);

    // Conectar validación
    connect(nameEdit, &QLineEdit::textChanged, this,
            &CreatingWindow::validateInput);

    // Foco inicial en el nombre
    nameEdit->setFocus();
}

void CreatingWindow::validateInput() {
    // Habilitar botón crear solo si hay nombre
    QString name = nameEdit->text().trimmed();
    createButton->setEnabled(!name.isEmpty());
}

/*CreatingWindow::GameConfig CreatingWindow::getConfig() const {
    GameConfig config;
    config.name = nameEdit->text().trimmed();
    config.maxPlayers = playersSpin->value();
    config.raceCount = racesSpin->value();
    config.lapCount = lapsSpin->value();
    config.city = cityCombo->currentData().toString();
    return config;
}*/

void CreatingWindow::reset() {
    nameEdit->clear();
    playersSpin->setValue(4);
    racesSpin->setValue(3);
    lapsSpin->setValue(3);
    cityCombo->setCurrentIndex(0);
    createButton->setEnabled(false);
}

void CreatingWindow::onSubmitClicked() {
    api.request_create_and_join_session({
        .name = nameEdit->text().trimmed().toUtf8().constData(),
        .maxPlayers = static_cast<uint8_t>(playersSpin->value()),
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
