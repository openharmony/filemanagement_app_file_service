/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef TEST_UNITTEST_MOCK_NAPI_H
#define TEST_UNITTEST_MOCK_NAPI_H

#include <gmock/gmock.h>

#ifdef FILE_SUBSYSTEM_DEBUG_LOCAL
#include <node_api.h>
#else
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#endif
#include "uv.h"

namespace OHOS::FileManagement::Backup {
class Napi {
public:
    virtual ~Napi() = default;
public:
    virtual napi_status napi_get_uv_event_loop(napi_env, struct uv_loop_s**) = 0;
    virtual napi_status napi_call_function(napi_env, napi_value, napi_value, size_t, const napi_value*, napi_value*)
        = 0;
    virtual napi_status napi_get_reference_value(napi_env, napi_ref, napi_value*) = 0;
    virtual napi_status napi_get_named_property(napi_env, napi_value, const char*, napi_value*) = 0;
    virtual napi_status napi_send_event(napi_env, const std::function<void()>, napi_event_priority) = 0;
    virtual napi_status napi_get_value_int32(napi_env, napi_value, int32_t*) = 0;
    virtual napi_status napi_get_value_int64(napi_env, napi_value, int64_t*) = 0;
    virtual napi_status napi_create_string_utf8(napi_env, const char*, size_t, napi_value*) = 0;
    virtual napi_status napi_create_int32(napi_env, int32_t, napi_value*) = 0;
    virtual napi_status napi_create_int64(napi_env, int64_t, napi_value*) = 0;
    virtual napi_status napi_get_value_string_utf8(napi_env, napi_value, char*, size_t, size_t*) = 0;
    virtual napi_status napi_get_boolean(napi_env, bool, napi_value*) = 0;
    virtual napi_status napi_create_array(napi_env, napi_value*) = 0;
    virtual napi_status napi_get_array_length(napi_env, napi_value, uint32_t*) = 0;
    virtual napi_status napi_get_element(napi_env, napi_value, uint32_t, napi_value*) = 0;
    virtual napi_status napi_escape_handle(napi_env, napi_escapable_handle_scope, napi_value, napi_value*) = 0;
    virtual napi_status napi_get_null(napi_env, napi_value*) = 0;
    virtual napi_status napi_create_object(napi_env, napi_value*) = 0;
    virtual napi_status napi_create_array_with_length(napi_env, size_t, napi_value*) = 0;
    virtual napi_status napi_create_double(napi_env, double, napi_value*) = 0;
    virtual napi_status napi_set_named_property(napi_env, napi_value, const char*, napi_value) = 0;
    virtual napi_status napi_get_and_clear_last_exception(napi_env, napi_value*) = 0;
    virtual napi_status napi_get_cb_info(napi_env, napi_callback_info, size_t*, napi_value*, napi_value*, void**) = 0;
    virtual napi_status napi_is_promise(napi_env, napi_value, bool*) = 0;
    virtual napi_status napi_is_callable(napi_env, napi_value, bool*) = 0;
    virtual napi_status napi_create_function(napi_env, const char*, size_t, napi_callback, void*, napi_value*) = 0;
    virtual napi_status napi_open_handle_scope(napi_env, napi_handle_scope*) = 0;
    virtual napi_status napi_close_handle_scope(napi_env, napi_handle_scope) = 0;
    virtual napi_status napi_is_exception_pending(napi_env, bool*) = 0;
    virtual napi_status napi_fatal_exception(napi_env, napi_value) = 0;
public:
    static inline std::shared_ptr<Napi> napi = nullptr;
};

class NapiMock : public Napi {
public:
    MOCK_METHOD(napi_status, napi_get_uv_event_loop, (napi_env, struct uv_loop_s**));
    MOCK_METHOD(napi_status, napi_call_function, (napi_env, napi_value, napi_value, size_t, const napi_value*,
        napi_value*));
    MOCK_METHOD(napi_status, napi_get_reference_value, (napi_env, napi_ref, napi_value*));
    MOCK_METHOD(napi_status, napi_get_named_property, (napi_env, napi_value, const char*, napi_value*));
    MOCK_METHOD(napi_status, napi_send_event, (napi_env, const std::function<void()>, napi_event_priority));
    MOCK_METHOD(napi_status, napi_get_value_int32, (napi_env, napi_value, int32_t*));
    MOCK_METHOD(napi_status, napi_get_value_int64, (napi_env, napi_value, int64_t*));
    MOCK_METHOD(napi_status, napi_create_int64, (napi_env, int64_t, napi_value*));
    MOCK_METHOD(napi_status, napi_create_string_utf8, (napi_env, const char*, size_t, napi_value*));
    MOCK_METHOD(napi_status, napi_create_int32, (napi_env, int32_t, napi_value*));
    MOCK_METHOD(napi_status, napi_get_value_string_utf8, (napi_env, napi_value, char*, size_t, size_t*));
    MOCK_METHOD(napi_status, napi_get_boolean, (napi_env, bool, napi_value*));
    MOCK_METHOD(napi_status, napi_create_array, (napi_env, napi_value*));
    MOCK_METHOD(napi_status, napi_get_array_length, (napi_env, napi_value, uint32_t*));
    MOCK_METHOD(napi_status, napi_get_element, (napi_env, napi_value, uint32_t, napi_value*));
    MOCK_METHOD(napi_status, napi_escape_handle, (napi_env, napi_escapable_handle_scope, napi_value, napi_value*));
    MOCK_METHOD(napi_status, napi_get_null, (napi_env, napi_value*));
    MOCK_METHOD(napi_status, napi_create_object, (napi_env, napi_value*));
    MOCK_METHOD(napi_status, napi_create_array_with_length, (napi_env, size_t, napi_value*));
    MOCK_METHOD(napi_status, napi_create_double, (napi_env, double, napi_value*));
    MOCK_METHOD(napi_status, napi_set_named_property, (napi_env, napi_value, const char*, napi_value));
    MOCK_METHOD(napi_status, napi_get_and_clear_last_exception, (napi_env, napi_value*));
    MOCK_METHOD(napi_status, napi_get_cb_info, (napi_env, napi_callback_info, size_t*, napi_value*, napi_value*,
        void**));
    MOCK_METHOD(napi_status, napi_is_promise, (napi_env, napi_value, bool*));
    MOCK_METHOD(napi_status, napi_is_callable, (napi_env, napi_value, bool*));
    MOCK_METHOD(napi_status, napi_create_function, (napi_env, const char*, size_t, napi_callback, void*, napi_value*));
    MOCK_METHOD(napi_status, napi_open_handle_scope, (napi_env, napi_handle_scope*));
    MOCK_METHOD(napi_status, napi_close_handle_scope, (napi_env, napi_handle_scope));
    MOCK_METHOD(napi_status, napi_is_exception_pending, (napi_env, bool*));
    MOCK_METHOD(napi_status, napi_fatal_exception, (napi_env, napi_value));
};
} // namespace OHOS::FileManagement::Backup
#endif // TEST_UNITTEST_MOCK_NAPI_H