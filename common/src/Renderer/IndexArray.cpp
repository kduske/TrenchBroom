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

#include "IndexArray.h"

namespace TrenchBroom {
    namespace Renderer {
        void IndexArray::BaseHolder::render(const PrimType primType, const size_t offset, const size_t count) const {
            doRender(primType, offset, count);
        }

        IndexArray::IndexArray() :
        m_prepared(false) {}

        bool IndexArray::empty() const {
            return indexCount() == 0;
        }

        size_t IndexArray::sizeInBytes() const {
            return m_holder.get() == nullptr ? 0 : m_holder->sizeInBytes();
        }

        size_t IndexArray::indexCount() const {
            return m_holder.get() == nullptr ? 0 : m_holder->indexCount();
        }

        bool IndexArray::prepared() const {
            return m_prepared;
        }

        void IndexArray::prepare(VboManager& vboManager) {
            if (!prepared() && !empty())
                m_holder->prepare(vboManager);
            m_prepared = true;
        }

        void IndexArray::render(const PrimType primType, const size_t offset, size_t count) const {
            assert(prepared());
            if (!empty())
                m_holder->render(primType, offset, count);
        }

        IndexArray::IndexArray(BaseHolder::Ptr holder) :
        m_holder(holder),
        m_prepared(false) {}
    }
}
