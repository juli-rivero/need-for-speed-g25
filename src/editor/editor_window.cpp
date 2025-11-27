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
        // Abrir automáticamente el diálogo para cargar un mapa existente
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

    // Checkpoints (disponibles en ambos modos)
    startAction = toolbar->addAction("🏁 Start");
    startAction->setToolTip("Place Start Checkpoint");
    connect(startAction, &QAction::triggered, this,
            &EditorWindow::onPlaceStart);

    checkpointAction = toolbar->addAction("⚑ Checkpoint");
    checkpointAction->setToolTip("Place Intermediate Checkpoint");
    connect(checkpointAction, &QAction::triggered, this,
            &EditorWindow::onPlaceCheckpoint);

    finishAction = toolbar->addAction("🏆 Finish");
    finishAction->setToolTip("Place Finish Checkpoint");
    connect(finishAction, &QAction::triggered, this,
            &EditorWindow::onPlaceFinish);

    toolbar->addSeparator();

    //  Herramientas de edificios (solo en modo CreateMap)
    if (editorMode == CreateMapMode) {
        buildingAction = toolbar->addAction("🏢 New Building");
        buildingAction->setToolTip("Start Building Placement (Ctrl+B)");
        buildingAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_B));
        connect(buildingAction, &QAction::triggered, this,
                &EditorWindow::onStartPlacingBuilding);

        finishBuildingAction = toolbar->addAction("✅ Finish Building");
        finishBuildingAction->setToolTip("Finish Current Building (Ctrl+F)");
        finishBuildingAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_F));
        finishBuildingAction->setEnabled(false);
        connect(finishBuildingAction, &QAction::triggered, this,
                &EditorWindow::onFinishBuilding);

        cancelBuildingAction = toolbar->addAction("❌ Cancel Building");
        cancelBuildingAction->setToolTip("Cancel Current Building (Esc)");
        cancelBuildingAction->setEnabled(false);
        connect(cancelBuildingAction, &QAction::triggered, this,
                &EditorWindow::onCancelBuilding);

        undoVertexAction = toolbar->addAction("↶ Undo Vertex");
        undoVertexAction->setToolTip("Remove Last Vertex (Ctrl+Z)");
        undoVertexAction->setEnabled(false);
        connect(undoVertexAction, &QAction::triggered, this,
                &EditorWindow::onRemoveLastVertex);

        toolbar->addSeparator();
        toolbar->addWidget(buildingStatusLabel);
    }
}

void EditorWindow::setupMenuBar() {
    QMenu* fileMenu = menuBar()->addMenu("&File");

    QAction* newAction = fileMenu->addAction("&New Map");
    newAction->setShortcut(QKeySequence::New);
    connect(newAction, &QAction::triggered, this, &EditorWindow::onNewMap);

    // Diferentes opciones según el modo
    if (editorMode == EditRaceMode) {
        // Solo permitir cargar proyectos YAML existentes
        QAction* loadProjectAction = fileMenu->addAction("&Open Race (YAML)");
        loadProjectAction->setShortcut(QKeySequence::Open);
        connect(loadProjectAction, &QAction::triggered, this,
                &EditorWindow::onLoadProject);
    } else {
        // Modo CreateMap: permitir cargar imagen o proyecto
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
    // Dock con lista de items
    QDockWidget* listDock = new QDockWidget("Items", this);
    listDock->setWidget(itemList);
    addDockWidget(Qt::RightDockWidgetArea, listDock);

    connect(itemList, &QListWidget::itemDoubleClicked,
            [](QListWidgetItem* item) {
                Q_UNUSED(item);
                // Aquí podrías centrar la vista en el item seleccionado
            });
}

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
    mapCanvas->startPlacingBuilding();
    finishBuildingAction->setEnabled(true);
    cancelBuildingAction->setEnabled(true);
    undoVertexAction->setEnabled(true);
    buildingStatusLabel->setText("Placing building: 0 vertices");
    statusBar()->showMessage(
        "Click to add vertices. Press Enter to finish, Esc to cancel.", 5000);
}

void EditorWindow::onFinishBuilding() {
    mapCanvas->finishCurrentBuilding();
    buildingStatusLabel->setText("");
    finishBuildingAction->setEnabled(false);
    cancelBuildingAction->setEnabled(false);
    undoVertexAction->setEnabled(false);
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

    if (vertexCount >= 3) {
        finishBuildingAction->setEnabled(true);
    } else {
        finishBuildingAction->setEnabled(false);
    }
}

void EditorWindow::onSaveMap() {
    QString fileName;

    if (currentMapFile.isEmpty()) {
        // Si no hay archivo previo, pedir nombre nuevo
        fileName = QFileDialog::getSaveFileName(
            this, tr("Save Map Configuration"), "../maps",
            tr("YAML Files (*.yaml *.yml)"));

        if (fileName.isEmpty()) {
            return;
        }
    } else {
        fileName = currentMapFile;
    }

    // Guardar el YAML con la configuración actual
    if (mapCanvas && yamlHandler) {
        yamlHandler->saveMap(fileName, mapCanvas->getCheckpoints(),
                             mapCanvas->getBuildings());
        currentMapFile = fileName;
        statusBar()->showMessage(tr("Map saved: %1").arg(fileName));
    }
}

