#include <QDebug>

#include <thread>

#include "engine.h"
#include "five.h"

namespace {

int counter__ = 0;

void startThinking(Engine* e, unsigned x, unsigned y) {
    counter__ = 0;
    e->setProcessing(true);
    std::thread([e, x, y]() mutable {
        auto s = five_calc_next(&x, &y);
        qDebug() << "score:" << s;
        if (s > 0) {
            qDebug() << "x:" << x << "y:" << y;
            emit e->thinkingFinished(x, y);
        }
        e->setProcessing(false);
    }).detach();
}

} // namespace

Engine::Engine(QObject *parent)
    : QObject(parent)
    , processing_(false) {
    five_prepare([](std::size_t t) {
        qDebug() << "games[" << ++counter__ << "]:" << t;
    }, 200000, 20);
    five_start_game();
}

bool Engine::processing() const {
    return processing_;
}

void Engine::setProcessing(bool p) {
    if (processing_ == p) return;
    processing_ = p;
    emit processingChanged();
}

void Engine::restart() {
    five_start_game();
}

void Engine::move(unsigned x, unsigned y) {
    qDebug() << "move:" << x << "," << y;
    startThinking(this, x, y);
}

void Engine::calcNext() {
    startThinking(this, -1, -1);
}
