#include "taskmanager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

void TaskManager::addTask(const QString& task) {
    tasks.append({task, false});
}

void TaskManager::toggleTaskDone(int index, bool done) {
    if (index >= 0 && index < tasks.size()) {
        tasks[index].done = done;
    }
}

void TaskManager::removeTask(int index) {
    if (index >= 0 && index < tasks.size()) {
        tasks.removeAt(index);
    }
}

QVector<Task> TaskManager::getTasks() const {
    return tasks;
}

bool TaskManager::saveToFile(const QString& filename) const {
    QJsonArray jsonArray;
    for (const auto& task : tasks) {
        QJsonObject obj;
        obj["text"] = task.text;
        obj["done"] = task.done;
        jsonArray.append(obj);
    }

    QJsonDocument doc(jsonArray);
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) return false;

    file.write(doc.toJson());
    file.close();
    return true;
}

bool TaskManager::loadFromFile(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) return false;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isArray()) return false;

    tasks.clear();
    QJsonArray jsonArray = doc.array();
    for (const QJsonValue& val : jsonArray) {
        QJsonObject obj = val.toObject();
        tasks.append({obj["text"].toString(), obj["done"].toBool()});
    }

    return true;
}
