#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    a.setStyleSheet(R"(
    QMainWindow {
        background-color: #b8c9cc; /* light blue */
    }
    QListWidget {
        background-color: #ffffff;
        color: #333;
        border: 1px solid #ccc;
    }
    QLineEdit {
        background-color: #ffffff;
        color: #000000;
        border: 1px solid #aaa;
        padding: 4px;
    }
    QPushButton#pushButtonAddTask,
    QPushButton#pushButtonDeleteTask {
        background-color: #a8e6a3; /* light green */
        color: #000000;
        border: 1px solid #7fc97f;
        padding: 6px;
        border-radius: 4px;
    }
    QPushButton#pushButtonAddTask:hover,
    QPushButton#pushButtonDeleteTask:hover {
        background-color: #90d890; /* darker on hover */
    }
    QMenuBar {
        background-color: #b0f051;
        color: #000;
    }
    QMenuBar::item:selected {
        background-color: #c0e0f0;
    }
    QMenu {
        background-color: #ffffff;
        color: #000000;
    }
    QMenu::item:selected {
        background-color: #e0f7ff;
    }
)");




    MainWindow w;
    w.show();
    return a.exec();
}
