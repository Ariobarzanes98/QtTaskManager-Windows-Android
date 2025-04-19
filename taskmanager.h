#pragma once

#include <QString>
#include <QVector>

struct Task {
    QString text;
    bool done = false;
};

class TaskManager {
public:
    void addTask(const QString& task);
    void toggleTaskDone(int index, bool done);
    void removeTask(int index);
    QVector<Task> getTasks() const;

    bool saveToFile(const QString& filename) const;
    bool loadFromFile(const QString& filename);

private:
    QVector<Task> tasks;
};
