#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "AddClientDialog.h"
#include "AddConsultationDialog.h"
#include "DeleteClientDialog.h"
#include "DeleteConsultationDialog.h"
#include "ReportDialog.h"
#include "DataManager.h"

#include "ui_KursProject.h"

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
    void updateClientsTable();
    void updateConsultationsTable();

    void onAddClientRecord();
    void onDeleteClientRecord();
    void onLoadClients();
    void onSaveClients();
    void onFindClient();
    void onAbout();

    void showClientContextMenu(const QPoint& pos);
    void showConsultationContextMenu(const QPoint& pos);

    void onAddConsultationRecord();
    void onDeleteConsultationRecord();
    void onDeleteConsultationBySearch();
    void onLoadConsultations();
    void onSaveConsultations();
    void onFindConsultations();

    void onGenerateReport();

    void onDebugActionTriggered();
private:
    Ui::KursProjectClass* ui;
};

#endif // MAINWINDOW_H