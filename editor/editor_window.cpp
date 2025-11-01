#include "editor_window.h"
#include "checkpoint_item.h"
#include "hint_item.h"
#include "map_canvas.h"
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>
#include <QStatusBar>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

EditorWindow::EditorWindow(QWidget *parent)
    : QMainWindow(parent),
      currentTool(ItemType::None),
      nextCheckpointId(1),
      nextHintId(1),
      selectedItemId(-1),
      selectedItemType(ItemType::None),
      hasUnsavedChanges(false) {
    
    setWindowTitle("Need for Speed - Editor de Mapas");
    setMinimumSize(1200, 800);
    
    setupUI();
    
    // Inicializar track vacío
    currentTrack.name = "Nuevo Recorrido";
    currentTrack.cityMap = "city";
}

void EditorWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        if (selectedItemId >= 0) {
            removeSelectedItem();
        }
    }
    QMainWindow::keyPressEvent(event);
}

void EditorWindow::setupUI() {
    // Crear escena y vista
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, 2000, 2000);  // Tamaño del mapa
    scene->setBackgroundBrush(QBrush(QColor(34, 139, 34)));  // Verde césped
    
    graphicsView = new MapCanvas(scene, this);
    setCentralWidget(graphicsView);
    
    // Conectar señales del canvas
    connect(graphicsView, &MapCanvas::canvasClicked,
            this, &EditorWindow::onSceneClicked);
    
    // Conectar señales de selección de la escena
    connect(scene, &QGraphicsScene::selectionChanged,
            this, [this]() {
        auto selectedItems = scene->selectedItems();
        if (selectedItems.isEmpty()) {
            clearSelection();
        } else {
            // Obtener el primer item seleccionado
            QGraphicsItem* item = selectedItems.first();
            
            // Usar data() en lugar de dynamic_cast
            int itemType = item->data(ItemTypeKey).toInt();
            int itemId = item->data(ItemIdKey).toInt();
            
            if (itemType == CheckpointItemType) {
                selectedItemId = itemId;
                selectedItemType = ItemType::Checkpoint;
                updatePropertiesPanel();
            } else if (itemType == HintItemType) {
                selectedItemId = itemId;
                selectedItemType = ItemType::HintArrow;
                updatePropertiesPanel();
            }
        }
    });
    
    // Crear componentes de UI
    createMenuBar();
    createToolbar();
    createDockPanels();
    
    // Status bar
    QStatusBar* statusBar = new QStatusBar(this);
    setStatusBar(statusBar);
    
    statusLabel = new QLabel("Listo");
    toolLabel = new QLabel("Herramienta: Ninguna");
    positionLabel = new QLabel("Posición: (0, 0)");
    
    statusBar->addWidget(statusLabel, 1);
    statusBar->addPermanentWidget(toolLabel);
    statusBar->addPermanentWidget(positionLabel);
}

void EditorWindow::createMenuBar() {
    QMenuBar* menuBar = new QMenuBar(this);
    setMenuBar(menuBar);
    
    // Menú Archivo
    QMenu* fileMenu = menuBar->addMenu("&Archivo");
    
    newAction = fileMenu->addAction("&Nuevo", this, &EditorWindow::onNewTrack);
    newAction->setShortcut(QKeySequence::New);
    
    openAction = fileMenu->addAction("&Abrir...", this, &EditorWindow::onOpenTrack);
    openAction->setShortcut(QKeySequence::Open);
    
    saveAction = fileMenu->addAction("&Guardar", this, &EditorWindow::onSaveTrack);
    saveAction->setShortcut(QKeySequence::Save);
    
    saveAsAction = fileMenu->addAction("Guardar &como...", this, &EditorWindow::onSaveTrackAs);
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    
    fileMenu->addSeparator();
    
    QAction* exitAction = fileMenu->addAction("&Salir", this, &QWidget::close);
    exitAction->setShortcut(QKeySequence::Quit);
    
    // Menú Editar
    QMenu* editMenu = menuBar->addMenu("&Editar");
    editMenu->addAction("Deshacer")->setEnabled(false);  // TODO: implementar
    editMenu->addAction("Rehacer")->setEnabled(false);   // TODO: implementar
    
    // Menú Ayuda
    QMenu* helpMenu = menuBar->addMenu("A&yuda");
    helpMenu->addAction("&Acerca de", [this]() {
        QMessageBox::about(this, "Editor de Mapas",
            "Editor de Recorridos para Need for Speed\n\n"
            "Crea recorridos colocando:\n"
            "• Checkpoint de inicio (verde)\n"
            "• Checkpoints intermedios (amarillo)\n"
            "• Checkpoint de meta (rojo)\n"
            "• Flechas de ayuda (azul)\n\n"
            "Versión 1.0");
    });
}

