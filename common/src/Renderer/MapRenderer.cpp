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

#include "MapRenderer.h"

#include "PreferenceManager.h"
#include "Preferences.h"
#include "Assets/EntityDefinitionManager.h"
#include "Model/Brush.h"
#include "Model/BrushNode.h"
#include "Model/BrushFace.h"
#include "Model/EditorContext.h"
#include "Model/EntityNode.h"
#include "Model/GroupNode.h"
#include "Model/LayerNode.h"
#include "Model/Node.h"
#include "Model/PatchNode.h"
#include "Model/WorldNode.h"
#include "Renderer/BrushRenderer.h"
#include "Renderer/EntityLinkRenderer.h"
#include "Renderer/GroupRenderer.h"
#include "Renderer/EntityRenderer.h"
#include "Renderer/GroupLinkRenderer.h"
#include "Renderer/PatchRenderer.h"
#include "Renderer/RenderBatch.h"
#include "Renderer/RenderContext.h"
#include "Renderer/RenderUtils.h"
#include "View/Selection.h"
#include "View/MapDocument.h"

#include <kdl/memory_utils.h>
#include <kdl/overload.h>
#include <kdl/vector_set.h>

#include <set>
#include <vector>

#include <QDebug>

namespace TrenchBroom {
    namespace Renderer {
        MapRenderer::MapRenderer(std::weak_ptr<View::MapDocument> document) :
        m_document(document),
        m_groupRenderer(std::make_unique<GroupRenderer>(kdl::mem_lock(document)->editorContext())),
        m_entityRenderer(std::make_unique<EntityRenderer>(
            *kdl::mem_lock(document),
            kdl::mem_lock(document)->entityModelManager(),
            kdl::mem_lock(document)->editorContext())),
        m_entityLinkRenderer(std::make_unique<EntityLinkRenderer>(m_document)),
        m_brushRenderer(std::make_unique<BrushRenderer>(kdl::mem_lock(document)->editorContext())),
        m_patchRenderer(std::make_unique<PatchRenderer>()),
        m_groupLinkRenderer(std::make_unique<GroupLinkRenderer>(m_document)) {
            connectObservers();
            setupRenderers();
        }

        MapRenderer::~MapRenderer() {
            clear();
        }

        void MapRenderer::clear() {
            m_groupRenderer->clear();
            m_entityRenderer->clear();
            m_entityLinkRenderer->invalidate();
            m_brushRenderer->clear();
            m_patchRenderer->clear();
            m_groupLinkRenderer->invalidate();
        }

        /**
         * Used to flash the selection color e.g. when duplicating
         */
        void MapRenderer::overrideSelectionColors(const Color& color, const float mix) {
            const Color edgeColor = pref(Preferences::SelectedEdgeColor).mixed(color, mix);
            const Color occludedEdgeColor = pref(Preferences::SelectedFaceColor).mixed(color, mix);
            const Color tintColor = pref(Preferences::SelectedFaceColor).mixed(color, mix);

            //m_selectionRenderer->setEntityBoundsColor(edgeColor);
            //m_selectionRenderer->setBrushEdgeColor(edgeColor);
            // m_selectionRenderer->setOccludedEdgeColor(occludedEdgeColor);
            // m_selectionRenderer->setTintColor(tintColor);
        }

        void MapRenderer::restoreSelectionColors() {
            //setupSelectionRenderer(*m_selectionRenderer);
        }

        void MapRenderer::render(RenderContext& renderContext, RenderBatch& renderBatch) {
            commitPendingChanges();
            setupGL(renderBatch);

            // renderDefaultOpaque(renderContext, renderBatch);
            // renderLockedOpaque(renderContext, renderBatch);
            // renderSelectionOpaque(renderContext, renderBatch);

            m_brushRenderer->renderOpaque(renderContext, renderBatch);
            m_entityRenderer->render(renderContext, renderBatch);
            m_groupRenderer->render(renderContext, renderBatch);
            m_patchRenderer->render(renderContext, renderBatch);

            // renderDefaultTransparent(renderContext, renderBatch);
            // renderLockedTransparent(renderContext, renderBatch);
            // renderSelectionTransparent(renderContext, renderBatch);
            m_brushRenderer->renderTransparent(renderContext, renderBatch);

            renderEntityLinks(renderContext, renderBatch);
            renderGroupLinks(renderContext, renderBatch);
        }

