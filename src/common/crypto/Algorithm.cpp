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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


#include "common/crypto/Algorithm.h"


#ifdef _MSC_VER
#   define strncasecmp _strnicmp
#   define strcasecmp  _stricmp
#endif


#ifndef ARRAY_SIZE
#   define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif


struct AlgoData
{
    const char *name;
    const char *shortName;
    xtlrig::Algo algo;
    xtlrig::Variant variant;
};


static AlgoData const algorithms[] = {
    { "cryptonight",           "cn",           xtlrig::CRYPTONIGHT,       xtlrig::VARIANT_AUTO },
    { "cryptonight/0",         "cn/0",         xtlrig::CRYPTONIGHT,       xtlrig::VARIANT_0    },
    { "cryptonight/1",         "cn/1",         xtlrig::CRYPTONIGHT,       xtlrig::VARIANT_1    },
    { "cryptonight/xtl",       "cn/xtl",       xtlrig::CRYPTONIGHT,       xtlrig::VARIANT_XTL  },
    { "cryptonight/msr",       "cn/msr",       xtlrig::CRYPTONIGHT,       xtlrig::VARIANT_MSR  },

#   ifndef XMRIG_NO_AEON
    { "cryptonight-lite",      "cn-lite",      xtlrig::CRYPTONIGHT_LITE,  xtlrig::VARIANT_AUTO },
    { "cryptonight-light",     "cn-light",     xtlrig::CRYPTONIGHT_LITE,  xtlrig::VARIANT_AUTO },
    { "cryptonight-lite/0",    "cn-lite/0",    xtlrig::CRYPTONIGHT_LITE,  xtlrig::VARIANT_0    },
    { "cryptonight-lite/1",    "cn-lite/1",    xtlrig::CRYPTONIGHT_LITE,  xtlrig::VARIANT_1    },
    { "cryptonight-lite/ipbc", "cn-lite/ipbc", xtlrig::CRYPTONIGHT_LITE,  xtlrig::VARIANT_IPBC },
#   endif

#   ifndef XMRIG_NO_SUMO
    { "cryptonight-heavy",     "cn-heavy",     xtlrig::CRYPTONIGHT_HEAVY, xtlrig::VARIANT_0    },
    { "cryptonight-heavy/0",   "cn-heavy/0",   xtlrig::CRYPTONIGHT_HEAVY, xtlrig::VARIANT_0    },
    { "cryptonight-heavy/xhv", "cn-heavy/xhv", xtlrig::CRYPTONIGHT_HEAVY, xtlrig::VARIANT_XHV  },
#   endif
};


#ifdef XMRIG_PROXY_PROJECT
static AlgoData const xmrStakAlgorithms[] = {
    { "cryptonight-monerov7",    nullptr, xtlrig::CRYPTONIGHT,      xtlrig::VARIANT_1    },
    { "cryptonight_v7",          nullptr, xtlrig::CRYPTONIGHT,      xtlrig::VARIANT_1    },
    { "cryptonight_v7_stellite", nullptr, xtlrig::CRYPTONIGHT,      xtlrig::VARIANT_XTL  },
    { "cryptonight_lite",        nullptr, xtlrig::CRYPTONIGHT_LITE, xtlrig::VARIANT_0    },
    { "cryptonight-aeonv7",      nullptr, xtlrig::CRYPTONIGHT_LITE, xtlrig::VARIANT_1    },
    { "cryptonight_lite_v7",     nullptr, xtlrig::CRYPTONIGHT_LITE, xtlrig::VARIANT_1    },
    { "cryptonight_lite_v7_xor", nullptr, xtlrig::CRYPTONIGHT_LITE, xtlrig::VARIANT_IPBC },
};
#endif


static const char *variants[] = {
    "0",
    "1",
    "ipbc",
    "xtl",
    "msr",
    "xhv"
};


bool xtlrig::Algorithm::isValid() const
{
    if (m_algo == INVALID_ALGO) {
        return false;
    }

    for (size_t i = 0; i < ARRAY_SIZE(algorithms); i++) {
        if (algorithms[i].algo == m_algo && algorithms[i].variant == m_variant) {
            return true;
        }
    }

    return false;
}


const char *xtlrig::Algorithm::variantName() const
{
    if (m_variant == VARIANT_AUTO) {
        return "auto";
    }

    return variants[m_variant];
}


void xtlrig::Algorithm::parseAlgorithm(const char *algo)
{
    m_algo    = INVALID_ALGO;
    m_variant = VARIANT_AUTO;

    assert(algo != nullptr);
    if (algo == nullptr) {
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(algorithms); i++) {
        if ((strcasecmp(algo, algorithms[i].name) == 0) || (strcasecmp(algo, algorithms[i].shortName) == 0)) {
            m_algo    = algorithms[i].algo;
            m_variant = algorithms[i].variant;
            break;
        }
    }

    if (m_algo == INVALID_ALGO) {
        assert(false);
    }
}


void xtlrig::Algorithm::parseVariant(const char *variant)
{
    m_variant = VARIANT_AUTO;

    for (size_t i = 0; i < ARRAY_SIZE(variants); i++) {
        if (strcasecmp(variant, variants[i]) == 0) {
            m_variant = static_cast<Variant>(i);
            break;
        }
    }
}


void xtlrig::Algorithm::parseVariant(int variant)
{
    if (variant >= VARIANT_AUTO && variant < VARIANT_MAX) {
       m_variant = static_cast<Variant>(variant);
    }
    else {
        assert(false);
    }
}


void xtlrig::Algorithm::setAlgo(Algo algo)
{
    m_algo = algo;
}


#ifdef XMRIG_PROXY_PROJECT
void xtlrig::Algorithm::parseXmrStakAlgorithm(const char *algo)
{
    m_algo    = INVALID_ALGO;
    m_variant = VARIANT_AUTO;

    assert(algo != nullptr);
    if (algo == nullptr) {
        return;
    }

    for (size_t i = 0; i < ARRAY_SIZE(xmrStakAlgorithms); i++) {
        if (strcasecmp(algo, xmrStakAlgorithms[i].name) == 0) {
            m_algo    = xmrStakAlgorithms[i].algo;
            m_variant = xmrStakAlgorithms[i].variant;
            break;
        }
    }

    if (m_algo == INVALID_ALGO) {
        assert(false);
    }
}
#endif


const char *xtlrig::Algorithm::name(bool shortName) const
{
    for (size_t i = 0; i < ARRAY_SIZE(algorithms); i++) {
        if (algorithms[i].algo == m_algo && algorithms[i].variant == m_variant) {
            return shortName ? algorithms[i].shortName : algorithms[i].name;
        }
    }

    return "invalid";
}
