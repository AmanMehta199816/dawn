// Copyright 2021 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// This file provides core interop helpers used by the code generated by the
// templates.

#ifndef DAWN_NODE_INTEROP_CORE_WEBGPU_H_
#define DAWN_NODE_INTEROP_CORE_WEBGPU_H_

#include <cstdint>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

#include "napi.h"

#include "src/dawn_node/utils/Debug.h"

#define ENABLE_INTEROP_LOGGING 0  // Enable for verbose interop logging

#if ENABLE_INTEROP_LOGGING
#    define INTEROP_LOG(...) LOG(__VA_ARGS__)
#else
#    define INTEROP_LOG(...)
#endif

namespace wgpu { namespace interop {

    ////////////////////////////////////////////////////////////////////////////////
    // Primitive JavaScript types
    ////////////////////////////////////////////////////////////////////////////////
    using Object = Napi::Object;
    using ArrayBuffer = Napi::ArrayBuffer;
    using Int8Array = Napi::TypedArrayOf<int8_t>;
    using Int16Array = Napi::TypedArrayOf<int16_t>;
    using Int32Array = Napi::TypedArrayOf<int32_t>;
    using Uint8Array = Napi::TypedArrayOf<uint8_t>;
    using Uint16Array = Napi::TypedArrayOf<uint16_t>;
    using Uint32Array = Napi::TypedArrayOf<uint32_t>;
    using Float32Array = Napi::TypedArrayOf<float>;
    using Float64Array = Napi::TypedArrayOf<double>;
    using DataView = Napi::TypedArray;

    template <typename T>
    using FrozenArray = std::vector<T>;

    ////////////////////////////////////////////////////////////////////////////////
    // Interface<T>
    ////////////////////////////////////////////////////////////////////////////////

    // Interface<T> is a templated wrapper around a JavaScript object, which
    // implements the template-generated interface type T. Interfaces are returned
    // by either calling T::Bind() or T::Create().
    template <typename T>
    class Interface {
      public:
        // Constructs an Interface with no JS object.
        inline Interface() {
        }

        // Constructs an Interface wrapping the given JS object.
        // The JS object must have been created with a call to T::Bind().
        explicit inline Interface(Napi::Object o) : object(o) {
        }

        // Implicit conversion operators to Napi objects.
        inline operator napi_value() const {
            return object;
        }
        inline operator const Napi::Value &() const {
            return object;
        }
        inline operator const Napi::Object &() const {
            return object;
        }

        // Member and dereference operators
        inline T* operator->() const {
            return T::Unwrap(object);
        }
        inline T* operator*() const {
            return T::Unwrap(object);
        }

        // As<IMPL>() returns the unwrapped object cast to the implementation type.
        // The interface implementation *must* be of the template type IMPL.
        template <typename IMPL>
        inline IMPL* As() const {
            return static_cast<IMPL*>(T::Unwrap(object));
        }

      private:
        Napi::Object object;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Promise<T>
    ////////////////////////////////////////////////////////////////////////////////

    // Promise<T> is a templated wrapper around a JavaScript promise, which can
    // resolve to the template type T.
    template <typename T>
    class Promise {
      public:
        // Constructor
        Promise(Napi::Env env) : deferred(Napi::Promise::Deferred::New(env)) {
        }

        // Implicit conversion operators to Napi promises.
        inline operator napi_value() const {
            return deferred.Promise();
        }
        inline operator Napi::Value() const {
            return deferred.Promise();
        }
        inline operator Napi::Promise() const {
            return deferred.Promise();
        }

        // Resolve() fulfills the promise with the given value.
        void Resolve(T&& value) const {
            deferred.Resolve(ToJS(deferred.Env(), std::forward<T>(value)));
        }

        // Reject() rejects the promise with the given failure value.
        void Reject(Napi::Object obj) const {
            deferred.Reject(obj);
        }
        void Reject(Napi::Error err) const {
            deferred.Reject(err.Value());
        }
        void Reject(std::string err) const {
            Reject(Napi::Error::New(deferred.Env(), err));
        }

      private:
        Napi::Promise::Deferred deferred;
    };

    // Specialization for Promises that resolve with no value
    template <>
    class Promise<void> {
      public:
        // Constructor
        Promise(Napi::Env env) : deferred(Napi::Promise::Deferred::New(env)) {
        }

