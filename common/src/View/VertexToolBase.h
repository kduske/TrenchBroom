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

#ifndef VertexToolBase_h
#define VertexToolBase_h

#include "VecMath.h"
#include "TrenchBroom.h"
#include "PreferenceManager.h"
#include "Preferences.h"
#include "Model/Hit.h"
#include "Model/ModelTypes.h"
#include "Renderer/RenderBatch.h"
#include "Renderer/RenderService.h"
#include "View/Lasso.h"
#include "View/MapDocument.h"
#include "View/MoveBrushVerticesCommand.h"
#include "View/MoveBrushEdgesCommand.h"
#include "View/MoveBrushFacesCommand.h"
#include "View/RemoveBrushVerticesCommand.h"
#include "View/RemoveBrushEdgesCommand.h"
#include "View/RemoveBrushFacesCommand.h"
#include "View/Selection.h"
#include "View/SplitBrushEdgesCommand.h"
#include "View/SplitBrushFacesCommand.h"
#include "View/Tool.h"
#include "View/VertexCommand.h"
#include "View/VertexHandleManager.h"
#include "View/ViewTypes.h"

#include <cassert>
#include <numeric>

namespace TrenchBroom {
    namespace Model {
        class PickResult;
    }
    
    namespace Renderer {
        class Camera;
    }
    
    namespace View {
        class Grid;
        class Lasso;
        
        template <typename H>
        class VertexToolBase : public Tool {
        public:
            typedef enum {
                MR_Continue,
                MR_Deny,
                MR_Cancel
            } MoveResult;
        protected:
            MapDocumentWPtr m_document;
        private:
            size_t m_changeCount;
        protected:
            bool m_ignoreChangeNotifications;
            
            H m_dragHandlePosition;
            bool m_dragging;
        protected:
            VertexToolBase(MapDocumentWPtr document) :
            Tool(false),
            m_document(document),
            m_changeCount(0),
            m_ignoreChangeNotifications(false),
            m_dragging(false) {}
        public:
            virtual ~VertexToolBase() {}
        public:
            const Grid& grid() const {
                return lock(m_document)->grid();
            }

            const Model::BrushList& selectedBrushes() const {
                MapDocumentSPtr document = lock(m_document);
                return document->selectedNodes().brushes();
            }
        public:
            template <typename M, typename H2>
            Model::BrushSet findIncidentBrushes(const M& manager, const H2& handle) const {
                const Model::BrushList& brushes = selectedBrushes();
                return manager.findIncidentBrushes(handle, std::begin(brushes), std::end(brushes));
            }
            
            virtual void pick(const Ray3& pickRay, const Renderer::Camera& camera, Model::PickResult& pickResult) const = 0;
        public: // Handle selection
            bool select(const Model::Hit::List& hits, const bool addToSelection) {
                assert(!hits.empty());
                const Model::Hit& firstHit = hits.front();
                if (firstHit.type() == VertexHandleManager::HandleHit) {
                    if (!addToSelection)
                        handleManager().deselectAll();
                    
                    // Count the number of hit handles which are selected already.
                    const size_t selected = std::accumulate(std::begin(hits), std::end(hits), 0u, [this](const size_t cur, const Model::Hit& hit) {
                        const H& handle = hit.target<H>();
                        const bool curSelected = handleManager().selected(handle);
                        return cur + (curSelected ? 1 : 0);
                    });
                    
                    if (selected < hits.size()) {
                        for (const auto& hit : hits)
                            handleManager().select(hit.target<H>());
                    } else if (addToSelection) {
                        // The user meant to deselect a selected handle.
                        for (const auto& hit : hits)
                            handleManager().deselect(hit.target<H>());
                    }
                }
                refreshViews();
                return true;
            }
            
            void select(const Lasso& lasso, const bool modifySelection) {
                typedef std::vector<H> HandleList;
                
                const HandleList allHandles = handleManager().allHandles();
                HandleList selectedHandles;
                
                lasso.selected(std::begin(allHandles), std::end(allHandles), std::back_inserter(selectedHandles));
                if (!modifySelection)
                    handleManager().deselectAll();
                handleManager().toggle(std::begin(selectedHandles), std::end(selectedHandles));
            }
            
