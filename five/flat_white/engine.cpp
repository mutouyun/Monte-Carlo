#include <QDebug>
#include <QJsonObject>

#include <thread>
#include <vector>

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

    five_prepare([](void* p, std::size_t t, st_info* l, std::size_t s) {
        qDebug() << "games[" << ++counter__ << "]:" << t;

        std::vector<QJsonObject> infos;
        for (std::size_t i = 0; i < s; ++i) {
            infos.push_back(QJsonObject {
                { "x"     , l[i].x_      },
                { "y"     , l[i].y_      },
                { "rate"  , l[i].rate_   },
                { "score" , l[i].score_  },
                { "visits", l[i].visits_ }
            });
        }

        std::sort(infos.begin(), infos.end(), [](QJsonObject const & x, QJsonObject const & y) {
            return x["visits"].toInt() > y["visits"].toInt();
        });
        QJsonArray r1;
        for (QJsonObject const & i : infos) r1 << i;

        std::sort(infos.begin(), infos.end(), [](QJsonObject const & x, QJsonObject const & y) {
            return x["score"].toInt() > y["score"].toInt();
        });
        QJsonArray r2;
        for (QJsonObject const & i : infos) r2 << i;

        emit ((Engine*)p)->thinking(r1, r2);
    }, 200000, 10, this);

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
