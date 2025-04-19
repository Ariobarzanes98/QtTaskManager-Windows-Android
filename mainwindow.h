#pragma once

#include <QMainWindow>
#include <QVector>
#include <QCloseEvent>
#include <QListWidget>
#include "taskmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onAddTaskClicked();
    void onDeleteTaskClicked();
    void onSearchTextChanged(const QString& text);
    void on_actionExit_triggered();
    void on_actionNewList_triggered();
    void onTabChanged(int index);

private:
    Ui::MainWindow *ui;

    QVector<TaskManager> projectManagers;

    void loadTasksForCurrentTab();
    void saveTasksForCurrentTab();
    void addTaskToListWidget(QListWidget* list, const Task& task);
    void connectTabWidgets(int index);

    
    QWidget* createTabUI(int index);
};
