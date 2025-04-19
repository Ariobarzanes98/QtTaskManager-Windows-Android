#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "taskmanager.h"

#include <QListWidgetItem>
#include <QFont>
#include <QColor>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QListWidget>
#include <QFile>
#include <QTextStream>

#include <QScrollArea>
#include <QSizePolicy>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setGeometry(QGuiApplication::primaryScreen()->availableGeometry());


    int tabCount = 0;
    QFile countFile("tab_count.txt");
    if (countFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&countFile);
        in >> tabCount;
        countFile.close();
    }

    if (tabCount <= 0) {
        tabCount = 1;
    }

    for (int i = 0; i < tabCount; ++i) {
        QWidget* tab = createTabUI(i);
        ui->tabWidgetProjects->addTab(tab, QString("List %1").arg(i + 1));

        TaskManager manager;
        manager.loadFromFile(QString("tasks_project_%1.json").arg(i));
        projectManagers.append(manager);

        connectTabWidgets(i);
    }

    int oldIndex = ui->tabWidgetProjects->currentIndex();
    for (int i = 0; i < tabCount; ++i) {
        ui->tabWidgetProjects->setCurrentIndex(i);
        loadTasksForCurrentTab();
    }
    ui->tabWidgetProjects->setCurrentIndex(oldIndex);

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
    connect(ui->tabWidgetProjects, &QTabWidget::currentChanged, this, &MainWindow::onTabChanged);
}

MainWindow::~MainWindow() {
    delete ui;
}

QWidget* MainWindow::createTabUI(int index) {
    QSize screenSize = QGuiApplication::primaryScreen()->availableGeometry().size();
    int targetHeight = screenSize.height() * 0.5;
    int targetWidth = screenSize.width() * 0.95;

    QWidget* tab = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(tab);

    QLineEdit* search = new QLineEdit;
    search->setPlaceholderText("Search...");
    search->setObjectName("lineEditSearch");

    QLineEdit* input = new QLineEdit;
    input->setObjectName("lineEditTaskInput");

    QPushButton* addBtn = new QPushButton("Add Task");
    addBtn->setObjectName("pushButtonAddTask");

    QPushButton* delBtn = new QPushButton("Delete Task");
    delBtn->setObjectName("pushButtonDeleteTask");

    QHBoxLayout* btnLayout = new QHBoxLayout;
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delBtn);

    QListWidget* list = new QListWidget;
    list->setObjectName("listWidgetTasks");

    layout->addWidget(search);
    layout->addWidget(input);
    layout->addLayout(btnLayout);
    layout->addWidget(list);

    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);

    tab->setMinimumSize(targetWidth, targetHeight);
    tab->setMaximumSize(targetWidth, targetHeight);
    tab->setLayout(layout);

    return tab;
}









void MainWindow::on_actionNewList_triggered() {
    int index = ui->tabWidgetProjects->count();
    QWidget* tab = createTabUI(index);

    ui->tabWidgetProjects->addTab(tab, QString("List %1").arg(index + 1));
    projectManagers.append(TaskManager());

    connectTabWidgets(index);
    ui->tabWidgetProjects->setCurrentIndex(index);
}

void MainWindow::connectTabWidgets(int index) {
    QWidget* tab = ui->tabWidgetProjects->widget(index);
    if (!tab) return;

    auto addBtn = tab->findChild<QPushButton*>("pushButtonAddTask");
    auto delBtn = tab->findChild<QPushButton*>("pushButtonDeleteTask");
    auto search = tab->findChild<QLineEdit*>("lineEditSearch");
    auto list = tab->findChild<QListWidget*>("listWidgetTasks");

    if (addBtn) connect(addBtn, &QPushButton::clicked, this, &MainWindow::onAddTaskClicked);
    if (delBtn) connect(delBtn, &QPushButton::clicked, this, &MainWindow::onDeleteTaskClicked);
    if (search) connect(search, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);

    if (list) {
        connect(list, &QListWidget::itemChanged, this, [=](QListWidgetItem* item) {
            int row = list->row(item);
            bool done = item->checkState() == Qt::Checked;
            int currentTab = ui->tabWidgetProjects->currentIndex();
            if (currentTab < projectManagers.size()) {
                projectManagers[currentTab].toggleTaskDone(row, done);
            }

            QFont font = item->font();
            font.setStrikeOut(done);
            item->setFont(font);
            item->setForeground(done ? Qt::gray : Qt::black);
        });
    }
}

