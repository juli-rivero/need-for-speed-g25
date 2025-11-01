#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QToolBar>
#include <QAction>
#include <QLabel>
#include <QDockWidget>
#include <QListWidget>
#include <QSpinBox>
#include <QComboBox>
#include <QString>
#include <QVector>
#include <QPointF>
#include <QMap>

// Forward declarations
class CheckpointItem;
class HintItem;
class MapCanvas;

// Tipos de items que se pueden colocar en el mapa
enum class ItemType {
    None,
    StartCheckpoint,
    Checkpoint,
    FinishCheckpoint,
    HintArrow
};

// Estructura para representar un checkpoint
struct CheckpointData {
    int id;
    QPointF position;
    float rotation;  // Ángulo en grados
    float width;     // Ancho de la franja
    bool isStart;
    bool isFinish;
};

// Estructura para representar un hint/flecha
struct HintData {
    int id;
    QPointF position;
    float rotation;  // Dirección de la flecha
    int targetCheckpoint;  // ID del checkpoint al que apunta
};

// Estructura para el recorrido completo
struct RaceTrack {
    QString name;
    QString cityMap;  // "city", "desert", "snow", "beach", "forest"
    QVector<CheckpointData> checkpoints;
    QVector<HintData> hints;
};

/**
 * Ventana principal del editor de mapas.
 * Permite crear recorridos colocando checkpoints y hints sobre las ciudades.
 */
class EditorWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit EditorWindow(QWidget *parent = nullptr);
    ~EditorWindow() override = default;

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    // Slots para las acciones de la toolbar
    void onNewTrack();
    void onOpenTrack();
    void onSaveTrack();
    void onSaveTrackAs();
    
    // Slots para las herramientas
    void onSelectTool();
    void onStartCheckpointTool();
    void onCheckpointTool();
    void onFinishCheckpointTool();
    void onHintTool();
    void onDeleteTool();
    
    // Slots para el canvas
    void onSceneClicked(QPointF position);
    void onItemSelected(int itemId);
    
    // Slots para propiedades
    void onRotationChanged(int value);
    void onWidthChanged(int value);

private:
    void setupUI();
    void createToolbar();
    void createDockPanels();
    void createMenuBar();
    void updatePropertiesPanel();
    void updateItemsList();
    void clearSelection();
    void addCheckpointToScene(ItemType type, QPointF position);
    void addHintToScene(QPointF position);
    void removeSelectedItem();
    
    // Gestión de archivo
    bool loadTrack(const QString& filename);
    bool saveTrack(const QString& filename);
    
    // Canvas y escena
    MapCanvas* graphicsView;
    QGraphicsScene* scene;
    
    // Mapeo de items gráficos
    QMap<int, CheckpointItem*> checkpointItems;  // id -> item
    QMap<int, HintItem*> hintItems;              // id -> item
    
    // Estado del editor
    ItemType currentTool;
    RaceTrack currentTrack;
    int nextCheckpointId;
    int nextHintId;
    int selectedItemId;
    ItemType selectedItemType;
    QString currentFilename;
    bool hasUnsavedChanges;
    
    // Toolbar y acciones
    QToolBar* toolbar;
    QAction* newAction;
    QAction* openAction;
    QAction* saveAction;
    QAction* saveAsAction;
    
    QAction* selectAction;
    QAction* startCheckpointAction;
    QAction* checkpointAction;
    QAction* finishCheckpointAction;
    QAction* hintAction;
    QAction* deleteAction;
    
    // Paneles laterales
    QDockWidget* toolsDock;
    QDockWidget* propertiesDock;
    QDockWidget* itemsDock;
    
    // Widgets de propiedades
    QSpinBox* rotationSpinBox;
    QSpinBox* widthSpinBox;
    QComboBox* cityComboBox;
    
    // Lista de items
    QListWidget* itemsListWidget;
    
    // Status bar
    QLabel* statusLabel;
    QLabel* toolLabel;
    QLabel* positionLabel;
};

#endif // EDITOR_WINDOW_H