        void MapRenderer::commitPendingChanges() {
            auto document = kdl::mem_lock(m_document);
            document->commitPendingAssets();
        }

        class SetupGL : public Renderable {
        private:
            void doRender(RenderContext&) override {
                glAssert(glFrontFace(GL_CW))
                glAssert(glEnable(GL_CULL_FACE))
                glAssert(glEnable(GL_DEPTH_TEST))
                glAssert(glDepthFunc(GL_LEQUAL))
                glResetEdgeOffset();
            }
        };

        void MapRenderer::setupGL(RenderBatch& renderBatch) {
            renderBatch.addOneShot(new SetupGL());
        }

        // void MapRenderer::renderDefaultOpaque(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     m_defaultRenderer->setShowOverlays(renderContext.render3D());
        //     m_defaultRenderer->renderOpaque(renderContext, renderBatch);
        // }
        //
        // void MapRenderer::renderDefaultTransparent(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     m_defaultRenderer->setShowOverlays(renderContext.render3D());
        //     //m_defaultRenderer->renderTransparent(renderContext, renderBatch);
        // }
        //
        // void MapRenderer::renderSelectionOpaque(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     if (!renderContext.hideSelection()) {
        //         m_selectionRenderer->renderOpaque(renderContext, renderBatch);
        //     }
        // }
        //
        // void MapRenderer::renderSelectionTransparent(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     // if (!renderContext.hideSelection()) {
        //     //     m_selectionRenderer->renderTransparent(renderContext, renderBatch);
        //     // }
        // }
        //
        // void MapRenderer::renderLockedOpaque(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     m_lockedRenderer->setShowOverlays(renderContext.render3D());
        //     m_lockedRenderer->renderOpaque(renderContext, renderBatch);
        // }
        //
        // void MapRenderer::renderLockedTransparent(RenderContext& renderContext, RenderBatch& renderBatch) {
        //     m_lockedRenderer->setShowOverlays(renderContext.render3D());
        //     //m_lockedRenderer->renderTransparent(renderContext, renderBatch);
        // }

        void MapRenderer::renderEntityLinks(RenderContext& renderContext, RenderBatch& renderBatch) {
            m_entityLinkRenderer->render(renderContext, renderBatch);
        }

        void MapRenderer::renderGroupLinks(RenderContext& renderContext, RenderBatch& renderBatch) {
            m_groupLinkRenderer->render(renderContext, renderBatch);
        }

        void MapRenderer::setupRenderers() {
            // setupDefaultRenderer(*m_defaultRenderer);
            // setupSelectionRenderer(*m_selectionRenderer);
            // setupLockedRenderer(*m_lockedRenderer);
            setupEntityLinkRenderer();
        }

#if 0
        void MapRenderer::setupDefaultRenderer(ObjectRenderer& renderer) {
            renderer.setEntityOverlayTextColor(pref(Preferences::InfoOverlayTextColor));
            renderer.setGroupOverlayTextColor(pref(Preferences::GroupInfoOverlayTextColor));
            renderer.setOverlayBackgroundColor(pref(Preferences::InfoOverlayBackgroundColor));
            renderer.setTint(false);
            renderer.setTransparencyAlpha(pref(Preferences::TransparentFaceAlpha));

            renderer.setGroupBoundsColor(pref(Preferences::DefaultGroupColor));
            renderer.setEntityBoundsColor(pref(Preferences::UndefinedEntityColor));

            renderer.setBrushFaceColor(pref(Preferences::FaceColor));
            renderer.setBrushEdgeColor(pref(Preferences::EdgeColor));
        }

