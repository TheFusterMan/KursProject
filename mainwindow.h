#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// Forward declarations to avoid including full headers
class QTabWidget;
class QTableWidget;
class QToolBar;
class QAction;
class QMenu;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    // Slots for actions
    void onAddRecord();
    void onDeleteRecord();
    void onLoad();
    void onSave();
    void onDebug();
    void onFind();
    void onAbout();

private:
    // UI setup methods
    void setupUi();
    void createActions();
    void createMenus();

    void updateClientsTable();

    // Widget pointers
    QTabWidget* mainTabWidget;
    QTableWidget* sellersTable; // Table for Sellers
    QTableWidget* salesTable;   // Table for Sales

    // Menus and panels
    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* helpMenu;

    // Actions
    QAction* displayAction;
    QAction* addAction;
    QAction* deleteAction;
    QAction* loadAction;
    QAction* saveAction;
    QAction* debugAction;
    QAction* findAction;
    QAction* exitAction;
    QAction* aboutAction;
}; // <-- The crucial semicolon

#endif // MAINWINDOW_H