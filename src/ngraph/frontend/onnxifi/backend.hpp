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

#include <memory>  // std::shared_ptr
#include <string>  // std::string
#include <utility> // std::move
#include <vector>  // std::vector

#include "ngraph/function.hpp"
#include "ngraph/runtime/backend.hpp"
#include "ngraph/runtime/tensor_view.hpp"

namespace ngraph
{
    namespace onnxifi
    {
        class Backend
        {
        public:
            Backend(const Backend&) = delete;
            Backend& operator=(const Backend&) = delete;

            Backend(Backend&&) = default;
            Backend& operator=(Backend&&) = default;

            Backend() = delete;

            explicit Backend(std::string type)
                : m_type{std::move(type)}
            {
            }

            bool compile(const std::shared_ptr<Function>& function) const
            {
                return get().compile(function);
            }

            bool call(const std::shared_ptr<Function>& function,
                      const std::vector<std::shared_ptr<runtime::TensorView>>& outputs,
                      const std::vector<std::shared_ptr<runtime::TensorView>>& inputs) const
            {
                return get().call(function, outputs, inputs);
            }

            bool call_with_validate(
                const std::shared_ptr<Function>& function,
                const std::vector<std::shared_ptr<runtime::TensorView>>& outputs,
                const std::vector<std::shared_ptr<runtime::TensorView>>& inputs) const
            {
                return get().call_with_validate(function, outputs, inputs);
            }

            void get_onnxifi_version(void* infoValue, std::size_t* infoValueSize) const;
            void get_name(void* infoValue, std::size_t* infoValueSize) const;
            void get_vendor(void* infoValue, std::size_t* infoValueSize) const;
            void get_version(void* infoValue, std::size_t* infoValueSize) const;
            void get_extensions(void* infoValue, std::size_t* infoValueSize) const;
            void get_device(void* infoValue, std::size_t* infoValueSize) const;
            void get_device_type(void* infoValue, std::size_t* infoValueSize) const;
            void get_onnx_ir_version(void* infoValue, std::size_t* infoValueSize) const;
            void get_opset_version(void* infoValue, std::size_t* infoValueSize) const;
            void get_capabilities(void* infoValue, std::size_t* infoValueSize) const;
            void get_init_properties(void* infoValue, std::size_t* infoValueSize) const;
            void get_memory_types(void* infoValue, std::size_t* infoValueSize) const;
            void get_graph_init_properties(void* infoValue, std::size_t* infoValueSize) const;
            void get_synchronization_types(void* infoValue, std::size_t* infoValueSize) const;
            void get_memory_size(void* infoValue, std::size_t* infoValueSize) const;
            void get_max_graph_size(void* infoValue, std::size_t* infoValueSize) const;
            void get_max_graph_count(void* infoValue, std::size_t* infoValueSize) const;

        private:
            std::string m_type{};
            mutable std::shared_ptr<runtime::Backend> m_backend{nullptr};

            runtime::Backend& get() const
            {
                if (m_backend == nullptr)
                {
                    m_backend = runtime::Backend::create(m_type);
                }
                return *m_backend;
            }
        };

    } // namespace onnxifi

} // namespace ngraph
