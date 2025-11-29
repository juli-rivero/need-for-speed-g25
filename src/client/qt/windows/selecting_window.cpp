#include "client/qt/windows/selecting_window.h"

#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <unordered_map>
#include <vector>

#include "client/qt/theme_manager.h"
#include "client/qt/windows/car_assets.h"
#include "spdlog/spdlog.h"

SelectingWindow::SelectingWindow(QWidget* parent, Connexion& connexion)
    : QWidget(parent),
      api(connexion.get_api()),
      selectedCarIndex(0),
      carDetailImageLabel(nullptr) {  // Eliminar inicialización de máximos
    setupUI();

    // Conectar al cambio de tema global
    connect(&ThemeManager::instance(), &ThemeManager::themeChanged, this,
            &SelectingWindow::applyTheme);

    // Aplicar tema inicial
    applyTheme();

    Responder::subscribe(connexion);
}

SelectingWindow::~SelectingWindow() { Responder::unsubscribe(); }

void SelectingWindow::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 30, 50, 30);

    // Título
    QLabel* titleLabel = new QLabel("🏎️ Selecciona tu Auto");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(20);

    QHBoxLayout* contentLayout = new QHBoxLayout();

    // === Panel izquierdo: Lista de autos ===
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setMinimumWidth(500);

    carCardsContainer = new QWidget();
    cardsLayout = new QVBoxLayout(carCardsContainer);

    carButtonGroup = new QButtonGroup(this);
    carButtonGroup->setExclusive(true);

    // Mensaje de carga
    QLabel* loadingLabel = new QLabel("Cargando autos disponibles...");
    loadingLabel->setAlignment(Qt::AlignCenter);
    cardsLayout->addWidget(loadingLabel);

    cardsLayout->addStretch();
    scrollArea->setWidget(carCardsContainer);
    contentLayout->addWidget(scrollArea, 2);

    // === Panel derecho: Detalles del auto seleccionado ===
    QGroupBox* detailsGroup = new QGroupBox("Estadísticas Detalladas");
    detailsGroup->setMinimumWidth(300);
    QVBoxLayout* detailsLayout = new QVBoxLayout(detailsGroup);

    carNameLabel = new QLabel("Selecciona un auto");
    QFont nameFont = carNameLabel->font();
    nameFont.setPointSize(16);
    nameFont.setBold(true);
    carNameLabel->setFont(nameFont);
    detailsLayout->addWidget(carNameLabel);

    carDescLabel =
        new QLabel("Elige un auto de la lista para ver sus estadísticas");
    carDescLabel->setWordWrap(true);
    detailsLayout->addWidget(carDescLabel);

    detailsLayout->addSpacing(10);

    // SOLO VALORES NUMÉRICOS - SIN BARRAS DE PROGRESO
    auto addStat = [&](const QString& label, QLabel*& valueLabel) {
        QHBoxLayout* statLayout = new QHBoxLayout();

        QLabel* statLabel = new QLabel(label);
        statLabel->setMinimumWidth(140);
        statLayout->addWidget(statLabel);

        valueLabel = new QLabel("0");
        valueLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
        valueLabel->setAlignment(Qt::AlignRight);
        statLayout->addWidget(valueLabel);

        detailsLayout->addLayout(statLayout);
        detailsLayout->addSpacing(8);
    };

    // Crear solo labels de valor (sin barras)
    addStat("⚡ Velocidad Máxima:", speedValueLabel);
    addStat("🚀 Aceleración:", accelValueLabel);
    addStat("❤️ Salud:", healthValueLabel);
    addStat("⚖️ Masa:", massValueLabel);
    addStat("🎮 Controlabilidad:", controlValueLabel);

    detailsLayout->addStretch();
    contentLayout->addWidget(detailsGroup, 1);

    mainLayout->addLayout(contentLayout);

    // === Botones de acción ===
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    cancelButton = new QPushButton("❌ Cancelar");
    cancelButton->setMinimumWidth(150);
    cancelButton->setMinimumHeight(45);
    connect(cancelButton, &QPushButton::clicked, this,
            &SelectingWindow::onCancelClicked);
    buttonLayout->addWidget(cancelButton);

    confirmButton = new QPushButton("✅ Confirmar Selección");
    confirmButton->setMinimumWidth(150);
    confirmButton->setMinimumHeight(45);
    confirmButton->setEnabled(false);
    connect(confirmButton, &QPushButton::clicked, this,
            &SelectingWindow::onConfirmClicked);
    buttonLayout->addWidget(confirmButton);

    mainLayout->addLayout(buttonLayout);

    // Conectar señales
    connect(carButtonGroup, QOverload<int>::of(&QButtonGroup::idClicked), this,
            &SelectingWindow::onCarSelected);
}

