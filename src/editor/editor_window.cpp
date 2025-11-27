#include "editor/editor_window.h"

#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>
#include <QTimer>
#include <QVBoxLayout>

#include "editor/yaml_handler.h"
// Asegúrate de incluir esto si no lo tienes indirectamente
#include "editor/sensor_item.h"

EditorWindow::EditorWindow(EditorMode mode, QWidget* parent)
    : QMainWindow(parent),
      editorMode(mode),
      mapCanvas(new MapCanvas(this)),
      itemList(new QListWidget(this)),
      yamlHandler(new YamlHandler(this)),
      currentMapFile(""),
      buildingStatusLabel(new QLabel("", this)),
      mousePosLabel(new QLabel(this)),
      selectedItemLabel(new QLabel(this)) {
    setupUI();
    setupMenuBar();
    setupToolbar();
    setupDocks();

    // Conectar señales
    connect(mapCanvas, &MapCanvas::itemPlaced, this,
            &EditorWindow::updateItemList);
    connect(mapCanvas, &MapCanvas::selectionChanged, this,
            &EditorWindow::updateItemList);
    connect(mapCanvas, &MapCanvas::buildingVertexAdded, this,
            &EditorWindow::updateBuildingStatus);

    // --- IMPORTANTE: Usar la señal para actualizar UI, NO llamar lógica
    // repetida
    connect(mapCanvas, &MapCanvas::buildingFinished, this,
            &EditorWindow::onFinishBuilding);

    connect(mapCanvas, &MapCanvas::mousePositionChanged, this,
            &EditorWindow::updateStatusBar);
    connect(mapCanvas, &MapCanvas::selectionChanged, this,
            &EditorWindow::updateSelectionInfo);
    connect(mapCanvas, &MapCanvas::itemPlaced, this,
            &EditorWindow::updateSelectionInfo);

    statusBar()->addPermanentWidget(mousePosLabel);
    statusBar()->addPermanentWidget(selectedItemLabel);
    mousePosLabel->setMinimumWidth(200);
    selectedItemLabel->setMinimumWidth(300);

    updateSelectionInfo();

    if (editorMode == EditRaceMode) {
        setWindowTitle("Need for Speed - Editor de Carreras");
        QTimer::singleShot(100, this, &EditorWindow::onLoadProject);
    } else {
        setWindowTitle("Need for Speed - Editor de Mapas");
    }
}

EditorWindow::~EditorWindow() = default;

void EditorWindow::setupUI() {
    setCentralWidget(mapCanvas);
    resize(1200, 800);
}

void EditorWindow::setupToolbar() {
    QToolBar* toolbar = addToolBar("Tools");
    toolbar->setMovable(false);

    // Checkpoints
    startAction = toolbar->addAction("🏁 Start");
    connect(startAction, &QAction::triggered, this,
            &EditorWindow::onPlaceStart);

    checkpointAction = toolbar->addAction("⚑ Checkpoint");
    connect(checkpointAction, &QAction::triggered, this,
            &EditorWindow::onPlaceCheckpoint);

    finishAction = toolbar->addAction("🏆 Finish");
    connect(finishAction, &QAction::triggered, this,
            &EditorWindow::onPlaceFinish);

    toolbar->addSeparator();

    if (editorMode == CreateMapMode) {
        buildingAction = toolbar->addAction("🏢 New Building");
        buildingAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
        connect(buildingAction, &QAction::triggered, this,
                &EditorWindow::onStartPlacingBuilding);

        finishBuildingAction = toolbar->addAction("✅ Finish Building");
        finishBuildingAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
        finishBuildingAction->setEnabled(false);

        // --- CORRECCIÓN CRÍTICA: Conectar directo al canvas para evitar loop
        // ---
        connect(finishBuildingAction, &QAction::triggered, mapCanvas,
                &MapCanvas::finishCurrentBuilding);

        cancelBuildingAction = toolbar->addAction("❌ Cancel Building");
        cancelBuildingAction->setEnabled(false);
        connect(cancelBuildingAction, &QAction::triggered, this,
                &EditorWindow::onCancelBuilding);

        undoVertexAction = toolbar->addAction("↶ Undo Vertex");
        undoVertexAction->setEnabled(false);
        connect(undoVertexAction, &QAction::triggered, this,
                &EditorWindow::onRemoveLastVertex);

        toolbar->addSeparator();
        toolbar->addWidget(buildingStatusLabel);
        toolbar->addSeparator();

        // Buildings types
        QAction* bridgeAction = toolbar->addAction("🌉 Bridge");
        connect(bridgeAction, &QAction::triggered, [this]() {
            mapCanvas->startPlacingBuilding("bridge");
            finishBuildingAction->setEnabled(true);
            cancelBuildingAction->setEnabled(true);
            undoVertexAction->setEnabled(true);
        });

        QAction* railingAction = toolbar->addAction("🚧 Railing");
        connect(railingAction, &QAction::triggered, [this]() {
            mapCanvas->startPlacingBuilding("railing");
            finishBuildingAction->setEnabled(true);
            cancelBuildingAction->setEnabled(true);
            undoVertexAction->setEnabled(true);
        });
    }

    toolbar->addSeparator();

    // SENSORS
    QAction* upperSensorAction = toolbar->addAction("⬆️ Upper Sensor");
    connect(upperSensorAction, &QAction::triggered,
            [this]() { mapCanvas->startPlacingSensor(SensorItem::Upper); });

    QAction* lowerSensorAction = toolbar->addAction("⬇️ Lower Sensor");
    connect(lowerSensorAction, &QAction::triggered,
            [this]() { mapCanvas->startPlacingSensor(SensorItem::Lower); });
}