void EditorWindow::createToolbar() {
    toolbar = new QToolBar("Herramientas", this);
    toolbar->setMovable(false);
    toolbar->setIconSize(QSize(32, 32));
    addToolBar(Qt::TopToolBarArea, toolbar);
    
    // Herramientas de archivo
    toolbar->addAction(newAction);
    toolbar->addAction(openAction);
    toolbar->addAction(saveAction);
    
    toolbar->addSeparator();
    
    // Herramientas de edición
    selectAction = toolbar->addAction("🖱️ Seleccionar", this, &EditorWindow::onSelectTool);
    selectAction->setCheckable(true);
    
    startCheckpointAction = toolbar->addAction("🟢 Inicio", this, &EditorWindow::onStartCheckpointTool);
    startCheckpointAction->setCheckable(true);
    
    checkpointAction = toolbar->addAction("🟡 Checkpoint", this, &EditorWindow::onCheckpointTool);
    checkpointAction->setCheckable(true);
    
    finishCheckpointAction = toolbar->addAction("🔴 Meta", this, &EditorWindow::onFinishCheckpointTool);
    finishCheckpointAction->setCheckable(true);
    
    hintAction = toolbar->addAction("➡️ Flecha", this, &EditorWindow::onHintTool);
    hintAction->setCheckable(true);
    
    toolbar->addSeparator();
    
    deleteAction = toolbar->addAction("🗑️ Eliminar", this, &EditorWindow::onDeleteTool);
    deleteAction->setCheckable(true);
}

void EditorWindow::createDockPanels() {
    // Panel de propiedades
    propertiesDock = new QDockWidget("Propiedades", this);
    propertiesDock->setAllowedAreas(Qt::RightDockWidgetArea);
    
    QWidget* propertiesWidget = new QWidget();
    QVBoxLayout* propsLayout = new QVBoxLayout(propertiesWidget);
    
    // Propiedades del recorrido
    QGroupBox* trackGroup = new QGroupBox("Recorrido");
    QFormLayout* trackLayout = new QFormLayout(trackGroup);
    
    cityComboBox = new QComboBox();
    cityComboBox->addItem("🏙️ City (Liberty City)", "city");
    cityComboBox->addItem("🏜️ Desert (San Andreas)", "desert");
    cityComboBox->addItem("❄️ Snow (Vice City)", "snow");
    cityComboBox->addItem("🏖️ Beach", "beach");
    cityComboBox->addItem("🌲 Forest", "forest");
    trackLayout->addRow("Ciudad:", cityComboBox);
    
    propsLayout->addWidget(trackGroup);
    
    // Propiedades del item seleccionado
    QGroupBox* itemGroup = new QGroupBox("Item Seleccionado");
    QFormLayout* itemLayout = new QFormLayout(itemGroup);
    
    rotationSpinBox = new QSpinBox();
    rotationSpinBox->setRange(0, 359);
    rotationSpinBox->setSuffix("°");
    rotationSpinBox->setEnabled(false);
    connect(rotationSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &EditorWindow::onRotationChanged);
    itemLayout->addRow("Rotación:", rotationSpinBox);
    
    widthSpinBox = new QSpinBox();
    widthSpinBox->setRange(50, 500);
    widthSpinBox->setValue(100);
    widthSpinBox->setSuffix(" px");
    widthSpinBox->setEnabled(false);
    connect(widthSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &EditorWindow::onWidthChanged);
    itemLayout->addRow("Ancho:", widthSpinBox);
    
    propsLayout->addWidget(itemGroup);
    
    propsLayout->addStretch();
    
    propertiesDock->setWidget(propertiesWidget);
    addDockWidget(Qt::RightDockWidgetArea, propertiesDock);
    
    // Panel de items
    itemsDock = new QDockWidget("Items del Recorrido", this);
    itemsDock->setAllowedAreas(Qt::LeftDockWidgetArea);
    
    QWidget* itemsWidget = new QWidget();
    QVBoxLayout* itemsLayout = new QVBoxLayout(itemsWidget);
    
    itemsListWidget = new QListWidget();
    itemsLayout->addWidget(itemsListWidget);
    
    QPushButton* clearBtn = new QPushButton("🗑️ Limpiar Todo");
    connect(clearBtn, &QPushButton::clicked, [this]() {
        if (QMessageBox::question(this, "Confirmar",
            "¿Eliminar todos los items del recorrido?") == QMessageBox::Yes) {
            
            // Limpiar items gráficos
            for (auto* item : checkpointItems.values()) {
                scene->removeItem(item);
                delete item;
            }
            for (auto* item : hintItems.values()) {
                scene->removeItem(item);
                delete item;
            }
            
            checkpointItems.clear();
            hintItems.clear();
            currentTrack.checkpoints.clear();
            currentTrack.hints.clear();
            itemsListWidget->clear();
            nextCheckpointId = 1;
            nextHintId = 1;
            hasUnsavedChanges = true;
            statusLabel->setText("Recorrido limpiado");
        }
    });
    itemsLayout->addWidget(clearBtn);
    
    itemsDock->setWidget(itemsWidget);
    addDockWidget(Qt::LeftDockWidgetArea, itemsDock);
}

