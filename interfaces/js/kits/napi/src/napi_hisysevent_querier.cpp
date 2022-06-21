/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "napi_hisysevent_querier.h"

#include "hilog/log.h"
#include "napi_hisysevent_util.h"

namespace OHOS {
namespace HiviewDFX {
namespace {
constexpr HiLogLabel LABEL = { LOG_CORE, 0xD002D08, "NAPI_HISYSEVENT_QUERIER" };
constexpr char ON_QUERY_ATTR[] = "onQuery";
constexpr char ON_COMPLETE_ATTR[] = "onComplete";
constexpr size_t ON_QUERY_COMPLTE_PARAM_COUNT = 2;
}

void NapiHiSysEventQuerier::OnQuery(const std::vector<std::string>& sysEvent,
    const std::vector<int64_t>& seq)
{
    jsCallbackManager->Add(callbackContext, [this, sysEvent, seq] (const napi_env env, const napi_ref ref) {
        napi_value sysEventInfoJsArray = nullptr;
        napi_create_array_with_length(env, sysEvent.size(), &sysEventInfoJsArray);
        NapiHiSysEventUtil::CreateJsSysEventInfoArray(env, sysEvent, sysEventInfoJsArray);
        napi_value seqJsArray = nullptr;
        napi_create_array_with_length(env, seq.size(), &seqJsArray);
        NapiHiSysEventUtil::CreateJsInt64Array(env, seq, seqJsArray);
        napi_value argv[ON_QUERY_COMPLTE_PARAM_COUNT] = {sysEventInfoJsArray, seqJsArray};
        napi_value querier = nullptr;
        napi_get_reference_value(env, ref, &querier);
        napi_value onQuery = NapiHiSysEventUtil::GetPropertyByName(env, querier, ON_QUERY_ATTR);
        napi_value ret = nullptr;
        napi_status status = napi_call_function(env, querier, onQuery, ON_QUERY_COMPLTE_PARAM_COUNT,
            argv, &ret);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "failed to call OnQuery JS function.");
        }
    });
}

void NapiHiSysEventQuerier::OnComplete(int32_t reason, int32_t total)
{
    jsCallbackManager->Add(callbackContext, [this, reason, total] (const napi_env env, const napi_ref ref) {
        napi_value reasonJsParam = nullptr;
        NapiHiSysEventUtil::CreateInt32Value(env, reason, reasonJsParam);
        napi_value totalJsParam = nullptr;
        NapiHiSysEventUtil::CreateInt32Value(env, total, totalJsParam);
        napi_value argv[ON_QUERY_COMPLTE_PARAM_COUNT] = {reasonJsParam, totalJsParam};
        napi_value querier = nullptr;
        napi_get_reference_value(env, ref, &querier);
        napi_value OnComplete = NapiHiSysEventUtil::GetPropertyByName(env, querier, ON_COMPLETE_ATTR);
        napi_value ret = nullptr;
        napi_status status = napi_call_function(env, querier, OnComplete, ON_QUERY_COMPLTE_PARAM_COUNT,
            argv, &ret);
        if (status != napi_ok) {
            HiLog::Error(LABEL, "failed to call OnComplete JS function.");
        }
    }, [this] () {
        if (this->onCompleteHandler != nullptr && this->callbackContext != nullptr) {
            this->onCompleteHandler(this->callbackContext->env, this->callbackContext->ref);
        }
    });
}
} // HiviewDFX
} // OHOS