#ifndef WORKERPOOL_H
#define WORKERPOOL_H

#include <QObject>
#include <QThreadPool>

// Centralized thread pool manager to offload heavy lifting
class WorkerPool : public QObject {
    Q_OBJECT
public:
    static WorkerPool& instance() {
        static WorkerPool instance;
        return instance;
    }

    QThreadPool* threadPool() {
        return m_pool;
    }

private:
    WorkerPool(QObject *parent = nullptr) : QObject(parent) {
        m_pool = new QThreadPool(this);
        m_pool->setMaxThreadCount(QThread::idealThreadCount());
    }
    QThreadPool* m_pool;
};

#endif // WORKERPOOL_H
