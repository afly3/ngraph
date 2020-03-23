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

#include <algorithm>
#include <iostream>
#include <limits>
#include <sstream>

#include "ngraph/dimension.hpp"

using namespace ngraph;

Dimension::Dimension(int64_t dimension)
    : m_dimension(dimension)
{
    if (dimension == s_dynamic_val)
    {
        std::stringstream ss;
        ss << "Cannot convert the value 0x" << std::uppercase << std::hex << s_dynamic_val
           << " to Dimension: this value is used internally to represent a dynamic "
              "dimension.";
        throw std::invalid_argument(ss.str());
    }
}

std::ostream& ngraph::operator<<(std::ostream& str, const Dimension& dimension)
{
    if (dimension.is_static())
    {
        return (str << int64_t(dimension));
    }
    else
    {
        return (str << "?");
    }
}

Dimension Dimension::operator+(const Dimension& dim) const
{
    return (is_static() && dim.is_static() ? m_dimension + int64_t(dim) : Dimension::dynamic());
}

Dimension Dimension::operator-(const Dimension& dim) const
{
    return (is_static() && dim.is_static() ? m_dimension - int64_t(dim) : Dimension::dynamic());
}

Dimension Dimension::operator*(const Dimension& dim) const
{
    return ((is_static() && dim.is_static())
                ? m_dimension * int64_t(dim)
                : (is_static() && m_dimension == 0)
                      ? 0
                      : (dim.is_static() && int64_t(dim) == 0) ? 0 : Dimension::dynamic());
}

bool Dimension::compatible(const Dimension& d) const
{
    return (is_dynamic() || d.is_dynamic() || m_dimension == int64_t(d));
}

bool Dimension::relaxes(const Dimension& d) const
{
    return (is_dynamic() || (d.is_static() && int64_t(*this) == int64_t(d)));
}

bool Dimension::refines(const Dimension& d) const
{
    return (d.is_dynamic() || (is_static() && int64_t(d) == int64_t(*this)));
}

bool Dimension::merge(Dimension& dst, const Dimension d1, const Dimension d2)
{
    if (d1.is_dynamic())
    {
        dst = d2;
        return true;
    }
    else if (d2.is_dynamic())
    {
        dst = d1;
        return true;
    }
    else if (int64_t(d1) != int64_t(d2))
    {
        return false;
    }
    else
    {
        dst = d1;
        return true;
    }
}

bool Dimension::broadcast_merge(Dimension& dst, const Dimension d1, const Dimension d2)
{
    if (d1.is_dynamic() && d2.is_dynamic())
    {
        dst = d1;
        return true;
    }
    else if (d1.is_dynamic() || d2.is_dynamic())
    {
        // One static. Set dst to static size if >1
        auto ds = d1.is_dynamic() ? int64_t(d2) : int64_t(d1);
        dst = (ds > 1) ? ds : Dimension::dynamic();
        return true;
    }
    else
    {
        // Static sizes. Both match or one of them is 1.
        if (int64_t(d1) == int64_t(d2) || int64_t(d1) == 1 || int64_t(d2) == 1)
        {
            dst = std::max(int64_t(d1), int64_t(d2));
            return true;
        }
        else
        {
            return false;
        }
    }
}

uint64_t Dimension::get_length() const
{
    if (is_dynamic())
    {
        throw std::invalid_argument("Cannot get length of dynamic dimension");
    }
    if (m_dimension < 0)
    {
        throw std::invalid_argument("Cannot get_length of negative dimension");
    }
    return m_dimension;
}
