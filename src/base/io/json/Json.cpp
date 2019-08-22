/* XMRig
 * Copyright 2010      Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2012-2014 pooler      <pooler@litecoinpool.org>
 * Copyright 2014      Lucas Jones <https://github.com/lucasjones>
 * Copyright 2014-2016 Wolf9466    <https://github.com/OhGodAPet>
 * Copyright 2016      Jay D Dee   <jayddee246@gmail.com>
 * Copyright 2017-2018 XMR-Stak    <https://github.com/fireice-uk>, <https://github.com/psychocrypt>
 * Copyright 2018-2019 SChernykh   <https://github.com/SChernykh>
 * Copyright 2016-2019 XLARig       <https://github.com/xmrig>, <support@xmrig.com>
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


#include "base/io/json/Json.h"
#include "rapidjson/document.h"


namespace xlarig {

static const rapidjson::Value kNullValue;

}


bool xlarig::Json::getBool(const rapidjson::Value &obj, const char *key, bool defaultValue)
{
    auto i = obj.FindMember(key);
    if (i != obj.MemberEnd() && i->value.IsBool()) {
        return i->value.GetBool();
    }

    return defaultValue;
}


const char *xlarig::Json::getString(const rapidjson::Value &obj, const char *key,  const char *defaultValue)
{
    auto i = obj.FindMember(key);
    if (i != obj.MemberEnd() && i->value.IsString()) {
        return i->value.GetString();
    }

    return defaultValue;
}


const rapidjson::Value &xlarig::Json::getArray(const rapidjson::Value &obj, const char *key)
{
    auto i = obj.FindMember(key);
    if (i != obj.MemberEnd() && i->value.IsArray()) {
        return i->value;
    }

    return kNullValue;
}


const rapidjson::Value &xlarig::Json::getObject(const rapidjson::Value &obj, const char *key)
{
    auto i = obj.FindMember(key);
    if (i != obj.MemberEnd() && i->value.IsObject()) {
        return i->value;
    }

    return kNullValue;
}


const rapidjson::Value &xlarig::Json::getValue(const rapidjson::Value &obj, const char *key)
{
    auto i = obj.FindMember(key);
    if (i != obj.MemberEnd()) {
        return i->value;
    }

    return kNullValue;
}


int xlarig::Json::getInt(const rapidjson::Value &obj, const char *key, int defaultValue)
{
    auto i = obj.FindMember(key);
    if (i != obj.MemberEnd() && i->value.IsInt()) {
        return i->value.GetInt();
    }

    return defaultValue;
}


int64_t xlarig::Json::getInt64(const rapidjson::Value &obj, const char *key, int64_t defaultValue)
{
    auto i = obj.FindMember(key);
    if (i != obj.MemberEnd() && i->value.IsInt64()) {
        return i->value.GetInt64();
    }

    return defaultValue;
}


uint64_t xlarig::Json::getUint64(const rapidjson::Value &obj, const char *key, uint64_t defaultValue)
{
    auto i = obj.FindMember(key);
    if (i != obj.MemberEnd() && i->value.IsUint64()) {
        return i->value.GetUint64();
    }

    return defaultValue;
}


unsigned xlarig::Json::getUint(const rapidjson::Value &obj, const char *key, unsigned defaultValue)
{
    auto i = obj.FindMember(key);
    if (i != obj.MemberEnd() && i->value.IsUint()) {
        return i->value.GetUint();
    }

    return defaultValue;
}


bool xlarig::JsonReader::isEmpty() const
{
    return !m_obj.IsObject() || m_obj.ObjectEmpty();
}
