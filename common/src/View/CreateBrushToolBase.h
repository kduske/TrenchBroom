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

#ifndef TrenchBroom_CreateBrushToolBase
#define TrenchBroom_CreateBrushToolBase

#include "Model/Model_Forward.h"
#include "Renderer/Renderer_Forward.h"
#include "View/Tool.h"
#include "View/View_Forward.h"

#include <memory>

namespace TrenchBroom {
    namespace View {
        class CreateBrushToolBase : public Tool {
        protected:
            std::weak_ptr<MapDocument> m_document;
        private:
            Model::Brush* m_brush;
            Renderer::BrushRenderer* m_brushRenderer;
        public:
            CreateBrushToolBase(bool initiallyActive, std::weak_ptr<MapDocument> document);
            ~CreateBrushToolBase() override;
        public:
            const Grid& grid() const;

            void createBrush();
            void cancel();

            void render(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch);
        private:
            void renderBrush(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch);
        protected:
            void updateBrush(Model::Brush* brush);
        private:
            virtual void doBrushWasCreated();
        };
    }
}

#endif /* defined(TrenchBroom_CreateBrushToolBase) */
