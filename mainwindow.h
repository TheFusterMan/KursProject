#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void onLoadConsultations();
    void onSaveConsultations();
    void onFindConsultations();

    void onGenerateReport();

    void onDebugButtonClicked();
private:
    Ui::KursProjectClass* ui;
};

#endif // MAINWINDOW_H