// Modificar createCarCard para mostrar valores reales en mini-barras:
QWidget* SelectingWindow::createCarCard(const CarStaticInfo& car, int index) {
    QWidget* card = new QWidget();
    card->setFixedHeight(80);

    QHBoxLayout* cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(15, 8, 15, 8);
    cardLayout->setSpacing(12);

    // Radio button
    QRadioButton* radioBtn = new QRadioButton();
    radioBtn->setFixedSize(20, 20);
    if (index == 0) {
        radioBtn->setChecked(true);
    }
    carButtonGroup->addButton(radioBtn, index);
    cardLayout->addWidget(radioBtn);

    // IMAGEN del auto
    QLabel* carImageLabel = new QLabel();
    carImageLabel->setFixedSize(50, 50);
    carImageLabel->setAlignment(Qt::AlignCenter);
    carImageLabel->setStyleSheet(
        "border: 2px solid #ddd;"
        "border-radius: 8px;"
        "background-color: white;"
        "padding: 2px;");

    QString imagePath = getCarImagePath(car.type);
    QPixmap carPixmap(imagePath);

    if (!carPixmap.isNull()) {
        carPixmap = carPixmap.scaled(46, 46, Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation);
        carImageLabel->setPixmap(carPixmap);
    } else {
        carImageLabel->setStyleSheet(
            "font-size: 24px; border: none; background: none;");
    }

    cardLayout->addWidget(carImageLabel);

    // Información del auto
    QVBoxLayout* infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(4);

    // Nombre del auto
    QLabel* nameLabel = new QLabel(car.name.c_str());
    QFont nameFont = nameLabel->font();
    nameFont.setPointSize(11);
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);
    nameLabel->setStyleSheet("color: #2c3e50;");
    infoLayout->addWidget(nameLabel);

    // Mini estadísticas - MOSTRAR VALORES REALES
    QHBoxLayout* miniStatsLayout = new QHBoxLayout();
    miniStatsLayout->setSpacing(8);

    auto addMiniStat = [&](const QString& label, float value) {
        QVBoxLayout* miniStatLayout = new QVBoxLayout();
        miniStatLayout->setSpacing(1);

        QLabel* miniLabel = new QLabel(label);
        miniLabel->setStyleSheet(
            "font-size: 8px; font-weight: bold; color: #7f8c8d;");
        miniLabel->setAlignment(Qt::AlignCenter);
        miniLabel->setFixedWidth(20);
        miniStatLayout->addWidget(miniLabel);

        QLabel* valueLabel = new QLabel(QString::number(value, 'f', 0));
        valueLabel->setStyleSheet(
            "font-size: 9px; font-weight: bold; color: #2c3e50;");
        valueLabel->setAlignment(Qt::AlignCenter);
        valueLabel->setFixedWidth(20);
        miniStatLayout->addWidget(valueLabel);

        miniStatsLayout->addLayout(miniStatLayout);
    };

    // MOSTRAR VALORES REALES
    addMiniStat("VEL", car.maxSpeed);
    addMiniStat("ACE", car.acceleration);
    addMiniStat("SAL", car.health);
    addMiniStat("MAS", car.mass);
    addMiniStat("CTR", car.control);

    infoLayout->addLayout(miniStatsLayout);
    cardLayout->addLayout(infoLayout, 1);

    // Estilo de la tarjeta
    card->setStyleSheet(
        "QWidget {"
        "    background-color: white;"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 10px;"
        "    margin: 2px;"
        "}"
        "QWidget:hover {"
        "    background-color: #f8f9fa;"
        "    border-color: #3498db;"
        "}");

    return card;
}

QProgressBar* SelectingWindow::createMiniStatBar(float value) {
    QProgressBar* bar = new QProgressBar();
    bar->setFixedSize(20, 6);
    bar->setRange(0, 100);
    bar->setValue(static_cast<int>(value));
    bar->setTextVisible(false);

    // Estilo simple
    bar->setStyleSheet(
        "QProgressBar {"
        "    border: 1px solid #bdc3c7;"
        "    border-radius: 3px;"
        "    background-color: #ecf0f1;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #3498db;"
        "    border-radius: 2px;"
        "}");

    return bar;
}

QProgressBar* SelectingWindow::createStatBar(float value) {
    QProgressBar* bar = new QProgressBar();
    bar->setRange(0, 100);
    bar->setValue(static_cast<int>(value));
    bar->setTextVisible(true);
    bar->setFormat(
        QString::number(value, 'f', 1));  // ✅ Muestra el valor con 1 decimal

    return bar;
}

void SelectingWindow::onCarSelected(int carIndex) {
    selectedCarIndex = carIndex;
    updateCarDetails(carIndex);
}

void SelectingWindow::onConfirmClicked() {
    api.choose_car(carTypes[selectedCarIndex].name);
    emit confirmRequested();
}

void SelectingWindow::onCancelClicked() { api.request_leave_current_session(); }

void SelectingWindow::reset() {
    if (!carTypes.empty() && carButtonGroup->buttons().size() > 0) {
        carButtonGroup->buttons()[0]->setChecked(true);
        selectedCarIndex = 0;
        updateCarDetails(0);  // Asegurar que se actualice
    }
}