        void MapRenderer::setupSelectionRenderer(ObjectRenderer& renderer) {
            renderer.setEntityOverlayTextColor(pref(Preferences::SelectedInfoOverlayTextColor));
            renderer.setGroupOverlayTextColor(pref(Preferences::SelectedInfoOverlayTextColor));
            renderer.setOverlayBackgroundColor(pref(Preferences::SelectedInfoOverlayBackgroundColor));
            renderer.setShowBrushEdges(true);
            renderer.setShowOccludedObjects(true);
            renderer.setOccludedEdgeColor(Color(pref(Preferences::SelectedEdgeColor), pref(Preferences::OccludedSelectedEdgeAlpha)));
            renderer.setTint(true);
            renderer.setTintColor(pref(Preferences::SelectedFaceColor));

            renderer.setOverrideGroupColors(true);
            renderer.setGroupBoundsColor(pref(Preferences::SelectedEdgeColor));

            renderer.setOverrideEntityBoundsColor(true);
            renderer.setEntityBoundsColor(pref(Preferences::SelectedEdgeColor));
            renderer.setShowEntityAngles(true);
            renderer.setEntityAngleColor(pref(Preferences::AngleIndicatorColor));

            renderer.setBrushFaceColor(pref(Preferences::FaceColor));
            renderer.setBrushEdgeColor(pref(Preferences::SelectedEdgeColor));
        }

        void MapRenderer::setupLockedRenderer(ObjectRenderer& renderer) {
            renderer.setEntityOverlayTextColor(pref(Preferences::LockedInfoOverlayTextColor));
            renderer.setGroupOverlayTextColor(pref(Preferences::LockedInfoOverlayTextColor));
            renderer.setOverlayBackgroundColor(pref(Preferences::LockedInfoOverlayBackgroundColor));
            renderer.setShowOccludedObjects(false);
            renderer.setTint(true);
            renderer.setTintColor(pref(Preferences::LockedFaceColor));
            renderer.setTransparencyAlpha(pref(Preferences::TransparentFaceAlpha));

            renderer.setOverrideGroupColors(true);
            renderer.setGroupBoundsColor(pref(Preferences::LockedEdgeColor));

            renderer.setOverrideEntityBoundsColor(true);
            renderer.setEntityBoundsColor(pref(Preferences::LockedEdgeColor));
            renderer.setShowEntityAngles(false);

            renderer.setBrushFaceColor(pref(Preferences::FaceColor));
            renderer.setBrushEdgeColor(pref(Preferences::LockedEdgeColor));
        }
#endif

        void MapRenderer::setupEntityLinkRenderer() {
        }

        void MapRenderer::updateRenderers() {
            auto document = kdl::mem_lock(m_document);
            Model::WorldNode* world = document->world();

            std::vector<Model::EntityNode*> entities;
            std::vector<Model::GroupNode*> groups;
            std::vector<Model::BrushNode*> brushes;
            std::vector<Model::PatchNode*> patches;

            world->accept(kdl::overload(
                [&](auto&& thisLambda, Model::WorldNode* world) { world->visitChildren(thisLambda); },
                [&](auto&& thisLambda, Model::LayerNode* layer) { layer->visitChildren(thisLambda); },
                [&](auto&& thisLambda, Model::GroupNode* group) {
                    groups.push_back(group);
                    group->visitChildren(thisLambda);
                },
                [&](auto&& thisLambda, Model::EntityNode* entity) {
                    entities.push_back(entity);
                    entity->visitChildren(thisLambda);
                },
                [&](Model::BrushNode* brush) {
                    brushes.push_back(brush);
                },
                [&](Model::PatchNode* patchNode) {
                    patches.push_back(patchNode);
#if 0
                    // FIXME: re-add this logic in the patch renderer
                    if (patchNode->locked()) {
                        if (renderLocked) lockedNodes.patches.push_back(patchNode);
                    } else if (selected(patchNode)) {
                        if (renderSelection) selectedNodes.patches.push_back(patchNode);
                    }
                    if (!patchNode->selected() && !patchNode->parentSelected() && !patchNode->locked()) {
                        if (renderDefault) defaultNodes.patches.push_back(patchNode);
                    }
#endif
                }
            ));

            m_entityRenderer->setEntities(entities);
            m_groupRenderer->setGroups(groups);
            m_brushRenderer->setBrushes(brushes);
            m_patchRenderer->setPatches(patches);

            invalidateEntityLinkRenderer();
        }

        void MapRenderer::invalidateRenderers() {
            m_groupRenderer->invalidate();
            m_entityRenderer->invalidate();
            m_entityLinkRenderer->invalidate();
            m_brushRenderer->invalidate();
            m_patchRenderer->invalidate();
        }

        void MapRenderer::invalidateEntityLinkRenderer() {
            m_entityLinkRenderer->invalidate();
        }