        // Implicit conversion operators to Napi promises.
        inline operator napi_value() const {
            return deferred.Promise();
        }
        inline operator Napi::Value() const {
            return deferred.Promise();
        }
        inline operator Napi::Promise() const {
            return deferred.Promise();
        }

        // Resolve() fulfills the promise.
        void Resolve() const {
            deferred.Resolve(deferred.Env().Undefined());
        }

        // Reject() rejects the promise with the given failure value.
        void Reject(Napi::Object obj) const {
            deferred.Reject(obj);
        }
        void Reject(Napi::Error err) const {
            deferred.Reject(err.Value());
        }
        void Reject(std::string err) const {
            Reject(Napi::Error::New(deferred.Env(), err));
        }

      private:
        Napi::Promise::Deferred deferred;
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Converter<T>
    ////////////////////////////////////////////////////////////////////////////////

    // Converter<T> is specialized for each type T which can be converted from C++
    // to JavaScript, or JavaScript to C++.
    // Each specialization of Converter<T> is expected to have two static methods
    // with the signatures:
    //
    //  // FromJS() converts the JavaScript value 'in' to the C++ value 'out'.
    //  // Returns true on success, false on failure.
    //  static bool FromJS(Napi::Env, Napi::Value in, T& out);
    //
    //  // ToJS() converts the C++ value 'in' to a JavaScript value, and returns
    //  // this value.
    //  static Napi::Value ToJS(Napi::Env, T in);
    template <typename T>
    class Converter {};

    template <>
    class Converter<Napi::Object> {
      public:
        static inline bool FromJS(Napi::Env, Napi::Value value, Napi::Object& out) {
            if (value.IsObject()) {
                out = value.ToObject();
                return true;
            }
            return false;
        }
        static inline Napi::Value ToJS(Napi::Env, Napi::Object value) {
            return value;
        }
    };

    template <>
    class Converter<ArrayBuffer> {
      public:
        static inline bool FromJS(Napi::Env, Napi::Value value, ArrayBuffer& out) {
            if (value.IsArrayBuffer()) {
                out = value.As<ArrayBuffer>();
                return true;
            }
            return false;
        };
        static inline Napi::Value ToJS(Napi::Env, ArrayBuffer value) {
            return value;
        }
    };

    template <>
    class Converter<Napi::TypedArray> {
      public:
        static inline bool FromJS(Napi::Env, Napi::Value value, Napi::TypedArray& out) {
            if (value.IsTypedArray()) {
                out = value.As<Napi::TypedArray>();
                return true;
            }
            return false;
        };
        static inline Napi::Value ToJS(Napi::Env, ArrayBuffer value) {
            return value;
        }
    };

    template <typename T>
    class Converter<Napi::TypedArrayOf<T>> {
      public:
        // clang-format off
        // The Napi element type of T
        static constexpr napi_typedarray_type element_type =
              std::is_same<T, int8_t>::value   ? napi_int8_array
            : std::is_same<T, uint8_t>::value  ? napi_uint8_array
            : std::is_same<T, int16_t>::value  ? napi_int16_array
            : std::is_same<T, uint16_t>::value ? napi_uint16_array
            : std::is_same<T, int32_t>::value  ? napi_int32_array
            : std::is_same<T, uint32_t>::value ? napi_uint32_array
            : std::is_same<T, float>::value    ? napi_float32_array
            : std::is_same<T, double>::value   ? napi_float64_array
            : std::is_same<T, int64_t>::value  ? napi_bigint64_array
            : std::is_same<T, uint64_t>::value ? napi_biguint64_array
            : static_cast<napi_typedarray_type>(-1);
        // clang-format on
        static_assert(static_cast<int>(element_type) >= 0,
                      "unsupported T type for Napi::TypedArrayOf<T>");
        static inline bool FromJS(Napi::Env, Napi::Value value, Napi::TypedArrayOf<T>& out) {
            if (value.IsTypedArray()) {
                auto arr = value.As<Napi::TypedArrayOf<T>>();
                if (arr.TypedArrayType() == element_type) {
                    out = arr;
                    return true;
                }
            }
            return false;
        };
        static inline Napi::Value ToJS(Napi::Env, ArrayBuffer value) {
            return value;
        }
    };

