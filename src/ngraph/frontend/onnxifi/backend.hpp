//*****************************************************************************
// Copyright 2017-2018 Intel Corporation
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
//*****************************************************************************

#pragma once

#include <map>     // std::map
#include <memory>  // std::shared_ptr
#include <mutex>   // std::mutex
#include <string>  // std::string
#include <utility> // std::move
#include <vector>  // std::vector

#include <onnxifi.h>

#include "ngraph/function.hpp"
#include "ngraph/runtime/backend.hpp"
#include "ngraph/runtime/tensor_view.hpp"

#include "exceptions.hpp"
#include "graph.hpp"

namespace ngraph
{
    namespace onnxifi
    {
        /// \brief ONNXIFI extensions to nGraph backend
        class Backend
        {
        public:
            Backend(const Backend&) = delete;
            Backend& operator=(const Backend&) = delete;

            Backend(Backend&& other) noexcept
            {
                std::unique_lock<std::mutex> lock{other.m_mutex};
                m_handle = other.m_handle;
                m_functions = std::move(other.m_functions);
                m_type = std::move(other.m_type);
                m_backend = std::move(other.m_backend);
            }

            Backend& operator=(Backend&& other) noexcept
            {
                if (&other != this)
                {
                    std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};
                    std::unique_lock<std::mutex> other_lock{other.m_mutex, std::defer_lock};
                    std::lock(lock, other_lock);
                    m_handle = other.m_handle;
                    m_functions = std::move(other.m_functions);
                    m_type = std::move(other.m_type);
                    m_backend = std::move(other.m_backend);
                }
                return *this;
            }

            Backend() = delete;

            explicit Backend(std::string type)
                : m_type{std::move(type)}
            {
            }

            bool compile(::onnxGraph graph) const
            {
                std::lock_guard<std::mutex> lock{m_mutex};
                const auto& fn = m_functions.at(graph);
                return get().compile(fn->get_ng_function());
            }

            bool call(::onnxGraph graph) const
            {
                std::lock_guard<std::mutex> lock{m_mutex};
                const auto& fn = m_functions.at(graph);
                return get().call_with_validate(fn->get_ng_function(), fn->get_outputs(), fn->get_outputs());
            }

            ::onnxBackend init_handle()
            {
                std::lock_guard<std::mutex> lock{m_mutex};
                if (m_backend == nullptr)
                {
                    m_handle = reinterpret_cast<::onnxBackend>(
                        (m_backend = runtime::Backend::create(m_type)).get());
                }
                return m_handle;
            }

            ::onnxGraph init_graph(const void* onnx_model, std::size_t onnx_model_size);

            ::onnxBackend get_handle() const
            {
                std::lock_guard<std::mutex> lock{m_mutex};
                return m_handle;
            }

            bool operator==(const Backend& other) const noexcept
            {
                std::unique_lock<std::mutex> lock{m_mutex, std::defer_lock};
                std::unique_lock<std::mutex> other_lock{other.m_mutex, std::defer_lock};
                std::lock(lock, other_lock);
                return (m_backend.get() == other.m_backend.get());
            }

            bool operator!=(const Backend& other) const noexcept { return !(other == *this); }
            bool operator==(::onnxBackend other) const noexcept
            {
                std::lock_guard<std::mutex> lock{m_mutex};
                return (reinterpret_cast<::onnxBackend>(m_backend.get()) == other);
            }

        private:
            mutable std::mutex m_mutex{};
            std::string m_type{};
            std::shared_ptr<runtime::Backend> m_backend{nullptr};
            ::onnxBackend m_handle{nullptr};
            std::map<::onnxGraph, std::unique_ptr<Graph>> m_functions{};

            runtime::Backend& get() const
            {
                if (m_backend == nullptr)
                {
                    throw status::backend_unavailable{};
                }
                return *m_backend;
            }
        };

    } // namespace onnxifi

} // namespace ngraph
