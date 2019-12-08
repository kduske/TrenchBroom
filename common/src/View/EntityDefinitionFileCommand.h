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

#ifndef TrenchBroom_EntityDefinitionFileCommand
#define TrenchBroom_EntityDefinitionFileCommand

#include "Assets/EntityDefinitionFileSpec.h"
#include "View/DocumentCommand.h"

#include <string>

namespace TrenchBroom {
    namespace View {
        class EntityDefinitionFileCommand : public DocumentCommand {
        public:
            static const CommandType Type;
            using Ptr = std::shared_ptr<EntityDefinitionFileCommand>;
        private:
            Assets::EntityDefinitionFileSpec m_oldSpec;
            Assets::EntityDefinitionFileSpec m_newSpec;
        public:
            static Ptr set(const Assets::EntityDefinitionFileSpec& spec);
        private:
            EntityDefinitionFileCommand(const std::string& name, const Assets::EntityDefinitionFileSpec& spec);

            bool doPerformDo(MapDocumentCommandFacade* document) override;
            bool doPerformUndo(MapDocumentCommandFacade* document) override;

            bool doIsRepeatable(MapDocumentCommandFacade* document) const override;
            bool doCollateWith(UndoableCommand::Ptr command) override;
        };
    }
}

#endif /* defined(TrenchBroom_EntityDefinitionFileCommand) */