        void MapRenderer::invalidateGroupLinkRenderer() {
            m_groupLinkRenderer->invalidate();
        }

        void MapRenderer::reloadEntityModels() {
            m_entityRenderer->reloadModels();
        }

        void MapRenderer::connectObservers() {
            assert(!kdl::mem_expired(m_document));
            auto document = kdl::mem_lock(m_document);

            m_notifierConnection += document->documentWasClearedNotifier.connect(this, &MapRenderer::documentWasCleared);
            m_notifierConnection += document->documentWasNewedNotifier.connect(this, &MapRenderer::documentWasNewedOrLoaded);
            m_notifierConnection += document->documentWasLoadedNotifier.connect(this, &MapRenderer::documentWasNewedOrLoaded);
            m_notifierConnection += document->nodesWereAddedNotifier.connect(this, &MapRenderer::nodesWereAdded);
            m_notifierConnection += document->nodesWereRemovedNotifier.connect(this, &MapRenderer::nodesWereRemoved);
            m_notifierConnection += document->nodesDidChangeNotifier.connect(this, &MapRenderer::nodesDidChange);
            m_notifierConnection += document->nodeVisibilityDidChangeNotifier.connect(this, &MapRenderer::nodeVisibilityDidChange);
            m_notifierConnection += document->nodeLockingDidChangeNotifier.connect(this, &MapRenderer::nodeLockingDidChange);
            m_notifierConnection += document->groupWasOpenedNotifier.connect(this, &MapRenderer::groupWasOpened);
            m_notifierConnection += document->groupWasClosedNotifier.connect(this, &MapRenderer::groupWasClosed);
            m_notifierConnection += document->brushFacesDidChangeNotifier.connect(this, &MapRenderer::brushFacesDidChange);
            m_notifierConnection += document->selectionDidChangeNotifier.connect(this, &MapRenderer::selectionDidChange);
            m_notifierConnection += document->textureCollectionsWillChangeNotifier.connect(this, &MapRenderer::textureCollectionsWillChange);
            m_notifierConnection += document->entityDefinitionsDidChangeNotifier.connect(this, &MapRenderer::entityDefinitionsDidChange);
            m_notifierConnection += document->modsDidChangeNotifier.connect(this, &MapRenderer::modsDidChange);
            m_notifierConnection += document->editorContextDidChangeNotifier.connect(this, &MapRenderer::editorContextDidChange);

            PreferenceManager& prefs = PreferenceManager::instance();
            m_notifierConnection += prefs.preferenceDidChangeNotifier.connect(this, &MapRenderer::preferenceDidChange);
        }

        static void debugLog(const char* functionName) {
            qDebug() << functionName;
        }

        static void debugLog(const char* functionName, const std::vector<Model::Node*>& nodes) {
            qDebug() << functionName << nodes.size() << "nodes";
        }

        static void debugLog(const char* functionName, const Model::Node*) {
            qDebug() << functionName << "1 node";
        }

        static void debugLog(const char* functionName, const std::vector<Model::BrushFaceHandle>& faces) {
            qDebug() << functionName << faces.size() << "face handles";
        }

        static void debugLog(const char* functionName, const View::Selection& selection) {
            qDebug() << functionName <<
                QString::fromLatin1("%1/%2 nodes selected/deselected, %3/%4 faces selected/deselected")
                .arg(selection.selectedNodes().size())
                .arg(selection.deselectedNodes().size())
                .arg(selection.selectedBrushFaces().size())
                .arg(selection.deselectedBrushFaces().size());
        }

        void MapRenderer::documentWasCleared(View::MapDocument*) {
            debugLog(__func__);
            clear();
        }

        void MapRenderer::documentWasNewedOrLoaded(View::MapDocument*) {
            debugLog(__func__);
            clear();
            updateRenderers();
        }

        void MapRenderer::nodesWereAdded(const std::vector<Model::Node*>& nodes) {
            debugLog(__func__, nodes);
            // FIXME: selective
            updateRenderers();
            invalidateGroupLinkRenderer();
        }

        void MapRenderer::nodesWereRemoved(const std::vector<Model::Node*>& nodes) {
            debugLog(__func__, nodes);
            // FIXME: selective
            updateRenderers();
            invalidateGroupLinkRenderer();
        }