// Slots de archivo
void EditorWindow::onNewTrack() {
    if (hasUnsavedChanges) {
        auto reply = QMessageBox::question(this, "Cambios sin guardar",
            "¿Guardar cambios antes de crear un nuevo recorrido?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Yes) {
            onSaveTrack();
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }
    
    currentTrack = RaceTrack();
    currentTrack.name = "Nuevo Recorrido";
    currentTrack.cityMap = "city";
    scene->clear();
    itemsListWidget->clear();
    nextCheckpointId = 1;
    nextHintId = 1;
    currentFilename.clear();
    hasUnsavedChanges = false;
    
    statusLabel->setText("Nuevo recorrido creado");
}

void EditorWindow::onOpenTrack() {
    QString filename = QFileDialog::getOpenFileName(this,
        "Abrir Recorrido", "", "Recorridos (*.yaml *.yml)");
    
    if (!filename.isEmpty()) {
        if (loadTrack(filename)) {
            currentFilename = filename;
            statusLabel->setText("Recorrido cargado: " + filename);
        }
    }
}

void EditorWindow::onSaveTrack() {
    if (currentFilename.isEmpty()) {
        onSaveTrackAs();
    } else {
        if (saveTrack(currentFilename)) {
            hasUnsavedChanges = false;
            statusLabel->setText("Recorrido guardado: " + currentFilename);
        }
    }
}

void EditorWindow::onSaveTrackAs() {
    QString filename = QFileDialog::getSaveFileName(this,
        "Guardar Recorrido", "", "Recorridos (*.yaml *.yml)");
    
    if (!filename.isEmpty()) {
        if (saveTrack(filename)) {
            currentFilename = filename;
            hasUnsavedChanges = false;
            statusLabel->setText("Recorrido guardado: " + filename);
        }
    }
}

// Slots de herramientas
void EditorWindow::onSelectTool() {
    currentTool = ItemType::None;
    selectAction->setChecked(true);
    startCheckpointAction->setChecked(false);
    checkpointAction->setChecked(false);
    finishCheckpointAction->setChecked(false);
    hintAction->setChecked(false);
    deleteAction->setChecked(false);
    toolLabel->setText("Herramienta: Seleccionar");
    graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
}

void EditorWindow::onStartCheckpointTool() {
    currentTool = ItemType::StartCheckpoint;
    selectAction->setChecked(false);
    startCheckpointAction->setChecked(true);
    checkpointAction->setChecked(false);
    finishCheckpointAction->setChecked(false);
    hintAction->setChecked(false);
    deleteAction->setChecked(false);
    toolLabel->setText("Herramienta: Checkpoint de Inicio");
    graphicsView->setDragMode(QGraphicsView::NoDrag);
}

void EditorWindow::onCheckpointTool() {
    currentTool = ItemType::Checkpoint;
    selectAction->setChecked(false);
    startCheckpointAction->setChecked(false);
    checkpointAction->setChecked(true);
    finishCheckpointAction->setChecked(false);
    hintAction->setChecked(false);
    deleteAction->setChecked(false);
    toolLabel->setText("Herramienta: Checkpoint");
    graphicsView->setDragMode(QGraphicsView::NoDrag);
}

void EditorWindow::onFinishCheckpointTool() {
    currentTool = ItemType::FinishCheckpoint;
    selectAction->setChecked(false);
    startCheckpointAction->setChecked(false);
    checkpointAction->setChecked(false);
    finishCheckpointAction->setChecked(true);
    hintAction->setChecked(false);
    deleteAction->setChecked(false);
    toolLabel->setText("Herramienta: Checkpoint de Meta");
    graphicsView->setDragMode(QGraphicsView::NoDrag);
}

void EditorWindow::onHintTool() {
    currentTool = ItemType::HintArrow;
    selectAction->setChecked(false);
    startCheckpointAction->setChecked(false);
    checkpointAction->setChecked(false);
    finishCheckpointAction->setChecked(false);
    hintAction->setChecked(true);
    deleteAction->setChecked(false);
    toolLabel->setText("Herramienta: Flecha de Ayuda");
    graphicsView->setDragMode(QGraphicsView::NoDrag);
}

void EditorWindow::onDeleteTool() {
    // Si hay un item seleccionado, eliminarlo directamente
    if (selectedItemId >= 0) {
        auto reply = QMessageBox::question(this, "Eliminar Item",
            "¿Eliminar el item seleccionado?",
            QMessageBox::Yes | QMessageBox::No);
        
        if (reply == QMessageBox::Yes) {
            removeSelectedItem();
        }
    }
    
    currentTool = ItemType::None;
    selectAction->setChecked(false);
    startCheckpointAction->setChecked(false);
    checkpointAction->setChecked(false);
    finishCheckpointAction->setChecked(false);
    hintAction->setChecked(false);
    deleteAction->setChecked(true);
    toolLabel->setText("Herramienta: Eliminar");
    graphicsView->setDragMode(QGraphicsView::NoDrag);
}

// Otros slots
void EditorWindow::onSceneClicked(QPointF position) {
    positionLabel->setText(QString("Posición: (%1, %2)")
                          .arg(position.x(), 0, 'f', 0)
                          .arg(position.y(), 0, 'f', 0));
    
    // Colocar item según la herramienta actual
    switch (currentTool) {
        case ItemType::StartCheckpoint:
            addCheckpointToScene(ItemType::StartCheckpoint, position);
            hasUnsavedChanges = true;
            break;
            
        case ItemType::Checkpoint:
            addCheckpointToScene(ItemType::Checkpoint, position);
            hasUnsavedChanges = true;
            break;
            
        case ItemType::FinishCheckpoint:
            addCheckpointToScene(ItemType::FinishCheckpoint, position);
            hasUnsavedChanges = true;
            break;
            
        case ItemType::HintArrow:
            addHintToScene(position);
            hasUnsavedChanges = true;
            break;
            
        case ItemType::None:
        default:
            // Modo selección, no hacer nada
            break;
    }
}

void EditorWindow::onItemSelected(int itemId) {
    selectedItemId = itemId;
    updatePropertiesPanel();
}

void EditorWindow::onRotationChanged(int value) {
    if (selectedItemId < 0) return;
    
    // Actualizar rotación del item seleccionado
    if (selectedItemType == ItemType::Checkpoint || 
        selectedItemType == ItemType::StartCheckpoint ||
        selectedItemType == ItemType::FinishCheckpoint) {
        
        if (checkpointItems.contains(selectedItemId)) {
            CheckpointItem* item = checkpointItems[selectedItemId];
            item->setRotationAngle(static_cast<float>(value));
            
            // Actualizar en los datos
            for (auto& cp : currentTrack.checkpoints) {
                if (cp.id == selectedItemId) {
                    cp.rotation = static_cast<float>(value);
                    break;
                }
            }
            hasUnsavedChanges = true;
        }
    } else if (selectedItemType == ItemType::HintArrow) {
        if (hintItems.contains(selectedItemId)) {
            HintItem* item = hintItems[selectedItemId];
            item->setRotationAngle(static_cast<float>(value));
            
            // Actualizar en los datos
            for (auto& hint : currentTrack.hints) {
                if (hint.id == selectedItemId) {
                    hint.rotation = static_cast<float>(value);
                    break;
                }
            }
            hasUnsavedChanges = true;
        }
    }
}

void EditorWindow::onWidthChanged(int value) {
    if (selectedItemId < 0) return;
    
    // Solo los checkpoints tienen ancho
    if (selectedItemType == ItemType::Checkpoint || 
        selectedItemType == ItemType::StartCheckpoint ||
        selectedItemType == ItemType::FinishCheckpoint) {
        
        if (checkpointItems.contains(selectedItemId)) {
            CheckpointItem* item = checkpointItems[selectedItemId];
            item->setWidth(static_cast<float>(value));
            
            // Actualizar en los datos
            for (auto& cp : currentTrack.checkpoints) {
                if (cp.id == selectedItemId) {
                    cp.width = static_cast<float>(value);
                    break;
                }
            }
            hasUnsavedChanges = true;
        }
    }
}

void EditorWindow::updatePropertiesPanel() {
    if (selectedItemId < 0) {
        rotationSpinBox->setEnabled(false);
        widthSpinBox->setEnabled(false);
        return;
    }
    
    // Habilitar controles
    rotationSpinBox->setEnabled(true);
    
    if (selectedItemType == ItemType::Checkpoint || 
        selectedItemType == ItemType::StartCheckpoint ||
        selectedItemType == ItemType::FinishCheckpoint) {
        
        if (checkpointItems.contains(selectedItemId)) {
            CheckpointItem* item = checkpointItems[selectedItemId];
            rotationSpinBox->setValue(static_cast<int>(item->getRotation()));
            widthSpinBox->setValue(static_cast<int>(item->getWidth()));
            widthSpinBox->setEnabled(true);
        }
    } else if (selectedItemType == ItemType::HintArrow) {
        if (hintItems.contains(selectedItemId)) {
            HintItem* item = hintItems[selectedItemId];
            rotationSpinBox->setValue(static_cast<int>(item->getRotation()));
            widthSpinBox->setEnabled(false);  // Las flechas no tienen ancho
        }
    }
}

void EditorWindow::updateItemsList() {
    itemsListWidget->clear();
    
    // Agregar checkpoints
    for (const auto& cp : currentTrack.checkpoints) {
        QString label;
        if (cp.isStart) {
            label = QString("🟢 START");
        } else if (cp.isFinish) {
            label = QString("🔴 FINISH");
        } else {
            label = QString("🟡 Checkpoint %1").arg(cp.id);
        }
        itemsListWidget->addItem(label);
    }
    
    // Agregar hints
    for (const auto& hint : currentTrack.hints) {
        QString label = QString("➡️ Flecha %1").arg(hint.id);
        itemsListWidget->addItem(label);
    }
}

void EditorWindow::clearSelection() {
    selectedItemId = -1;
    selectedItemType = ItemType::None;
    rotationSpinBox->setEnabled(false);
    widthSpinBox->setEnabled(false);
}

void EditorWindow::addCheckpointToScene(ItemType type, QPointF position) {
    // Determinar tipo de checkpoint
    CheckpointItem::CheckpointType cpType;
    bool isStart = false;
    bool isFinish = false;
    
    switch (type) {
        case ItemType::StartCheckpoint:
            cpType = CheckpointItem::Start;
            isStart = true;
            
            // Solo puede haber un checkpoint de inicio
            for (const auto& cp : currentTrack.checkpoints) {
                if (cp.isStart) {
                    QMessageBox::warning(this, "Checkpoint de Inicio",
                        "Ya existe un checkpoint de inicio. Elimínalo primero.");
                    return;
                }
            }
            break;
            
        case ItemType::FinishCheckpoint:
            cpType = CheckpointItem::Finish;
            isFinish = true;
            
            // Solo puede haber un checkpoint de meta
            for (const auto& cp : currentTrack.checkpoints) {
                if (cp.isFinish) {
                    QMessageBox::warning(this, "Checkpoint de Meta",
                        "Ya existe un checkpoint de meta. Elimínalo primero.");
                    return;
                }
            }
            break;
            
        case ItemType::Checkpoint:
        default:
            cpType = CheckpointItem::Normal;
            break;
    }
    
    // Crear item gráfico
    int id = nextCheckpointId++;
    CheckpointItem* item = new CheckpointItem(id, cpType, position, 0.0f, 100.0f);
    scene->addItem(item);
    checkpointItems[id] = item;
    
    // Agregar a los datos
    CheckpointData data;
    data.id = id;
    data.position = position;
    data.rotation = 0.0f;
    data.width = 100.0f;
    data.isStart = isStart;
    data.isFinish = isFinish;
    currentTrack.checkpoints.append(data);
    
    // Actualizar lista de items
    updateItemsList();
    
    QString msg;
    if (isStart) msg = "Checkpoint de INICIO colocado";
    else if (isFinish) msg = "Checkpoint de META colocado";
    else msg = QString("Checkpoint %1 colocado").arg(id);
    statusLabel->setText(msg);
}

void EditorWindow::addHintToScene(QPointF position) {
    // Crear item gráfico
    int id = nextHintId++;
    HintItem* item = new HintItem(id, position, 0.0f, -1);
    scene->addItem(item);
    hintItems[id] = item;
    
    // Agregar a los datos
    HintData data;
    data.id = id;
    data.position = position;
    data.rotation = 0.0f;
    data.targetCheckpoint = -1;  // Sin target por ahora
    currentTrack.hints.append(data);
    
    // Actualizar lista de items
    updateItemsList();
    
    statusLabel->setText(QString("Flecha %1 colocada").arg(id));
}

void EditorWindow::removeSelectedItem() {
    if (selectedItemId < 0) return;
    
    if (selectedItemType == ItemType::Checkpoint || 
        selectedItemType == ItemType::StartCheckpoint ||
        selectedItemType == ItemType::FinishCheckpoint) {
        
        if (checkpointItems.contains(selectedItemId)) {
            // Eliminar item gráfico
            CheckpointItem* item = checkpointItems[selectedItemId];
            scene->removeItem(item);
            delete item;
            checkpointItems.remove(selectedItemId);
            
            // Eliminar de los datos
            for (int i = 0; i < currentTrack.checkpoints.size(); ++i) {
                if (currentTrack.checkpoints[i].id == selectedItemId) {
                    currentTrack.checkpoints.removeAt(i);
                    break;
                }
            }
            
            statusLabel->setText("Checkpoint eliminado");
            hasUnsavedChanges = true;
        }
    } else if (selectedItemType == ItemType::HintArrow) {
        if (hintItems.contains(selectedItemId)) {
            // Eliminar item gráfico
            HintItem* item = hintItems[selectedItemId];
            scene->removeItem(item);
            delete item;
            hintItems.remove(selectedItemId);
            
            // Eliminar de los datos
            for (int i = 0; i < currentTrack.hints.size(); ++i) {
                if (currentTrack.hints[i].id == selectedItemId) {
                    currentTrack.hints.removeAt(i);
                    break;
                }
            }
            
            statusLabel->setText("Flecha eliminada");
            hasUnsavedChanges = true;
        }
    }
    
    updateItemsList();
    clearSelection();
}

bool EditorWindow::loadTrack(const QString& filename) {
    // TODO: Implementar carga desde YAML
    Q_UNUSED(filename);
    QMessageBox::information(this, "En desarrollo",
        "La carga de recorridos se implementará próximamente.");
    return false;
}

bool EditorWindow::saveTrack(const QString& filename) {
    // TODO: Implementar guardado a YAML
    Q_UNUSED(filename);
    QMessageBox::information(this, "En desarrollo",
        "El guardado de recorridos se implementará próximamente.\n\n"
        "Formato YAML con:\n"
        "- Nombre del recorrido\n"
        "- Ciudad\n"
        "- Lista de checkpoints (posición, rotación, ancho)\n"
        "- Lista de hints (posición, rotación, checkpoint destino)");
    return true;
}
