#include "car_selection_dialog.h"
#include "theme_manager.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QScrollArea>

CarSelectionDialog::CarSelectionDialog(QWidget* parent)
    : QDialog(parent), selectedCarIndex(0) {
    
    setWindowTitle("🏎️ Seleccionar Auto");
    setMinimumSize(900, 600);
    
    initCarTypes();
    setupUI();
    
    // Conectar al cambio de tema global
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged,
            this, &CarSelectionDialog::applyTheme);
    
    // Aplicar tema inicial
    applyTheme();

    // Seleccionar el primer auto por defecto
    if (!carTypes.empty()) {
        updateCarDetails(0);
    }
}

void CarSelectionDialog::initCarTypes() {
    // Inicializar tipos de autos con sus características
    carTypes = {
        {
            "Deportivo",
            "Rápido y ágil, pero frágil. Ideal para circuitos urbanos.",
            0.95f,  // maxSpeed
            0.90f,  // acceleration
            0.50f,  // health
            0.40f,  // mass
            0.85f,  // controllability
            "🏎️"
        },
        {
            "Sedán",
            "Balanceado en todas sus características. Buena opción para principiantes.",
            0.70f,  // maxSpeed
            0.70f,  // acceleration
            0.70f,  // health
            0.65f,  // mass
            0.75f,  // controllability
            "🚗"
        },
        {
            "SUV",
            "Resistente y pesado. Puede embestir a otros sin mucho daño.",
            0.60f,  // maxSpeed
            0.55f,  // acceleration
            0.85f,  // health
            0.85f,  // mass
            0.60f,  // controllability
            "🚙"
        },
        {
            "Camión",
            "Tanque rodante. Muy lento pero casi indestructible.",
            0.45f,  // maxSpeed
            0.40f,  // acceleration
            0.95f,  // health
            0.95f,  // mass
            0.50f,  // controllability
            "🚚"
        },
        {
            "Muscle Car",
            "Aceleración brutal en línea recta, pero difícil de controlar en curvas.",
            0.85f,  // maxSpeed
            0.95f,  // acceleration
            0.60f,  // health
            0.70f,  // mass
            0.55f,  // controllability
            "🚗"
        },
        {
            "Compacto",
            "Pequeño y maniobrable. Excelente para esquivar el tráfico.",
            0.65f,  // maxSpeed
            0.75f,  // acceleration
            0.45f,  // health
            0.35f,  // mass
            0.90f,  // controllability
            "🚕"
        }
    };
}