        void MapRenderer::nodesDidChange(const std::vector<Model::Node*>& nodes) {
            debugLog(__func__, nodes);
            invalidateNodes(nodes);
            invalidateEntityLinkRenderer();
            invalidateGroupLinkRenderer();
        }

        void MapRenderer::nodeVisibilityDidChange(const std::vector<Model::Node*>& nodes) {
            debugLog(__func__, nodes);
            // FIXME: do we need to add/remove from the renderers?
            invalidateNodes(nodes);
            //invalidateRenderers();
        }

        void MapRenderer::nodeLockingDidChange(const std::vector<Model::Node*>& nodes) {
            debugLog(__func__, nodes);
            invalidateNodes(nodes);
            //updateRenderers();
        }

        void MapRenderer::groupWasOpened(Model::GroupNode* group) {
            debugLog(__func__, group);
            updateRenderers();
            invalidateGroupLinkRenderer();
        }

        void MapRenderer::groupWasClosed(Model::GroupNode* group) {
            debugLog(__func__, group);
            updateRenderers();
            invalidateGroupLinkRenderer();
        }

        void MapRenderer::brushFacesDidChange(const std::vector<Model::BrushFaceHandle>& faces) {
            debugLog(__func__, faces);
            invalidateBrushFaces(faces);
        }

        void MapRenderer::selectionDidChange(const View::Selection& selection) {
            debugLog(__func__, selection);
            invalidateNodes(selection.selectedNodes());
            invalidateNodes(selection.deselectedNodes());
            invalidateBrushFaces(selection.selectedBrushFaces());
            invalidateBrushFaces(selection.deselectedBrushFaces());
            invalidateGroupLinkRenderer();
        }

        void MapRenderer::textureCollectionsWillChange() {
            debugLog(__func__);
            invalidateRenderers();
        }

        void MapRenderer::entityDefinitionsDidChange() {
            reloadEntityModels();
            invalidateRenderers();
            invalidateEntityLinkRenderer();
        }

        void MapRenderer::modsDidChange() {
            reloadEntityModels();
            invalidateRenderers();
            invalidateEntityLinkRenderer();
        }

        void MapRenderer::editorContextDidChange() {
            invalidateRenderers();
            invalidateEntityLinkRenderer();
            invalidateGroupLinkRenderer();
        }

        void MapRenderer::preferenceDidChange(const IO::Path& path) {
            setupRenderers();

            auto document = kdl::mem_lock(m_document);
            if (document->isGamePathPreference(path)) {
                reloadEntityModels();
                invalidateRenderers();
                invalidateEntityLinkRenderer();
                invalidateGroupLinkRenderer();
            }

            if (path.hasPrefix(IO::Path("Map view"), true)) {
                invalidateRenderers();
                invalidateEntityLinkRenderer();
                invalidateGroupLinkRenderer();
            }
        }

        // invalidating specific nodes

        void MapRenderer::invalidateNodes(const std::vector<Model::Node*>& nodes) {
            size_t invalidatedNodes = 0;

            for (auto* node : nodes) {
                node->accept(kdl::overload(
                    [](auto&&, Model::WorldNode* world)  {
                        //world->visitChildren(thisLambda);
                    },
                    [](auto&&, Model::LayerNode* layer)  {
                        //layer->visitChildren(thisLambda);
                    },
                    [&](auto&&, Model::GroupNode* group) {
                        m_groupRenderer->invalidateGroup(group);
                        ++invalidatedNodes;
                        //group->visitChildren(thisLambda);
                    },
                    [&](auto&&, Model::EntityNode* entity) {
                        m_entityRenderer->invalidateEntity(entity);
                        ++invalidatedNodes;
                        //entity->visitChildren(thisLambda);
                    },
                    [&](auto&&, Model::BrushNode* brush) {
                        m_brushRenderer->invalidateBrush(brush);
                        ++invalidatedNodes;
                    },
                    [&](auto&&, Model::PatchNode* patchNode) {
                        m_patchRenderer->invalidate();
                        ++invalidatedNodes;
                    }
                ));
            }
        }

        void MapRenderer::invalidateBrushFaces(const std::vector<Model::BrushFaceHandle>& faces) {
            for (const auto& face : faces) {
                m_brushRenderer->invalidateBrush(face.node());
            }
        }
    }
}
