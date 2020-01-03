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

#ifndef TrenchBroom_CopyTexCoordSystemFromFaceCommand
#define TrenchBroom_CopyTexCoordSystemFromFaceCommand

#include "Macros.h"
#include "TrenchBroom.h"
#include "View/DocumentCommand.h"
#include "Model/Model_Forward.h"
#include "Model/BrushFaceAttributes.h"
#include "View/View_Forward.h"

#include <vecmath/plane.h>

#include <memory>

namespace TrenchBroom {
    namespace View {
        class CopyTexCoordSystemFromFaceCommand : public DocumentCommand {
        public:
            static const CommandType Type;
        private:

            std::unique_ptr<Model::Snapshot> m_snapshot;
            std::unique_ptr<Model::TexCoordSystemSnapshot> m_coordSystemSnapshot;
            const vm::plane3 m_sourceFacePlane;
            const Model::WrapStyle m_wrapStyle;
            const Model::BrushFaceAttributes m_attribs;
        public:
            static std::unique_ptr<CopyTexCoordSystemFromFaceCommand> command(const Model::TexCoordSystemSnapshot& coordSystemSanpshot, const Model::BrushFaceAttributes& attribs, const vm::plane3& sourceFacePlane, const Model::WrapStyle wrapStyle);

            CopyTexCoordSystemFromFaceCommand(const Model::TexCoordSystemSnapshot& coordSystemSanpshot, const Model::BrushFaceAttributes& attribs, const vm::plane3& sourceFacePlane, const Model::WrapStyle wrapStyle);
            ~CopyTexCoordSystemFromFaceCommand() override;
        private:
            std::unique_ptr<CommandResult> doPerformDo(MapDocumentCommandFacade* document) override;
            std::unique_ptr<CommandResult> doPerformUndo(MapDocumentCommandFacade* document) override;

            bool doIsRepeatable(MapDocumentCommandFacade* document) const override;
            std::unique_ptr<UndoableCommand> doRepeat(MapDocumentCommandFacade* document) const override;

            bool doCollateWith(UndoableCommand* command) override;

            deleteCopyAndMove(CopyTexCoordSystemFromFaceCommand)
        };
    }
}

#endif /* defined(TrenchBroom_CopyTexCoordSystemFromFaceCommand) */
