#pragma once

#include <QObject>

class Engine : public QObject {

    Q_OBJECT

    Q_PROPERTY(bool processing READ processing NOTIFY processingChanged)

public:
    explicit Engine(QObject* parent = nullptr);

    bool processing() const;
    void setProcessing(bool p);

    Q_INVOKABLE void restart();
    Q_INVOKABLE void move(unsigned x, unsigned y);
    Q_INVOKABLE void calcNext();

signals:
    void processingChanged();
    void thinkingFinished(unsigned x, unsigned y);

public slots:

private:
    bool processing_;
};
