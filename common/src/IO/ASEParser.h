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

#ifndef TRENCHBROOM_ASEPARSER_H
#define TRENCHBROOM_ASEPARSER_H

#include "IO/EntityModelParser.h"
#include "IO/Parser.h"
#include "IO/Token.h"
#include "IO/Tokenizer.h"

#include <vecmath/forward.h>

#include <vector>

namespace TrenchBroom {
    class Logger;

    namespace Assets {
        class EntityModel;
    }

    namespace IO {
        namespace AseToken {
            typedef unsigned int Type;
            static const Type Directive         = 1 <<  0; // Any directive, i.e. *SCENE
            static const Type OBrace            = 1 <<  1; // opening brace: {
            static const Type CBrace            = 1 <<  2; // closing brace: }
            static const Type String            = 1 <<  3; // quoted string: "..."
            static const Type Integer           = 1 <<  4; // integer number
            static const Type Decimal           = 1 <<  5; // decimal number
            static const Type Keyword           = 1 <<  6; // keyword: Filter etc.
            static const Type Eof               = 1 << 12; // end of file
        }

        class AseTokenizer : public Tokenizer<AseToken::Type> {
        private:
            static const String WordDelims;
        public:
            AseTokenizer(const char* begin, const char* end);
            explicit AseTokenizer(const String& str);
        private:
            Token emitToken() override;
        };

        class ASEParser : public EntityModelParser, private Parser<AseToken::Type> {
        private:
            using Token = AseTokenizer::Token;

            AseTokenizer m_tokenizer;
        private:
            Assets::EntityModel* doParseModel(Logger& logger) override;

            void parseAseFile(Logger& logger);

            // SCENE
            void parseScene(Logger& logger);

            // MATERIALS
            void parseMaterialList(Logger& logger);
            void parseMaterialListMaterialCount(Logger& logger);
            void parseMaterialListMaterial(Logger& logger);
            void parseMaterialListMaterialMapDiffuse(Logger& logger);
            void parseMaterialListMaterialMapDiffuseBitmap(Logger& logger);

            void parseGeomObject(Logger& logger);
            void parseGeomObjectNodeName(Logger& logger);
            void parseGeomObjectMesh(Logger& logger);
            void parseGeomObjectMeshNumVertex(Logger& logger, std::vector<vm::vec3f>& vertices);
            void parseGeomObjectMeshVertexList(Logger& logger, std::vector<vm::vec3f>& vertices);
            void parseGeomObjectMeshNumFaces(Logger& logger);
            void parseGeomObjectMeshFaceList(Logger& logger);
            void parseGeomObjectMeshNumTVertex(Logger& logger);
            void parseGeomObjectMeshTVertexList(Logger& logger);
            void parseGeomObjectMeshNumTVFaces(Logger& logger);
            void parseGeomObjectMeshTFaceList(Logger& logger);

            void parseBlock(const std::map<std::string, std::function<void(void)>>& handlers);

            void expectDirective(const String& name);
            void skipDirective();

            void expectSizeArgument(size_t expected);
            size_t parseSizeArgument();
        };
    }
}


#endif //TRENCHBROOM_ASEPARSER_H
