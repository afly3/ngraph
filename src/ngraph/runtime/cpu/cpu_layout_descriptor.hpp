/*******************************************************************************
* Copyright 2017-2018 Intel Corporation
*
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
*******************************************************************************/

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <mkldnn.hpp>

#include "ngraph/axis_vector.hpp"
#include "ngraph/descriptor/layout/tensor_view_layout.hpp"
#include "ngraph/shape.hpp"
#include "ngraph/type/type.hpp"

namespace ngraph
{
    namespace runtime
    {
        namespace cpu
        {
            class LayoutDescriptor : public ngraph::descriptor::layout::TensorViewLayout
            {
            public:
                LayoutDescriptor(const ngraph::descriptor::TensorView& tv,
                                 const AxisVector& tv_axis_order);
                ~LayoutDescriptor() override {}
                size_t get_size() override { return m_size; }
                size_t get_offset() const { return m_offset; }
                size_t get_index_offset(const std::vector<size_t>& indices) override;

                const Strides& get_strides() const override { return m_strides; }
                void set_strides(Strides& strides) { m_strides = strides; }
                bool operator==(const TensorViewLayout& other) const override;

                const mkldnn::memory::desc& get_mkldnn_md() const { return m_mkldnn_md; }
                void set_mkldnn_md(const mkldnn::memory::desc md)
                {
                    m_mkldnn_md = md;
                    m_mkldnn_layout = true;
                }
                bool is_mkldnn_layout() const { return m_mkldnn_layout; }
                const AxisVector& get_axis_order() const { return m_axis_order; }
                void set_axis_order(const AxisVector& perm) { m_axis_order = perm; }
                static AxisVector create_native_axis_order(size_t rank);

                static const mkldnn::memory::desc DummyDesc;

            private:
                AxisVector m_axis_order;
                Strides m_strides;
                size_t m_offset;
                size_t m_size;

                bool m_mkldnn_layout;
                mkldnn::memory::desc m_mkldnn_md;
            };

            typedef std::vector<std::shared_ptr<ngraph::runtime::cpu::LayoutDescriptor>>
                LayoutDescriptorPtrs;
        }
    }
}