void SelectingWindow::on_join_response(
    const SessionInfo&, const std::vector<CarStaticInfo>& car_static_infos) {
    QMetaObject::invokeMethod(
        this,
        [this, car_static_infos]() {
            carTypes = car_static_infos;

            if (carTypes.empty()) {
                QLabel* errorLabel = new QLabel("No hay autos disponibles");
                errorLabel->setAlignment(Qt::AlignCenter);
                cardsLayout->addWidget(errorLabel);
                return;
            }

            // Limpiar layout
            QLayoutItem* item;
            while ((item = cardsLayout->takeAt(0)) != nullptr) {
                if (item->widget()) item->widget()->deleteLater();
                delete item;
            }

            // Crear tarjetas
            for (size_t i = 0; i < carTypes.size(); ++i) {
                QWidget* card = createCarCard(carTypes[i], i);
                cardsLayout->addWidget(card);
            }

            confirmButton->setEnabled(true);
            reset();
        },
        Qt::QueuedConnection);
}

void SelectingWindow::on_leave_response() {
    QMetaObject::invokeMethod(
        this, [this]() { emit cancelRequested(); }, Qt::QueuedConnection);
}

void SelectingWindow::updateCarDetails(int carIndex) {
    if (carIndex < 0 || carIndex >= static_cast<int>(carTypes.size())) {
        return;
    }

    const CarStaticInfo& car = carTypes[carIndex];

    // Actualizar textos
    carNameLabel->setText(car.name.c_str());
    carDescLabel->setText(car.description.c_str());

    speedValueLabel->setText(QString::number(car.maxSpeed));
    accelValueLabel->setText(QString::number(car.acceleration));
    healthValueLabel->setText(QString::number(car.health));
    massValueLabel->setText(QString::number(car.mass));
    controlValueLabel->setText(QString::number(car.control));

    // No hay barras, así que no aplicar estilos a barras inexistentes
}

void SelectingWindow::applyTheme() {
    // Obtener el tema actual
    ThemeManager& theme = ThemeManager::instance();
    const ColorPalette& palette = theme.getCurrentPalette();

    // Aplicar estilo al widget principal
    this->setStyleSheet(QString("QWidget {"
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
                                "}")
                            .arg(palette.cardBackgroundHover)
                            .arg(palette.textPrimary)
                            .arg(palette.borderColor)
                            .arg(palette.cardBackground));

    // Actualizar estilo del título
    QList<QLabel*> labels = findChildren<QLabel*>();
    for (QLabel* label : labels) {
        if (label->text().contains("Elige tu auto")) {
            label->setStyleSheet(QString("font-size: 18px;"
                                         "font-weight: bold;"
                                         "padding: 10px;"
                                         "color: %1;")
                                     .arg(palette.textPrimary));
        }
    }

    if (carCardsContainer) {
        // Actualizar estilos de las tarjetas de autos
        QList<QWidget*> cards = carCardsContainer->findChildren<QWidget*>();
        for (QWidget* card : cards) {
            if (card->layout() && card->layout()->count() > 0) {
                card->setStyleSheet(theme.carCardStyle());
            }
        }

        // Actualizar labels del nombre del auto en las tarjetas
        QList<QLabel*> nameLabels = carCardsContainer->findChildren<QLabel*>();
        for (QLabel* label : nameLabels) {
            if (!label->text().isEmpty() && label->text().length() < 20) {
                label->setStyleSheet(QString("font-size: 14px;"
                                             "font-weight: bold;"
                                             "color: %1;")
                                         .arg(palette.textPrimary));
            }
        }
    }

    // Actualizar labels de estadísticas en el panel derecho
    if (carNameLabel) {
        carNameLabel->setStyleSheet(QString("font-size: 20px;"
                                            "font-weight: bold;"
                                            "color: %1;")
                                        .arg(palette.textPrimary));
    }

    if (carDescLabel) {
        carDescLabel->setStyleSheet(QString("color: %1;"
                                            "margin-bottom: 10px;")
                                        .arg(palette.textSecondary));
    }

    // Actualizar labels de estadísticas (⚡, 🚀, etc.)
    QList<QLabel*> statLabels = findChildren<QLabel*>();
    for (QLabel* label : statLabels) {
        QString text = label->text();
        if (text.contains("⚡") || text.contains("🚀") || text.contains("❤️") ||
            text.contains("⚖️") || text.contains("🎮")) {
            label->setStyleSheet(QString("font-weight: bold;"
                                         "color: %1;")
                                     .arg(palette.textPrimary));
        }
    }

    // Actualizar botones
    if (confirmButton) {
        confirmButton->setStyleSheet(theme.buttonPrimaryStyle());
    }

    if (cancelButton) {
        cancelButton->setStyleSheet(theme.buttonSecondaryStyle());
    }
}
