//*****************************************************************************
// Copyright 2017-2020 Intel Corporation
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

#include "ngraph/node.hpp"
#include "ngraph/op/op.hpp"
#include "ngraph/op/util/fused_op.hpp"

namespace ngraph
{
    namespace op
    {
        namespace v0
        {
            class NGRAPH_API CrossEntropy2 : public ngraph::op::util::FusedOp
            {
            public:
                static constexpr NodeTypeInfo type_info{"CrossEntropy2", 0};
                const NodeTypeInfo& get_type_info() const override { return type_info; }
                CrossEntropy2() = default;
                /// \brief CrossEntropy2: is a measure of the difference between two probability
                /// distributions for a given random variable or set of events
                /// \param arg1 Node that produces the input tensor
                /// \param arg2 Node that produces ground truth lables for the input
                /// \param soft_label flag indicating whether to interpretate the given labels as
                /// soft
                /// labels
                /// \param ignore_index Specifies a target value that is ignored and does not
                /// contribute
                /// to the input gradient Only valid if soft_label is set to False
                CrossEntropy2(const Output<Node>& arg1,
                              const Output<Node>& arg2,
                              bool soft_label = false,
                              int64_t ignore_index = -100);

                virtual NodeVector decompose_op() const override;

                virtual void pre_validate_and_infer_types() override;

                virtual std::shared_ptr<Node>
                    copy_with_new_args(const NodeVector& new_args) const override;

                bool get_soft_label() const { return m_soft_label; }
                int64_t get_ignore_index() const { return m_ignore_index; }
            private:
                bool m_soft_label;
                int64_t m_ignore_index;
            };

            class CrossEntropy2Backprop : public util::FusedOp
            {
            public:
                NGRAPH_API
                static constexpr NodeTypeInfo type_info{"CrossEntropy2Backprop", 0};
                const NodeTypeInfo& get_type_info() const override { return type_info; }
                CrossEntropy2Backprop() = default;

                /// \brief Backprop for CrossEntropy2
                /// \param input Node that produces tensor from the fprop
                /// \param labels Node that produces ground truth labels for input
                /// \param delta Node that produces the delta during bprop
                /// \param soft_label flag indicating whether to interpretate the given labels as
                /// soft
                /// labels
                /// \param ignore_index Specifies a target value that is ignored and does not
                /// contribute
                /// to the input gradient Only valid if soft_label is set to False
                CrossEntropy2Backprop(const Output<Node>& input,
                                      const Output<Node>& labels,
                                      const Output<Node>& x,
                                      const Output<Node>& dy,
                                      bool soft_label = false,
                                      int64_t ignore_index = -100);

                virtual NodeVector decompose_op() const override;

                virtual void pre_validate_and_infer_types() override;

                virtual std::shared_ptr<Node>
                    copy_with_new_args(const NodeVector& new_args) const override;
                bool get_soft_label() const { return m_soft_label; }
                int64_t get_ignore_index() const { return m_ignore_index; }
            private:
                bool m_soft_label;
                int64_t m_ignore_index;
            };
        }
        using v0::CrossEntropy2;
        using v0::CrossEntropy2Backprop;
    } // namespace op
} // namespace ngraph
