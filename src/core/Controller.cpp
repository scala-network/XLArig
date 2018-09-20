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

#include <assert.h>


#include "common/config/ConfigLoader.h"
#include "common/interfaces/IControllerListener.h"
#include "common/log/ConsoleLog.h"
#include "common/log/FileLog.h"
#include "common/log/Log.h"
#include "common/Platform.h"
#include "core/Config.h"
#include "core/Controller.h"
#include "Cpu.h"
#include "net/Network.h"


#ifdef HAVE_SYSLOG_H
#   include "common/log/SysLog.h"
#endif


class xtlrig::ControllerPrivate
{
public:
    inline ControllerPrivate() :
        network(nullptr),
        config(nullptr)
    {}


    inline ~ControllerPrivate()
    {
        delete network;
        delete config;
    }


    Network *network;
    std::vector<xtlrig::IControllerListener *> listeners;
    xtlrig::Config *config;
};


xtlrig::Controller::Controller()
    : d_ptr(new ControllerPrivate())
{
}


xtlrig::Controller::~Controller()
{
    ConfigLoader::release();

    delete d_ptr;
}


bool xtlrig::Controller::isReady() const
{
    return d_ptr->config && d_ptr->network;
}


xtlrig::Config *xtlrig::Controller::config() const
{
    assert(d_ptr->config != nullptr);

    return d_ptr->config;
}


int xtlrig::Controller::init(int argc, char **argv)
{
    Cpu::init();

    d_ptr->config = xtlrig::Config::load(argc, argv, this);
    if (!d_ptr->config) {
        return 1;
    }

    Log::init();
    Platform::init(config()->userAgent());
    Platform::setProcessPriority(d_ptr->config->priority());

    if (!config()->isBackground()) {
        Log::add(new ConsoleLog(this));
    }

    if (config()->logFile()) {
        Log::add(new FileLog(this, config()->logFile()));
    }

#   ifdef HAVE_SYSLOG_H
    if (config()->isSyslog()) {
        Log::add(new SysLog());
    }
#   endif

    d_ptr->network = new Network(this);
    return 0;
}


Network *xtlrig::Controller::network() const
{
    assert(d_ptr->network != nullptr);

    return d_ptr->network;
}


void xtlrig::Controller::addListener(IControllerListener *listener)
{
    d_ptr->listeners.push_back(listener);
}


void xtlrig::Controller::onNewConfig(IConfig *config)
{
    Config *previousConfig = d_ptr->config;
    d_ptr->config = static_cast<Config*>(config);

    for (xtlrig::IControllerListener *listener : d_ptr->listeners) {
        listener->onConfigChanged(d_ptr->config, previousConfig);
    }

    delete previousConfig;
}