void MainWindow::onTabChanged(int index) {
    saveTasksForCurrentTab();
    loadTasksForCurrentTab();
}

void MainWindow::onAddTaskClicked() {
    QWidget* tab = ui->tabWidgetProjects->currentWidget();
    auto input = tab->findChild<QLineEdit*>("lineEditTaskInput");
    auto list = tab->findChild<QListWidget*>("listWidgetTasks");
    int index = ui->tabWidgetProjects->currentIndex();

    if (input && list && !input->text().isEmpty() && index < projectManagers.size()) {
        QString taskText = input->text();
        projectManagers[index].addTask(taskText);
        addTaskToListWidget(list, {taskText, false});
        input->clear();
    }
}

void MainWindow::onDeleteTaskClicked() {
    QWidget* tab = ui->tabWidgetProjects->currentWidget();
    auto list = tab->findChild<QListWidget*>("listWidgetTasks");
    int index = ui->tabWidgetProjects->currentIndex();
    int row = list ? list->currentRow() : -1;

    if (list && row >= 0 && index < projectManagers.size()) {
        projectManagers[index].removeTask(row);
        delete list->takeItem(row);
    }
}

void MainWindow::onSearchTextChanged(const QString& text) {
    QWidget* tab = ui->tabWidgetProjects->currentWidget();
    auto list = tab->findChild<QListWidget*>("listWidgetTasks");
    int index = ui->tabWidgetProjects->currentIndex();

    if (!list || index >= projectManagers.size()) return;

    list->clear();
    const auto& tasks = projectManagers[index].getTasks();
    for (const auto& task : tasks) {
        if (task.text.contains(text, Qt::CaseInsensitive)) {
            addTaskToListWidget(list, task);
        }
    }
}

void MainWindow::addTaskToListWidget(QListWidget* list, const Task& task) {
    QListWidgetItem* item = new QListWidgetItem(task.text);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(task.done ? Qt::Checked : Qt::Unchecked);

    QFont font = item->font();
    font.setStrikeOut(task.done);
    item->setFont(font);
    item->setForeground(task.done ? Qt::gray : Qt::black);

    list->addItem(item);
}

void MainWindow::loadTasksForCurrentTab() {
    int index = ui->tabWidgetProjects->currentIndex();
    if (index < 0 || index >= projectManagers.size()) return;

    QString filename = QString("tasks_project_%1.json").arg(index);
    projectManagers[index].loadFromFile(filename);

    QWidget* tab = ui->tabWidgetProjects->widget(index);
    auto list = tab->findChild<QListWidget*>("listWidgetTasks");
    if (!list) return;

    list->clear();
    for (const auto& task : projectManagers[index].getTasks()) {
        addTaskToListWidget(list, task);
    }
}

void MainWindow::saveTasksForCurrentTab() {
    int index = ui->tabWidgetProjects->currentIndex();
    if (index < 0 || index >= projectManagers.size()) return;

    QString filename = QString("tasks_project_%1.json").arg(index);
    projectManagers[index].saveToFile(filename);
}

void MainWindow::on_actionExit_triggered() {
    close();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    for (int i = 0; i < projectManagers.size(); ++i) {
        QString filename = QString("tasks_project_%1.json").arg(i);
        projectManagers[i].saveToFile(filename);
    }

    QFile countFile("tab_count.txt");
    if (countFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&countFile);
        out << projectManagers.size();
        countFile.close();
    }

    QMainWindow::closeEvent(event);
}
