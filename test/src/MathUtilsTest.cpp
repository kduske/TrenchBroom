/*
Copyright (C) 2010-2017 Kristian Duske

This file is part of TrenchBroom.

TrenchBroom is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TrenchBroom is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtest/gtest.h>

#include "utils.h"

#include <cstdint>

TEST(MathUtilsTest, findHighestOrderBit) {
    ASSERT_EQ(sizeof(unsigned)*8, vm::findHighestOrderBit(0u));
    ASSERT_EQ(sizeof(unsigned)*8, vm::findHighestOrderBit(0u, 2));
    
    for (size_t i = 0; i < sizeof(uint64_t)*8; ++i) {
        ASSERT_EQ(i, vm::findHighestOrderBit(static_cast<uint64_t>(1u) << i));
    }
    
    for (size_t i = 0; i < sizeof(uint64_t)*8; ++i) {
        const uint64_t test = static_cast<uint64_t>(1u) | (static_cast<uint64_t>(1u) << i);
        ASSERT_EQ(i, vm::findHighestOrderBit(test));
    }
    
    for (size_t i = 0; i < sizeof(uint64_t)*8-1; ++i) {
        const uint64_t test = (static_cast<uint64_t>(1u) << 63) | (static_cast<uint64_t>(1u) << i);
        ASSERT_EQ(63u, vm::findHighestOrderBit(test));
    }

    for (size_t i = 0; i < sizeof(uint64_t)*8; ++i) {
        for (size_t j = 0; j < sizeof(uint64_t) * 8; ++j) {
            const size_t exp = i <= j ? i : sizeof(uint64_t)*8;
            ASSERT_EQ(exp, vm::findHighestOrderBit(static_cast<uint64_t>(1u) << i, j));
        }
    }
}