void EditorWindow::setupMenuBar() {
    QMenu* fileMenu = menuBar()->addMenu("&File");

    QAction* newAction = fileMenu->addAction("&New Map");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &EditorWindow::onNewMap);

    if (editorMode == EditRaceMode) {
        QAction* loadProjectAction = fileMenu->addAction("&Open Race (YAML)");
        loadProjectAction->setShortcut(QKeySequence::Open);
        connect(loadProjectAction, &QAction::triggered, this,
                &EditorWindow::onLoadProject);
    } else {
        QAction* loadImageAction =
            fileMenu->addAction("Load &Background Image");
        connect(loadImageAction, &QAction::triggered, this,
                &EditorWindow::onLoadBackgroundImage);

        QAction* loadProjectAction =
            fileMenu->addAction("&Open Project (YAML)");
        loadProjectAction->setShortcut(QKeySequence::Open);
        connect(loadProjectAction, &QAction::triggered, this,
                &EditorWindow::onLoadProject);
    }

    QAction* saveAction = fileMenu->addAction("&Save Map");
    saveAction->setShortcut(QKeySequence::Save);
    connect(saveAction, &QAction::triggered, this, &EditorWindow::onSaveMap);

    fileMenu->addSeparator();

    QAction* exitAction = fileMenu->addAction("E&xit");
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
}

void EditorWindow::setupDocks() {
    QDockWidget* listDock = new QDockWidget("Items", this);
    listDock->setWidget(itemList);
    addDockWidget(Qt::RightDockWidgetArea, listDock);
}

// ... (onPlaceStart, onPlaceCheckpoint, onPlaceFinish sin cambios) ...
void EditorWindow::onPlaceStart() {
    mapCanvas->placeCheckpoint(CheckpointItem::Start);
    statusBar()->showMessage("Click to place start checkpoint", 3000);
}
void EditorWindow::onPlaceCheckpoint() {
    mapCanvas->placeCheckpoint(CheckpointItem::Normal);
    statusBar()->showMessage("Click to place intermediate checkpoint", 3000);
}
void EditorWindow::onPlaceFinish() {
    mapCanvas->placeCheckpoint(CheckpointItem::Finish);
    statusBar()->showMessage("Click to place finish checkpoint", 3000);
}

void EditorWindow::onStartPlacingBuilding() {
    if (!mapCanvas) return;
    mapCanvas->startPlacingBuilding(QStringLiteral("generic"));
    if (finishBuildingAction) finishBuildingAction->setEnabled(true);
    if (cancelBuildingAction) cancelBuildingAction->setEnabled(true);
    if (undoVertexAction) undoVertexAction->setEnabled(true);
    buildingStatusLabel->setText(tr("Placing building: generic"));
}

void EditorWindow::onFinishBuilding() {
    // IMPORTANTE: NO LLAMAR A mapCanvas->finishCurrentBuilding() AQUÍ
    // Porque esta función se llama como respuesta a que YA terminó.

    buildingStatusLabel->setText("");
    if (finishBuildingAction) finishBuildingAction->setEnabled(false);
    if (cancelBuildingAction) cancelBuildingAction->setEnabled(false);
    if (undoVertexAction) undoVertexAction->setEnabled(false);
    statusBar()->showMessage("Building finished", 3000);
}

void EditorWindow::onCancelBuilding() {
    mapCanvas->cancelCurrentBuilding();
    buildingStatusLabel->setText("");
    finishBuildingAction->setEnabled(false);
    cancelBuildingAction->setEnabled(false);
    undoVertexAction->setEnabled(false);
    statusBar()->showMessage("Building cancelled", 3000);
}

void EditorWindow::onRemoveLastVertex() {
    mapCanvas->removeLastBuildingVertex();
}

void EditorWindow::updateBuildingStatus(int vertexCount) {
    buildingStatusLabel->setText(
        QString("Building: %1 vertices").arg(vertexCount));
    if (vertexCount >= 3)
        finishBuildingAction->setEnabled(true);
    else
        finishBuildingAction->setEnabled(false);
}