void EditorWindow::onLoadBackgroundImage() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Load Background Image"), "../maps",
        tr("Image Files (*.png *.jpg *.bmp)"));

    if (fileName.isEmpty()) {
        return;
    }

    if (mapCanvas) {
        mapCanvas->loadBackgroundImage(fileName);
        currentMapFile = "";  // No hay archivo YAML todavía
        statusBar()->showMessage(
            tr("Background image loaded: %1").arg(fileName));
    }
}

void EditorWindow::onLoadProject() {
    QString yamlFileName =
        QFileDialog::getOpenFileName(this, tr("Open Map Project"), "../maps",
                                     tr("YAML Files (*.yaml *.yml)"));

    if (yamlFileName.isEmpty()) {
        return;
    }

    // Cargar el YAML
    QString backgroundPath;
    QList<CheckpointItem*> checkpoints;
    QList<BuildingItem*> buildings;

    if (!yamlHandler->loadMap(yamlFileName, backgroundPath, checkpoints,
                              buildings)) {
        QMessageBox::warning(this, "Error",
                             "Failed to load map from YAML file.");
        return;
    }

    // Limpiar el canvas actual
    mapCanvas->clearAll();

    // Cargar la imagen de fondo si está especificada en el YAML
    if (!backgroundPath.isEmpty()) {
        // Resolver la ruta relativa desde el YAML
        QFileInfo yamlFileInfo(yamlFileName);
        QString absoluteBackgroundPath =
            yamlFileInfo.dir().absoluteFilePath(backgroundPath);

        if (QFile::exists(absoluteBackgroundPath)) {
            mapCanvas->loadBackgroundImage(absoluteBackgroundPath);
        } else {
            // Si no encuentra la imagen, preguntar al usuario
            QString imgFileName = QFileDialog::getOpenFileName(
                this, tr("Select Background Image for this map"),
                yamlFileInfo.dir().absolutePath(),
                tr("Image Files (*.png *.jpg *.bmp)"));

            if (!imgFileName.isEmpty()) {
                mapCanvas->loadBackgroundImage(imgFileName);
            }
        }
    }

    // Agregar todos los items al canvas
    for (CheckpointItem* cp : checkpoints) {
        mapCanvas->addItemToScene(cp);
    }
    for (BuildingItem* building : buildings) {
        mapCanvas->addItemToScene(building);
    }

    currentMapFile = yamlFileName;
    updateItemList();
    updateSelectionInfo();
    statusBar()->showMessage(tr("Project loaded: %1").arg(yamlFileName));
}

void EditorWindow::onNewMap() {
    auto reply = QMessageBox::question(this, "New Map",
                                       "Are you sure you want to create a new "
                                       "map? Unsaved changes will be lost.",
                                       QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        mapCanvas->clearAll();
        updateItemList();
        statusBar()->showMessage("New map created", 3000);
    }
}

void EditorWindow::updateItemList() {
    itemList->clear();

    int checkpointCount = mapCanvas->getCheckpoints().size();
    int buildingCount = mapCanvas->getBuildings().size();

    itemList->addItem(QString("Checkpoints: %1").arg(checkpointCount));
    itemList->addItem(QString("Buildings: %1").arg(buildingCount));
}

void EditorWindow::updateStatusBar(const QPointF& pos) {
    mousePosLabel->setText(QString("Mouse: (%1, %2)")
                               .arg(pos.x(), 0, 'f', 1)
                               .arg(pos.y(), 0, 'f', 1));
}

void EditorWindow::updateSelectionInfo() {
    QList<CheckpointItem*> checkpoints = mapCanvas->getCheckpoints();
    QList<BuildingItem*> buildings = mapCanvas->getBuildings();

    QString info = QString("Items: %1 checkpoints, %3 buildings")
                       .arg(checkpoints.size())
                       .arg(buildings.size());

    // Mostrar info del item seleccionado si hay uno
    QList<QGraphicsItem*> selected = mapCanvas->getScene()->selectedItems();
    if (!selected.isEmpty()) {
        QGraphicsItem* item = selected.first();
        QPointF pos = item->pos();

        if (CheckpointItem* cp = dynamic_cast<CheckpointItem*>(item)) {
            QString typeStr;
            switch (cp->getType()) {
                case CheckpointItem::Start:
                    typeStr = "Start";
                    break;
                case CheckpointItem::Finish:
                    typeStr = "Finish";
                    break;
                case CheckpointItem::Normal:
                    typeStr = "Normal";
                    break;
            }
            info += QString(" | Selected: Checkpoint %1 at (%2, %3)")
                        .arg(typeStr)
                        .arg(pos.x(), 0, 'f', 1)
                        .arg(pos.y(), 0, 'f', 1);
        } else if (BuildingItem* building = dynamic_cast<BuildingItem*>(item)) {
            int vertices = building->getVertices().size();
            info +=
                QString(" | Selected: Building with %1 vertices").arg(vertices);
        }
    }

    selectedItemLabel->setText(info);
}
