/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "raw_data_encoder.h"

#include "def.h"
#include "hilog/log.h"

#include "securec.h"

namespace OHOS {
namespace HiviewDFX {
namespace Encode {
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D08, "HiSysEvent-RawDataEncoder" };
}

uint8_t RawDataEncoder::EncodedTag(uint8_t type)
{
    return (type << (TAG_BYTE_OFFSET + 1));
}

bool RawDataEncoder::StringValueEncoded(RawData& data, const std::string& val)
{
    if (!UnsignedVarintEncoded(data, EncodeType::LENGTH_DELIMITED, val.length())) {
        return false;
    }
    if (!data.Append(reinterpret_cast<uint8_t*>(const_cast<char*>(val.c_str())),
        val.length())) {
        HiLog::Error(LABEL, "String value copy failed.");
        return false;
    }
    return true;
}

bool RawDataEncoder::ValueTypeEncoded(RawData& data, bool isArray, ValueType type, uint8_t count)
{
    struct ParamValueType kvType {
        .isArray = isArray ? 1 : 0,
        .valueType = static_cast<uint8_t>(type),
        .valueByteCnt = count,
    };
    HiLog::Debug(LABEL, "Encoder isArray  is %{public}d.", static_cast<int>(kvType.isArray));
    HiLog::Debug(LABEL, "Encoder valueType  is %{public}d.", static_cast<int>(kvType.valueType));
    if (!data.Append(reinterpret_cast<uint8_t*>(&kvType), sizeof(struct ParamValueType))) {
        return false;
    }
    return true;
}
} // namespace Encode
} // namespace HiviewDFX
} // namespace OHOS