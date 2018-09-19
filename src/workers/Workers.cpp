/* XTLRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2016-2018 XMRig       <https://github.com/xmrig>, <support@xmrig.com>
 * Copyright 2018 XTLRig       <https://github.com/stellitecoin>, <support@stellite.cash>
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cmath>
#include <inttypes.h>
#include <thread>


#include "api/Api.h"
#include "common/log/Log.h"
#include "core/Config.h"
#include "core/Controller.h"
#include "crypto/CryptoNight_constants.h"
#include "interfaces/IJobResultListener.h"
#include "interfaces/IThread.h"
#include "Mem.h"
#include "rapidjson/document.h"
#include "workers/Handle.h"
#include "workers/MultiWorker.h"
#include "workers/Workers.h"


bool Workers::m_active = false;
bool Workers::m_enabled = true;
IJobResultListener *Workers::m_listener = nullptr;
Job Workers::m_job;
Workers::LaunchStatus Workers::m_status;
std::atomic<int> Workers::m_paused;
std::atomic<uint64_t> Workers::m_sequence;
std::list<JobResult> Workers::m_queue;
std::vector<Handle *> Workers::m_workers;
uint64_t Workers::m_ticks = 0;
uv_async_t Workers::m_async;
uv_mutex_t Workers::m_mutex;
uv_rwlock_t Workers::m_rwlock;
uv_timer_t Workers::m_timer;
xtlrig::Controller *Workers::m_controller = nullptr;


Job Workers::job() {
    uv_rwlock_rdlock(&m_rwlock);
    Job job = m_job;
    uv_rwlock_rdunlock(&m_rwlock);

    return job;
}


size_t Workers::hugePages() {
    uv_mutex_lock(&m_mutex);
    const size_t hugePages = m_status.hugePages;
    uv_mutex_unlock(&m_mutex);

    return hugePages;
}


size_t Workers::threads() {
    uv_mutex_lock(&m_mutex);
    const size_t threads = m_status.threads;
    uv_mutex_unlock(&m_mutex);

    return threads;
}

void Workers::setEnabled(bool enabled) {
    if (m_enabled == enabled) {
        return;
    }

    m_enabled = enabled;
    if (!m_active) {
        return;
    }

    m_paused = enabled ? 0 : 1;
    m_sequence++;
}


void Workers::setJob(const Job &job, bool donate) {
    uv_rwlock_wrlock(&m_rwlock);
    m_job = job;

    if (donate) {
        m_job.setPoolId(-1);
    }
    uv_rwlock_wrunlock(&m_rwlock);

    m_active = true;
    if (!m_enabled) {
        return;
    }

    m_sequence++;
    m_paused = 0;
}

bool isStop;
time_t startTime;
time_t nowTime;

void Workers::start(xtlrig::Controller *controller) {
    m_controller = controller;

    const std::vector<xtlrig::IThread *> &threads = controller->config()->threads();
    m_status.algo = controller->config()->algorithm().algo();
    m_status.colors = controller->config()->isColors();
    m_status.threads = threads.size();

    for (const xtlrig::IThread *thread : threads) {
        m_status.ways += thread->multiway();
    }

    uv_mutex_init(&m_mutex);
    uv_rwlock_init(&m_rwlock);

    m_sequence = 1;
    m_paused = 1;

    uv_async_init(uv_default_loop(), &m_async, Workers::onResult);
    uv_timer_init(uv_default_loop(), &m_timer);

    const int printTime = controller->config()->printTime();

//    LOG_INFO("percent:%d",controller->config()->sleepTimes());

    if (printTime > 0) {
        uv_timer_start(&m_timer, Workers::onTick, 2000, printTime * 1000);
    } else {
        uv_timer_start(&m_timer, Workers::onTick, 2000, 10000);
    }

    startTime = time(&startTime);

    isStop = false;

    uint32_t offset = 0;

    for (xtlrig::IThread *thread : threads) {
        Handle *handle = new Handle(thread, offset, m_status.ways);
        offset += thread->multiway();

        m_workers.push_back(handle);
        handle->start(Workers::onReady);
    }
}


void Workers::stop() {
    uv_timer_stop(&m_timer);
    isStop = true;
    uv_close(reinterpret_cast<uv_handle_t *>(&m_async), nullptr);
    m_paused = 0;
    m_sequence = 0;

    for (size_t i = 0; i < m_workers.size(); ++i) {
        m_workers[i]->join();
    }
}


void Workers::submit(const JobResult &result) {
    uv_mutex_lock(&m_mutex);
    m_queue.push_back(result);
    uv_mutex_unlock(&m_mutex);

    uv_async_send(&m_async);
}


#ifndef XMRIG_NO_API
void Workers::threadsSummary(rapidjson::Document &doc)
{
    uv_mutex_lock(&m_mutex);
    const uint64_t pages[2] = { m_status.hugePages, m_status.pages };
    const uint64_t memory   = m_status.ways * xtlrig::cn_select_memory(m_status.algo);
    uv_mutex_unlock(&m_mutex);

    auto &allocator = doc.GetAllocator();

    rapidjson::Value hugepages(rapidjson::kArrayType);
    hugepages.PushBack(pages[0], allocator);
    hugepages.PushBack(pages[1], allocator);

    doc.AddMember("hugepages", hugepages, allocator);
    doc.AddMember("memory", memory, allocator);
}
#endif


void Workers::onReady(void *arg) {
    auto handle = static_cast<Handle *>(arg);

    IWorker *worker = nullptr;

    switch (handle->config()->multiway()) {
        case 1:
            worker = new MultiWorker<1>(handle);
            break;

        case 2:
            worker = new MultiWorker<2>(handle);
            break;

        case 3:
            worker = new MultiWorker<3>(handle);
            break;

        case 4:
            worker = new MultiWorker<4>(handle);
            break;

        case 5:
            worker = new MultiWorker<5>(handle);
            break;

        default:
            break;
    }

    handle->setWorker(worker);

    if (!worker->selfTest()) {
        LOG_ERR("thread %zu error: \"hash self-test failed\".", handle->worker()->id());

        return;
    }

    start(worker);
}


void Workers::onResult(uv_async_t *handle) {
    std::list<JobResult> results;

    uv_mutex_lock(&m_mutex);
    while (!m_queue.empty()) {
        results.push_back(std::move(m_queue.front()));
        m_queue.pop_front();
    }
    uv_mutex_unlock(&m_mutex);

    for (auto result : results) {
        m_listener->onJobResult(result);
    }

    results.clear();
}

#include <unistd.h>

uint64_t startCount = 0;
uint64_t count = 0;
double highHash = 0.0;

void Workers::onTick(uv_timer_t *handle) {

    calc();

}

inline const char *format(double h, char *buf, size_t size) {
    if (std::isnormal(h)) {
        snprintf(buf, size, "%03.1f", h);
        return buf;
    }

    return "n/a";
}

void Workers::calc() {
    if (isStop || !m_active) {
        return;
    }

    count = 0;

    for (Handle *handle : m_workers) {
        if (!handle->worker()) {
            return;
        }

        count += handle->worker()->hashCount();
    }

    time(&nowTime);

    char num1[8];
    char num3[8];

    double hash;

    if (nowTime - startTime > 0) {
        hash = (count - startCount) / (nowTime - startTime);
    } else {
        hash = 0;
    }

    char *res;

    if (std::isnormal(hash)) {
        snprintf(num1, sizeof(num1), "%03.1f", hash);
        res = num1;
    } else {
        sleep(1);
        calc();
        return;
    }

    if (hash > highHash) {
        highHash = hash;
    }

    if (m_controller->config()->isSimpleSpeed()) {
        LOG_INFO(m_controller->config()->isColors()
                 ? "\x1B[01;37mspeed\x1B[0m \x1B[01;36m%s H/s\x1B[0m max: \x1B[01;36m%s H/s"
                 : "speed %s H/s max: %s H/s", res, format(highHash, num3, sizeof(num3))
        );
    } else {
        LOG_INFO(m_controller->config()->isColors()
                 ? "\x1B[01;37mspeed\x1B[0m 2.5s/10s/30s \x1B[01;36m%s \x1B[22;36m%s %s \x1B[01;36mH/s\x1B[0m max: \x1B[01;36m%s H/s"
                 : "speed 2.5s/10s/30s %s %s %s H/s max: %s H/s", res, res, res, format(highHash, num3, sizeof(num3))
        );
    }

    startCount = count;
    time(&startTime);
}


void Workers::start(IWorker *worker) {
    const Worker *w = static_cast<const Worker *>(worker);

    uv_mutex_lock(&m_mutex);
    m_status.started++;
    m_status.pages += w->memory().pages;
    m_status.hugePages += w->memory().hugePages;

    if (m_status.started == m_status.threads) {
        const double percent = (double) m_status.hugePages / m_status.pages * 100.0;
        const size_t memory = m_status.ways * xtlrig::cn_select_memory(m_status.algo) / 1048576;

        if (m_status.colors) {
            LOG_INFO(GREEN_BOLD("READY (CPU)")
                             " threads "
                             CYAN_BOLD("%zu(%zu)")
                             " percent "
                             CYAN_BOLD("%zu%%")
                             " huge pages %s%zu/%zu %1.0f%%\x1B[0m memory "
                             CYAN_BOLD("%zu.0 MB")
                             "",
                     m_status.threads, m_status.ways, m_controller->config()->cpuPercent(),
                     (m_status.hugePages == m_status.pages ? "\x1B[1;32m" : (m_status.hugePages == 0 ? "\x1B[1;31m"
                                                                                                     : "\x1B[1;33m")),
                     m_status.hugePages, m_status.pages, percent, memory);
        } else {
            LOG_INFO("READY (CPU) threads %zu(%zu) percent %zu%% huge pages %zu/%zu %1.0f%% memory %zu.0 MB",
                     m_status.threads, m_status.ways, m_controller->config()->cpuPercent(), m_status.hugePages,
                     m_status.pages, percent, memory);
        }
    }

    uv_mutex_unlock(&m_mutex);

    worker->start(m_controller);
}