            virtual bool deselectAll() {
                if (handleManager().anySelected()) {
                    handleManager().deselectAll();
                    refreshViews();
                    return true;
                }
                return false;
            }
        private:
            typedef VertexHandleManagerBaseT<H> HandleManager;
            virtual HandleManager& handleManager() = 0;
            virtual const HandleManager& handleManager() const = 0;
        public: // performing moves
            virtual bool startMove(const Model::Hit& hit) = 0;
            virtual MoveResult move(const Vec3& delta) = 0;
            virtual void endMove() = 0;
            virtual void cancelMove() = 0;
            
            virtual const Vec3& getHandlePosition(const Model::Hit& hit) const = 0;
        public: // rendering
            void renderHandles(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch) const {
                Renderer::RenderService renderService(renderContext, renderBatch);
                if (!handleManager().allSelected())
                    renderHandles(handleManager().unselectedHandles(), renderService, pref(Preferences::HandleColor));
                if (handleManager().anySelected())
                    renderHandles(handleManager().selectedHandles(), renderService, pref(Preferences::SelectedHandleColor));
            }
            
            void renderDragHandle(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch) const {
                renderHandle(renderContext, renderBatch, m_dragHandlePosition, pref(Preferences::SelectedHandleColor));
            }
            
            void renderHandle(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch, const H& handle) const {
                renderHandle(renderContext, renderBatch, handle, pref(Preferences::HandleColor));
            }
            
            void renderDragHighlight(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch) const {
                renderHighlight(renderContext, renderBatch, m_dragHandlePosition);
            }
            
            void renderDragGuide(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch) const {
                renderGuide(renderContext, renderBatch, m_dragHandlePosition);
            }
            
            virtual void renderHandles(const std::vector<H>& handles, Renderer::RenderService& renderService, const Color& color) const = 0;
            virtual void renderHandle(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch, const H& handle, const Color& color) const = 0;
            virtual void renderHighlight(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch, const H& handle) const = 0;
            virtual void renderGuide(Renderer::RenderContext& renderContext, Renderer::RenderBatch& renderBatch, const H& position) const = 0;
        protected: // Tool interface
            virtual bool doActivate() override {
                m_changeCount = 0;
                bindObservers();
                return true;
            }
            
            virtual bool doDeactivate() override {
                /*
                 if (m_changeCount > 0) {
                 RebuildBrushGeometryCommand* command = RebuildBrushGeometryCommand::rebuildGeometry(document, document.editStateManager().selectedBrushes(), m_changeCount);
                 submitCommand(command);
                 }
                 */
                unbindObservers();
                return true;
            }
        private: // Observers and state management
            void bindObservers() {
                MapDocumentSPtr document = lock(m_document);
                document->selectionDidChangeNotifier.addObserver(this,  &VertexToolBase::selectionDidChange);
                document->nodesWillChangeNotifier.addObserver(this,  &VertexToolBase::nodesWillChange);
                document->nodesDidChangeNotifier.addObserver(this,  &VertexToolBase::nodesDidChange);
                document->commandDoNotifier.addObserver(this,  &VertexToolBase::commandDo);
                document->commandDoneNotifier.addObserver(this,  &VertexToolBase::commandDone);
                document->commandDoFailedNotifier.addObserver(this,  &VertexToolBase::commandDoFailed);
                document->commandUndoNotifier.addObserver(this,  &VertexToolBase::commandUndo);
                document->commandUndoneNotifier.addObserver(this,  &VertexToolBase::commandUndone);
                document->commandUndoFailedNotifier.addObserver(this,  &VertexToolBase::commandUndoFailed);
            }
            