void EditorWindow::onSaveMap() {
    QString fileName;
    if (currentMapFile.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(
            this, tr("Save Map"), "../maps", tr("YAML Files (*.yaml *.yml)"));
        if (fileName.isEmpty()) return;
    } else {
        fileName = currentMapFile;
    }

    if (mapCanvas && yamlHandler) {
        // Guardar pasando los sensores también
        yamlHandler->saveMap(fileName, mapCanvas->getCheckpoints(),
                             mapCanvas->getBuildings(),
                             mapCanvas->getSensors());  // <--- Agregado
        currentMapFile = fileName;
        statusBar()->showMessage(tr("Map saved: %1").arg(fileName));
    }
}

void EditorWindow::onLoadBackgroundImage() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Load Image"), "../maps", tr("Image Files (*.png *.jpg)"));
    if (fileName.isEmpty()) return;
    if (mapCanvas) {
        mapCanvas->loadBackgroundImage(fileName);
        currentMapFile = "";
        statusBar()->showMessage(tr("Background loaded: %1").arg(fileName));
    }
}

void EditorWindow::onLoadProject() {
    QString yamlFileName = QFileDialog::getOpenFileName(
        this, tr("Open Map"), "../maps", tr("YAML Files (*.yaml *.yml)"));
    if (yamlFileName.isEmpty()) return;

    QString backgroundPath;
    QList<CheckpointItem*> checkpoints;
    QList<BuildingItem*> buildings;
    QList<SensorItem*> sensors;  // <--- 1. Declarar lista de sensores

    // --- 2. Pasar sensores como 5to argumento ---
    if (!yamlHandler->loadMap(yamlFileName, backgroundPath, checkpoints,
                              buildings, sensors)) {
        QMessageBox::warning(this, "Error", "Failed to load map.");
        return;
    }

    mapCanvas->clearAll();

    if (!backgroundPath.isEmpty()) {
        QFileInfo yamlFileInfo(yamlFileName);
        QString absoluteBackgroundPath =
            yamlFileInfo.dir().absoluteFilePath(backgroundPath);
        if (QFile::exists(absoluteBackgroundPath)) {
            mapCanvas->loadBackgroundImage(absoluteBackgroundPath);
        } else {
            // Fallback manual
            QString imgFileName = QFileDialog::getOpenFileName(
                this, tr("Select Background"),
                yamlFileInfo.dir().absolutePath(), tr("Images (*.png *.jpg)"));
            if (!imgFileName.isEmpty())
                mapCanvas->loadBackgroundImage(imgFileName);
        }
    }

    for (CheckpointItem* cp : checkpoints) mapCanvas->addItemToScene(cp);
    for (BuildingItem* b : buildings) mapCanvas->addItemToScene(b);

    // --- 3. Agregar sensores al mapa ---
    for (SensorItem* s : sensors) mapCanvas->addItemToScene(s);

    currentMapFile = yamlFileName;
    updateItemList();
    updateSelectionInfo();
    statusBar()->showMessage(tr("Project loaded: %1").arg(yamlFileName));
}

void EditorWindow::onNewMap() {
    if (QMessageBox::Yes ==
        QMessageBox::question(this, "New Map", "Unsaved changes will be lost.",
                              QMessageBox::Yes | QMessageBox::No)) {
        mapCanvas->clearAll();
        updateItemList();
        statusBar()->showMessage("New map created", 3000);
    }
}

void EditorWindow::updateItemList() {
    itemList->clear();
    itemList->addItem(
        QString("Checkpoints: %1").arg(mapCanvas->getCheckpoints().size()));
    itemList->addItem(
        QString("Buildings: %1").arg(mapCanvas->getBuildings().size()));
    itemList->addItem(
        QString("Sensors: %1").arg(mapCanvas->getSensors().size()));
}

void EditorWindow::updateStatusBar(const QPointF& pos) {
    mousePosLabel->setText(QString("Mouse: (%1, %2)")
                               .arg(pos.x(), 0, 'f', 1)
                               .arg(pos.y(), 0, 'f', 1));
}

void EditorWindow::updateSelectionInfo() {
    QList<QGraphicsItem*> selected = mapCanvas->getScene()->selectedItems();
    if (!selected.isEmpty()) {
        QGraphicsItem* item = selected.first();
        if (dynamic_cast<CheckpointItem*>(item))
            selectedItemLabel->setText("Selected: Checkpoint");
        else if (dynamic_cast<BuildingItem*>(item))
            selectedItemLabel->setText("Selected: Building");
        else if (dynamic_cast<SensorItem*>(item))
            selectedItemLabel->setText("Selected: Sensor");
    } else {
        selectedItemLabel->setText("");
    }
}