void CarSelectionDialog::setupUI() {
    mainLayout = new QVBoxLayout(this);
    
    // Título
    QLabel* titleLabel = new QLabel("Elige tu auto para la carrera");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);
    
    // Layout horizontal: lista de autos + panel de detalles
    QHBoxLayout* contentLayout = new QHBoxLayout();
    
    // === Panel izquierdo: Lista de autos ===
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumWidth(500);
    
    carCardsContainer = new QWidget();
    QVBoxLayout* cardsLayout = new QVBoxLayout(carCardsContainer);
    
    carButtonGroup = new QButtonGroup(this);
    carButtonGroup->setExclusive(true);
    
    for (size_t i = 0; i < carTypes.size(); ++i) {
        QWidget* card = createCarCard(carTypes[i], i);
        cardsLayout->addWidget(card);
    }
    
    cardsLayout->addStretch();
    scrollArea->setWidget(carCardsContainer);
    contentLayout->addWidget(scrollArea, 2);
    
    // === Panel derecho: Detalles del auto seleccionado ===
    QGroupBox* detailsGroup = new QGroupBox("Estadísticas Detalladas");
    detailsGroup->setMinimumWidth(300);
    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsGroup);
    
    carNameLabel = new QLabel();
    detailsLayout->addWidget(carNameLabel);
    
    carDescLabel = new QLabel();
    carDescLabel->setWordWrap(true);
    detailsLayout->addWidget(carDescLabel);
    
    detailsLayout->addSpacing(10);
    
    // Barras de estadísticas
    auto addStatBar = [&](const QString& label, QProgressBar*& bar) {
        QLabel* statLabel = new QLabel(label);
        detailsLayout->addWidget(statLabel);
        
        bar = createStatBar(0.0f);
        detailsLayout->addWidget(bar);
        detailsLayout->addSpacing(5);
    };
    
    addStatBar("⚡ Velocidad Máxima", speedBar);
    addStatBar("🚀 Aceleración", accelBar);
    addStatBar("❤️ Salud", healthBar);
    addStatBar("⚖️ Masa", massBar);
    addStatBar("🎮 Controlabilidad", controlBar);
    
    detailsLayout->addStretch();
    contentLayout->addWidget(detailsGroup, 1);
    
    mainLayout->addLayout(contentLayout);
    
    // === Botones de acción ===
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    cancelButton = new QPushButton("Cancelar");
    cancelButton->setMinimumWidth(100);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    buttonLayout->addWidget(cancelButton);
    
    confirmButton = new QPushButton("Confirmar Selección");
    confirmButton->setMinimumWidth(150);
    connect(confirmButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(confirmButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // Conectar señales
    connect(carButtonGroup, QOverload<int>::of(&QButtonGroup::idClicked),
            this, &CarSelectionDialog::onCarSelected);
}

QWidget* CarSelectionDialog::createCarCard(const CarType& car, int index) {
    QWidget* card = new QWidget();
    
    QHBoxLayout* cardLayout = new QHBoxLayout(card);
    
    // Radio button
    QRadioButton* radioBtn = new QRadioButton();
    if (index == 0) {
        radioBtn->setChecked(true);
    }
    carButtonGroup->addButton(radioBtn, index);
    cardLayout->addWidget(radioBtn);
    
    // Emoji del auto
    QLabel* emojiLabel = new QLabel(car.emoji);
    emojiLabel->setStyleSheet("font-size: 32px;");
    cardLayout->addWidget(emojiLabel);
    
    // Información del auto
    QVBoxLayout* infoLayout = new QVBoxLayout();
    
    QLabel* nameLabel = new QLabel(car.name);
    infoLayout->addWidget(nameLabel);
    
    // Mini barras de estadísticas
    QHBoxLayout* miniStatsLayout = new QHBoxLayout();
    miniStatsLayout->setSpacing(3);
    
    auto addMiniBar = [&](const QString& label, float value) {
        QVBoxLayout* miniBarLayout = new QVBoxLayout();
        QLabel* miniLabel = new QLabel(label);
        miniLabel->setStyleSheet("font-size: 9px;");
        miniLabel->setAlignment(Qt::AlignCenter);
        miniBarLayout->addWidget(miniLabel);
        
        QProgressBar* miniBar = createStatBar(value);
        miniBar->setMaximumHeight(8);
        miniBar->setTextVisible(false);
        miniBarLayout->addWidget(miniBar);
        
        miniStatsLayout->addLayout(miniBarLayout);
    };
    
    addMiniBar("VEL", car.maxSpeed);
    addMiniBar("ACE", car.acceleration);
    addMiniBar("SAL", car.health);
    addMiniBar("MAS", car.mass);
    addMiniBar("CTR", car.controllability);
    
    infoLayout->addLayout(miniStatsLayout);
    cardLayout->addLayout(infoLayout, 1);
    
    return card;
}

QProgressBar* CarSelectionDialog::createStatBar(float value) {
    QProgressBar* bar = new QProgressBar();
    bar->setRange(0, 100);
    bar->setValue(static_cast<int>(value * 100));
    bar->setTextVisible(true);
    bar->setFormat("%v%");
    
    // El estilo se aplicará en applyTheme()
    
    return bar;
}

void CarSelectionDialog::onCarSelected(int carIndex) {
    selectedCarIndex = carIndex;
    updateCarDetails(carIndex);
}

void CarSelectionDialog::updateCarDetails(int carIndex) {
    if (carIndex < 0 || carIndex >= static_cast<int>(carTypes.size())) {
        return;
    }
    
    const CarType& car = carTypes[carIndex];
    
    carNameLabel->setText(car.emoji + " " + car.name);
    carDescLabel->setText(car.description);
    
    speedBar->setValue(static_cast<int>(car.maxSpeed * 100));
    accelBar->setValue(static_cast<int>(car.acceleration * 100));
    healthBar->setValue(static_cast<int>(car.health * 100));
    massBar->setValue(static_cast<int>(car.mass * 100));
    controlBar->setValue(static_cast<int>(car.controllability * 100));
    
    // Aplicar estilos del tema a las barras
    ThemeManager& theme = ThemeManager::instance();
    speedBar->setStyleSheet(theme.carStatBarStyle(car.maxSpeed));
    accelBar->setStyleSheet(theme.carStatBarStyle(car.acceleration));
    healthBar->setStyleSheet(theme.carStatBarStyle(car.health));
    massBar->setStyleSheet(theme.carStatBarStyle(car.mass));
    controlBar->setStyleSheet(theme.carStatBarStyle(car.controllability));
}

CarConfig CarSelectionDialog::getSelectedCar() const {
    if (selectedCarIndex < 0 || selectedCarIndex >= static_cast<int>(carTypes.size())) {
        // Retornar el primero por defecto
        const CarType& car = carTypes[0];
        return {car.name, car.name, car.maxSpeed, car.acceleration, 
                car.health, car.mass, car.controllability};
    }
    
    const CarType& car = carTypes[selectedCarIndex];
    return {
        car.name,
        car.name,
        car.maxSpeed,
        car.acceleration,
        car.health,
        car.mass,
        car.controllability
    };
}

void CarSelectionDialog::applyTheme() {
    // Obtener el tema actual
    ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();
    
    // Aplicar estilo al diálogo principal
    this->setStyleSheet(QString(
        "QDialog {"
        "    background-color: %1;"
        "}"
        "QLabel {"
        "    color: %2;"
        "}"
        "QGroupBox {"
        "    color: %2;"
        "    border: 2px solid %3;"
        "    border-radius: 5px;"
        "    margin-top: 10px;"
        "    padding-top: 10px;"
        "    background-color: %4;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px;"
        "    font-weight: bold;"
        "}"
        "QScrollArea {"
        "    border: none;"
        "    background-color: %1;"
        "}"
    ).arg(palette.cardBackgroundHover)
      .arg(palette.textPrimary)
      .arg(palette.borderColor)
      .arg(palette.cardBackground));
    
    // Actualizar estilo del título
    QList<QLabel*> labels = findChildren<QLabel*>();
    for (QLabel* label : labels) {
        if (label->text().contains("Elige tu auto")) {
            label->setStyleSheet(QString(
                "font-size: 18px;"
                "font-weight: bold;"
                "padding: 10px;"
                "color: %1;"
            ).arg(palette.textPrimary));
        }
    }
    
    // Actualizar estilos de las tarjetas de autos
    if (carCardsContainer) {
        QList<QWidget*> cards = carCardsContainer->findChildren<QWidget*>();
        for (QWidget* card : cards) {
            if (card->layout() && card->layout()->count() > 0) {
                card->setStyleSheet(theme.carCardStyle());
            }
        }
    }
    
    // Actualizar labels del nombre del auto en las tarjetas
    if (carCardsContainer) {
        QList<QLabel*> nameLabels = carCardsContainer->findChildren<QLabel*>();
        for (QLabel* label : nameLabels) {
            if (!label->text().isEmpty() && label->text().length() < 20) {
                label->setStyleSheet(QString(
                    "font-size: 14px;"
                    "font-weight: bold;"
                    "color: %1;"
                ).arg(palette.textPrimary));
            }
        }
    }
    
    // Actualizar labels de estadísticas en el panel derecho
    if (carNameLabel) {
        carNameLabel->setStyleSheet(QString(
            "font-size: 20px;"
            "font-weight: bold;"
            "color: %1;"
        ).arg(palette.textPrimary));
    }
    
    if (carDescLabel) {
        carDescLabel->setStyleSheet(QString(
            "color: %1;"
            "margin-bottom: 10px;"
        ).arg(palette.textSecondary));
    }
    
    // Actualizar labels de estadísticas (⚡, 🚀, etc.)
    QList<QLabel*> statLabels = findChildren<QLabel*>();
    for (QLabel* label : statLabels) {
        QString text = label->text();
        if (text.contains("⚡") || text.contains("🚀") || 
            text.contains("❤️") || text.contains("⚖️") || text.contains("🎮")) {
            label->setStyleSheet(QString(
                "font-weight: bold;"
                "color: %1;"
            ).arg(palette.textPrimary));
        }
    }
    
    // Actualizar botones
    if (confirmButton) {
        confirmButton->setStyleSheet(theme.buttonPrimaryStyle());
    }
    
    if (cancelButton) {
        cancelButton->setStyleSheet(theme.buttonSecondaryStyle());
    }
    
    // Re-aplicar estilos a las barras de progreso actuales
    if (selectedCarIndex >= 0 && selectedCarIndex < static_cast<int>(carTypes.size())) {
        const CarType& car = carTypes[selectedCarIndex];
        speedBar->setStyleSheet(theme.carStatBarStyle(car.maxSpeed));
        accelBar->setStyleSheet(theme.carStatBarStyle(car.acceleration));
        healthBar->setStyleSheet(theme.carStatBarStyle(car.health));
        massBar->setStyleSheet(theme.carStatBarStyle(car.mass));
        controlBar->setStyleSheet(theme.carStatBarStyle(car.controllability));
    }
}