            void unbindObservers() {
                if (!expired(m_document)) {
                    MapDocumentSPtr document = lock(m_document);
                    document->selectionDidChangeNotifier.removeObserver(this,  &VertexToolBase::selectionDidChange);
                    document->nodesWillChangeNotifier.removeObserver(this,  &VertexToolBase::nodesWillChange);
                    document->nodesDidChangeNotifier.removeObserver(this,  &VertexToolBase::nodesDidChange);
                    document->commandDoNotifier.removeObserver(this,  &VertexToolBase::commandDo);
                    document->commandDoneNotifier.removeObserver(this,  &VertexToolBase::commandDone);
                    document->commandDoFailedNotifier.removeObserver(this,  &VertexToolBase::commandDoFailed);
                    document->commandUndoNotifier.removeObserver(this,  &VertexToolBase::commandUndo);
                    document->commandUndoneNotifier.removeObserver(this,  &VertexToolBase::commandUndone);
                    document->commandUndoFailedNotifier.removeObserver(this,  &VertexToolBase::commandUndoFailed);
                }
            }
            
            void commandDo(Command::Ptr command) {
                commandDoOrUndo(command);
            }
            
            void commandDone(Command::Ptr command) {
                commandDoneOrUndoFailed(command);
            }
            
            void commandDoFailed(Command::Ptr command) {
                commandDoFailedOrUndone(command);
            }
            
            void commandUndo(UndoableCommand::Ptr command) {
                commandDoOrUndo(command);
            }
            
            void commandUndone(UndoableCommand::Ptr command) {
                commandDoFailedOrUndone(command);
            }
            
            void commandUndoFailed(UndoableCommand::Ptr command) {
                commandDoneOrUndoFailed(command);
            }
            
            void commandDoOrUndo(Command::Ptr command) {
                if (isVertexCommand(command)) {
                    VertexCommand* vertexCommand = static_cast<VertexCommand*>(command.get());
                    removeHandles(vertexCommand);
                    m_ignoreChangeNotifications = true;
                }
            }
            
            void commandDoneOrUndoFailed(Command::Ptr command) {
                if (isVertexCommand(command)) {
                    VertexCommand* vertexCommand = static_cast<VertexCommand*>(command.get());
                    addHandles(vertexCommand);
                    selectNewHandlePositions(vertexCommand);
                    m_ignoreChangeNotifications = false;
                    
                    if (!m_dragging)
                        rebuildBrushGeometry();
                }
            }
            
            void commandDoFailedOrUndone(Command::Ptr command) {
                if (isVertexCommand(command)) {
                    VertexCommand* vertexCommand = static_cast<VertexCommand*>(command.get());
                    addHandles(vertexCommand);
                    selectOldHandlePositions(vertexCommand);
                    m_ignoreChangeNotifications = false;
                    
                    if (!m_dragging)
                        rebuildBrushGeometry();
                }
            }
            
            bool isVertexCommand(const Command::Ptr command) const {
                return command->isType(MoveBrushVerticesCommand::Type, SplitBrushEdgesCommand::Type, SplitBrushFacesCommand::Type, RemoveBrushVerticesCommand::Type, RemoveBrushEdgesCommand::Type, RemoveBrushFacesCommand::Type, MoveBrushEdgesCommand::Type, MoveBrushFacesCommand::Type);
            }
            
            void selectionDidChange(const Selection& selection) {
                addHandles(selection.selectedNodes());
                removeHandles(selection.deselectedNodes());
            }
            
            void nodesWillChange(const Model::NodeList& nodes) {
                if (!m_ignoreChangeNotifications) {
                    removeHandles(nodes);
                }
            }
            
            void nodesDidChange(const Model::NodeList& nodes) {
                if (!m_ignoreChangeNotifications) {
                    addHandles(nodes);
                }
            }
        protected:
            void rebuildBrushGeometry() {
                // TODO: implement
            }
        protected:
            virtual void addHandles(VertexCommand* command) {
                command->addHandles(handleManager());
            }
            
            virtual void removeHandles(VertexCommand* command) {
                command->removeHandles(handleManager());
            }
            
            virtual void selectNewHandlePositions(VertexCommand* command) {
                command->selectNewHandlePositions(handleManager());
            }
            
            virtual void selectOldHandlePositions(VertexCommand* command) {
                command->selectOldHandlePositions(handleManager());
            }
            
            virtual void addHandles(const Model::NodeList& nodes) = 0;
            virtual void removeHandles(const Model::NodeList& nodes) = 0;
        };
    }
}

#endif /* VertexToolBase_h */
