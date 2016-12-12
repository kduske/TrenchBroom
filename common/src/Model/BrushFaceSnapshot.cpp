/*
 Copyright (C) 2010-2016 Kristian Duske
 
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

#include "BrushFaceSnapshot.h"

#include "Model/Brush.h"

namespace TrenchBroom {
    namespace Model {
        BrushFaceSnapshot::BrushFaceSnapshot(BrushFace* face, TexCoordSystem* coordSystem) :
        m_brush(face->brush()),
        m_faceBoundary(face->boundary()),
        m_attribs(face->attribs().takeSnapshot()),
        m_coordSystemSnapshot(coordSystem->takeSnapshot()) {
            ensure(m_brush != nullptr, "need a brush to snapshot a face");
        }
        
        BrushFaceSnapshot::~BrushFaceSnapshot() {
            delete m_coordSystemSnapshot;
            m_coordSystemSnapshot = NULL;
        }

        void BrushFaceSnapshot::restore() {
            for (BrushFace* face : m_brush->faces()) {
                if (face->boundary() == m_faceBoundary) {
                    face->setAttribs(m_attribs);
                    if (m_coordSystemSnapshot != NULL)
                        face->restoreTexCoordSystemSnapshot(m_coordSystemSnapshot);
                    return;
                }
            }
            ensure(false, "couldn't find face");
        }
    }
}
