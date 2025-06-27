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
    void onAddClientRecord();
    void onDeleteClientRecord();
    void onLoadClients();
    void onSaveClients();
    void onDebug();
    void onFind();
    void onAbout();

    void showClientContextMenu(const QPoint& pos);
    void showConsultationContextMenu(const QPoint& pos);

    void onAddConsultationRecord();
    void onDeleteConsultationRecord();
    void onLoadConsultations();
    void onSaveConsultations();
    void onFindConsultations();

    void onGenerateReport();
private:
    // UI setup methods
    void setupUi();
    void createActions();
    void createMenus();

    void updateClientsTable();
    void updateConsultationsTable();

    // Widget pointers
    QTabWidget* mainTabWidget;
    QTableWidget* sellersTable; // Table for Sellers
    QTableWidget* salesTable;   // Table for Sales

    // Menus and panels
    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* helpMenu;

    // Actions
    QAction* addAction;
    QAction* deleteAction;
    QAction* loadClientsAction;
    QAction* saveClientsAction;
    QAction* debugAction;
    QAction* findAction;
    QAction* exitAction;
    QAction* aboutAction;
    QAction* reportAction;

    QAction* addConsultationAction;
    QAction* deleteConsultationAction;
    QAction* loadConsultationsAction;
    QAction* saveConsultationsAction;
    QAction* findConsultationsAction;
}; // <-- The crucial semicolon

#endif // MAINWINDOW_H