    template <>
    class Converter<std::string> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, std::string&);
        static Napi::Value ToJS(Napi::Env, std::string);
    };

    template <>
    class Converter<bool> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, bool&);
        static Napi::Value ToJS(Napi::Env, bool);
    };

    template <>
    class Converter<int8_t> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, int8_t&);
        static Napi::Value ToJS(Napi::Env, int8_t);
    };

    template <>
    class Converter<uint8_t> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, uint8_t&);
        static Napi::Value ToJS(Napi::Env, uint8_t);
    };

    template <>
    class Converter<int16_t> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, int16_t&);
        static Napi::Value ToJS(Napi::Env, int16_t);
    };

    template <>
    class Converter<uint16_t> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, uint16_t&);
        static Napi::Value ToJS(Napi::Env, uint16_t);
    };

    template <>
    class Converter<int32_t> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, int32_t&);
        static Napi::Value ToJS(Napi::Env, int32_t);
    };

    template <>
    class Converter<uint32_t> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, uint32_t&);
        static Napi::Value ToJS(Napi::Env, uint32_t);
    };

    template <>
    class Converter<int64_t> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, int64_t&);
        static Napi::Value ToJS(Napi::Env, int64_t);
    };

    template <>
    class Converter<uint64_t> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, uint64_t&);
        static Napi::Value ToJS(Napi::Env, uint64_t);
    };

    template <>
    class Converter<float> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, float&);
        static Napi::Value ToJS(Napi::Env, float);
    };

    template <>
    class Converter<double> {
      public:
        static bool FromJS(Napi::Env, Napi::Value, double&);
        static Napi::Value ToJS(Napi::Env, double);
    };

    template <typename T>
    class Converter<Interface<T>> {
      public:
        static bool FromJS(Napi::Env env, Napi::Value value, Interface<T>& out) {
            if (value.IsObject()) {
                auto obj = value.As<Napi::Object>();
                if (T::Unwrap(obj)) {
                    out = Interface<T>(obj);
                    return true;
                }
            }
            return false;
        }
        static Napi::Value ToJS(Napi::Env env, const Interface<T>& value) {
            return {env, value};
        }
    };

    template <typename T>
    class Converter<std::optional<T>> {
      public:
        static bool FromJS(Napi::Env env, Napi::Value value, std::optional<T>& out) {
            if (value.IsNull() || value.IsUndefined()) {
                out.reset();
                return true;
            }
            T v{};
            if (!Converter<T>::FromJS(env, value, v)) {
                return false;
            }
            out = std::move(v);
            return true;
        }
        static Napi::Value ToJS(Napi::Env env, std::optional<T> value) {
            if (value.has_value()) {
                return Converter<T>::ToJS(env, value.value());
            }
            return env.Null();
        }
    };

    template <typename T>
    class Converter<std::vector<T>> {
      public:
        static inline bool FromJS(Napi::Env env, Napi::Value value, std::vector<T>& out) {
            if (!value.IsArray()) {
                return false;
            }
            auto arr = value.As<Napi::Array>();
            std::vector<T> vec(arr.Length());
            for (size_t i = 0; i < vec.size(); i++) {
                if (!Converter<T>::FromJS(env, arr[static_cast<uint32_t>(i)], vec[i])) {
                    return false;
                }
            }
            out = std::move(vec);
            return true;
        }
        static inline Napi::Value ToJS(Napi::Env env, const std::vector<T>& vec) {
            auto arr = Napi::Array::New(env, vec.size());
            for (size_t i = 0; i < vec.size(); i++) {
                arr.Set(static_cast<uint32_t>(i), Converter<T>::ToJS(env, vec[i]));
            }
            return arr;
        }
    };

    template <typename K, typename V>
    class Converter<std::unordered_map<K, V>> {
      public:
        static inline bool FromJS(Napi::Env env, Napi::Value value, std::unordered_map<K, V>& out) {
            if (!value.IsObject()) {
                return false;
            }
            auto obj = value.ToObject();
            auto keys = obj.GetPropertyNames();
            std::unordered_map<K, V> map(keys.Length());
            for (uint32_t i = 0; i < static_cast<uint32_t>(map.size()); i++) {
                K key{};
                V value{};
                if (!Converter<K>::FromJS(env, keys[i], key) ||
                    !Converter<V>::FromJS(env, obj.Get(keys[i]), value)) {
                    return false;
                }
                map[key] = value;
            }
            out = std::move(map);
            return true;
        }
        static inline Napi::Value ToJS(Napi::Env env, std::unordered_map<K, V> value) {
            auto obj = Napi::Object::New(env);
            for (auto it : value) {
                obj.Set(Converter<K>::ToJS(env, it.first), Converter<V>::ToJS(env, it.second));
            }
            return obj;
        }
    };

    template <typename... TYPES>
    class Converter<std::variant<TYPES...>> {
        template <typename TY>
        static inline bool TryFromJS(Napi::Env env,
                                     Napi::Value value,
                                     std::variant<TYPES...>& out) {
            TY v{};
            if (Converter<TY>::FromJS(env, value, v)) {
                out = std::move(v);
                return true;
            }
            return false;
        }

        template <typename T0, typename T1, typename... TN>
        static inline bool TryFromJS(Napi::Env env,
                                     Napi::Value value,
                                     std::variant<TYPES...>& out) {
            if (TryFromJS<T0>(env, value, out)) {
                return true;
            }
            return TryFromJS<T1, TN...>(env, value, out);
        }

      public:
        static inline bool FromJS(Napi::Env env, Napi::Value value, std::variant<TYPES...>& out) {
            return TryFromJS<TYPES...>(env, value, out);
        }
        static inline Napi::Value ToJS(Napi::Env env, std::variant<TYPES...> value) {
            return std::visit(
                [&](auto&& v) {
                    using T = std::remove_cv_t<std::remove_reference_t<decltype(v)>>;
                    return Converter<T>::ToJS(env, v);
                },
                value);
        }
    };

    template <typename T>
    class Converter<Promise<T>> {
      public:
        static inline bool FromJS(Napi::Env, Napi::Value, Promise<T>&) {
            UNIMPLEMENTED();
        }
        static inline Napi::Value ToJS(Napi::Env, Promise<T> promise) {
            return promise;
        }
    };

    ////////////////////////////////////////////////////////////////////////////////
    // Helpers
    ////////////////////////////////////////////////////////////////////////////////

    // FromJS() is a helper function which delegates to
    // Converter<T>::FromJS()
    template <typename T>
    inline bool FromJS(Napi::Env env, Napi::Value value, T& out) {
        return Converter<T>::FromJS(env, value, out);
    }

    // FromJSOptional() is similar to FromJS(), but if 'value' is either null
    // or undefined then FromJSOptional() returns true and 'out' is left
    // unassigned.
    // Returns true on success, false on failure.
    template <typename T>
    inline bool FromJSOptional(Napi::Env env, Napi::Value value, T& out) {
        if (value.IsNull() || value.IsUndefined()) {
            return true;
        }
        return Converter<T>::FromJS(env, value, out);
    }

    // ToJS() is a helper function which delegates to Converter<T>::ToJS()
    template <typename T>
    inline Napi::Value ToJS(Napi::Env env, T&& value) {
        return Converter<std::remove_cv_t<std::remove_reference_t<T>>>::ToJS(
            env, std::forward<T>(value));
    }

    // FromJS() is a helper function for bulk converting the arguments of 'info'.
    // PARAM_TYPES is a std::tuple<> describing the C++ function parameter types.
    // Returns true on success, false on failure.
    template <typename PARAM_TYPES, int BASE_INDEX = 0>
    inline bool FromJS(const Napi::CallbackInfo& info, PARAM_TYPES& args) {
        if constexpr (BASE_INDEX < std::tuple_size_v<PARAM_TYPES>) {
            using T = std::tuple_element_t<BASE_INDEX, PARAM_TYPES>;
            if (!FromJS<T>(info.Env(), info[BASE_INDEX], std::get<BASE_INDEX>(args))) {
                return false;
            }
            return FromJS<PARAM_TYPES, BASE_INDEX + 1>(info, args);
        } else {
            return true;
        }
    }

}}  // namespace wgpu::interop

#endif  //  DAWN_NODE_INTEROP_CORE_WEBGPU_H_
