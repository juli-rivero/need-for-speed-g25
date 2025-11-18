#ifndef EDITOR_WINDOW_H
#define EDITOR_WINDOW_H

#include <QDockWidget>
#include <QLabel>
#include <QListWidget>
#include <QMainWindow>
#include <QToolBar>

#include "map_canvas.h"
#include "yaml_handler.h"

class EditorWindow : public QMainWindow {
  Q_OBJECT

 public:
  enum EditorMode { EditRaceMode, CreateMapMode };

  explicit EditorWindow(EditorMode mode, QWidget* parent = nullptr);
  ~EditorWindow();

 private:
  void setupUI();
  void setupMenuBar();
  void setupToolbar();
  void setupDocks();
  void updateStatusBar(const QPointF& pos);
  void updateSelectionInfo();

 private slots:
  // File operations
  void onNewMap();
  void onSaveMap();
  void onLoadBackgroundImage();
  void onLoadProject();

  // Item placement
  void onPlaceStart();
  void onPlaceCheckpoint();
  void onPlaceFinish();
  void onPlaceHint();
  void onStartPlacingBuilding();
  void onFinishBuilding();
  void onCancelBuilding();
  void onRemoveLastVertex();

  void updateBuildingStatus(int vertexCount);
  void updateItemList();

 private:
  EditorMode editorMode;
  MapCanvas* mapCanvas;
  QListWidget* itemList;
  YamlHandler* yamlHandler;
  QString currentMapFile;

  // Toolbar actions (solo las que realmente se usan)
  QAction* startAction;
  QAction* checkpointAction;
  QAction* finishAction;
  QAction* hintAction;
  QAction* buildingAction;
  QAction* finishBuildingAction;
  QAction* cancelBuildingAction;
  QAction* undoVertexAction;

  // Status bar labels
  QLabel* buildingStatusLabel;
  QLabel* mousePosLabel;
  QLabel* selectedItemLabel;
};

#endif  // EDITOR_WINDOW_H
