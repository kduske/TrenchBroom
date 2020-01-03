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

#ifndef TrenchBroom_EntityDefinitionLoader
#define TrenchBroom_EntityDefinitionLoader

#include "IO/IO_Forward.h"

#include <vector>

namespace TrenchBroom {
    namespace Assets {
        class EntityDefinition;
    }

    namespace IO {
        class EntityDefinitionLoader {
        public:
            virtual ~EntityDefinitionLoader();
            std::vector<Assets::EntityDefinition*> loadEntityDefinitions(ParserStatus& status, const IO::Path& path) const;
        private:
            virtual std::vector<Assets::EntityDefinition*> doLoadEntityDefinitions(ParserStatus& status, const IO::Path& path) const = 0;
        };
    }
}

#endif /* defined(TrenchBroom_EntityDefinitionLoader) */
