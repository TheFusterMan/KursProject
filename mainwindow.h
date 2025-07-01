#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void setupUi();
    void createActions();
    void createMenus();

    void updateClientsTable();
    void updateConsultationsTable();

    QTabWidget* mainTabWidget;
    QTableWidget* sellersTable;
    QTableWidget* salesTable;

    QMenu* fileMenu;
    QMenu* editMenu;
    QMenu* helpMenu;

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
};

#endif // MAINWINDOW_H