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

#include "WorldReader.h"

#include "IO/ParserStatus.h"
#include "Color.h"
#include "Model/BrushNode.h"
#include "Model/Entity.h"
#include "Model/EntityProperties.h"
#include "Model/LayerNode.h"
#include "Model/LockState.h"
#include "Model/WorldNode.h"
#include "Model/VisibilityState.h"

#include <kdl/vector_set.h>
#include <kdl/string_utils.h>

#include <cassert>
#include <string>

namespace TrenchBroom {
    namespace IO {
        WorldReader::WorldReader(std::string_view str, const Model::MapFormat sourceAndTargetMapFormat) :
        MapReader(std::move(str), sourceAndTargetMapFormat, sourceAndTargetMapFormat),
        m_world(std::make_unique<Model::WorldNode>(Model::Entity(), sourceAndTargetMapFormat)) {
            m_world->disableNodeTreeUpdates();
        }

        std::unique_ptr<Model::WorldNode> WorldReader::read(const vm::bbox3& worldBounds, ParserStatus& status) {
            readEntities(worldBounds, status);
            sanitizeLayerSortIndicies(status);
            m_world->rebuildNodeTree();
            m_world->enableNodeTreeUpdates();
            return std::move(m_world);
        }

        /**
         * Sanitizes the sort indices of custom layers:
         * Ensures there are no duplicates or sort indices less than 0.
         *
         * This will be a no-op on a well-formed map file.
         * If the map was saved without layer indices, the file order is used.
         */
        void WorldReader::sanitizeLayerSortIndicies(ParserStatus& /* status */) {
            std::vector<Model::LayerNode*> customLayers = m_world->customLayers();
            Model::LayerNode::sortLayers(customLayers);

            // Gather the layers whose sort indices are invalid. Visit them in the current sorted order.
            std::vector<Model::LayerNode*> invalidLayers;
            std::vector<Model::LayerNode*> validLayers;
            kdl::vector_set<int> usedIndices;
            for (auto* layerNode : customLayers) {
                // Check for a totally invalid index
                const auto sortIndex = layerNode->layer().sortIndex();
                if (sortIndex < 0  || sortIndex == Model::Layer::invalidSortIndex()) {
                    invalidLayers.push_back(layerNode);
                    continue;
                }

                // Check for an index that has already been used
                const bool wasInserted = usedIndices.insert(sortIndex).second;
                if (!wasInserted) {
                    invalidLayers.push_back(layerNode);
                    continue;
                }

                validLayers.push_back(layerNode);
            }

            assert(invalidLayers.size() + validLayers.size() == customLayers.size());

            // Renumber the invalid layers
            int nextValidLayerIndex = (validLayers.empty() ? 0 : (validLayers.back()->layer().sortIndex() + 1));            
            for (auto* layerNode : invalidLayers) {
                auto layer = layerNode->layer();
                layer.setSortIndex(nextValidLayerIndex++);
                layerNode->setLayer(std::move(layer));
            }
        }

        Model::Node* WorldReader::onWorldspawn(const std::vector<Model::EntityProperty>& properties, ParserStatus& /* status */) {
            auto entity = Model::Entity{properties};

            // handle default layer attributes, which are stored in worldspawn
            auto* defaultLayerNode = m_world->defaultLayer();
            auto defaultLayer = defaultLayerNode->layer();
            if (const auto* colorStr = entity.property(Model::PropertyKeys::LayerColor)) {
                if (const auto color = Color::parse(*colorStr)) {
                    defaultLayer.setColor(*color);
                }
                entity.removeProperty(Model::PropertyKeys::LayerColor);
            }
            if (const auto* omitFromExportStr = entity.property(Model::PropertyKeys::LayerOmitFromExport)) {
                if (*omitFromExportStr == Model::PropertyValues::LayerOmitFromExportValue) {
                    defaultLayer.setOmitFromExport(true);
                }
                entity.removeProperty(Model::PropertyKeys::LayerOmitFromExport);
            }
            defaultLayerNode->setLayer(std::move(defaultLayer));

            if (const auto* lockedStr = entity.property(Model::PropertyKeys::LayerLocked)) {
                if (*lockedStr == Model::PropertyValues::LayerLockedValue) {
                    defaultLayerNode->setLockState(Model::LockState::Lock_Locked);
                }
                entity.removeProperty(Model::PropertyKeys::LayerOmitFromExport);
            }
            if (const auto* hiddenStr = entity.property(Model::PropertyKeys::LayerHidden)) {
                if (*hiddenStr == Model::PropertyValues::LayerHiddenValue) {
                    defaultLayerNode->setVisibilityState(Model::VisibilityState::Visibility_Hidden);
                }
                entity.removeProperty(Model::PropertyKeys::LayerOmitFromExport);
            }

            m_world->setEntity(std::move(entity));
            return m_world->defaultLayer();
        }

        void WorldReader::onWorldspawnFilePosition(const size_t lineNumber, const size_t lineCount, ParserStatus& /* status */) {
            m_world->setFilePosition(lineNumber, lineCount);
        }

        void WorldReader::onLayer(Model::LayerNode* layer, ParserStatus& /* status */) {
            m_world->addChild(layer);
        }

        void WorldReader::onNode(Model::Node* parent, Model::Node* node, ParserStatus& /* status */) {
            if (parent != nullptr) {
                parent->addChild(node);
            } else {
                m_world->defaultLayer()->addChild(node);
            }
        }

        void WorldReader::onUnresolvedNode(const ParentInfo& parentInfo, Model::Node* node, ParserStatus& status) {
            if (parentInfo.type == ParentType::Layer) {
                status.warn(node->lineNumber(), kdl::str_to_string("Entity references missing layer '", parentInfo.id, "', adding to default layer"));
            } else {
                status.warn(node->lineNumber(), kdl::str_to_string("Entity references missing group '", parentInfo.id, "', adding to default layer"));
            }
            m_world->defaultLayer()->addChild(node);
        }

        void WorldReader::onBrush(Model::Node* parent, Model::BrushNode* brush, ParserStatus& /* status */) {
            if (parent != nullptr) {
                parent->addChild(brush);
            } else {
                m_world->defaultLayer()->addChild(brush);
            }
        }
    }
}
