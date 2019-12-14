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

#ifndef TRENCHBROOM_VIEW_FORWARD_H
#define TRENCHBROOM_VIEW_FORWARD_H

namespace TrenchBroom {
    namespace View {
        class MapDocumentCommandFacade;
        class MapDocument;

        class Grid;

        class Selection;

        class InputState;

        class Tool;
        class ClipTool;
        class CreateComplexBrushTool;
        class CreateEntityTool;
        class CreateSimpleBrushTool;
        class EdgeTool;
        class FaceTool;
        class VertexTool;

        class KeyEvent;
        class MouseEvent;
        class CancelEvent;
        class InputEventProcessor;

        class Command;
        class UndoableCommand;
        class CommandProcessor;

        class SmartAttributeEditorManager;

        class VertexHandleManager;
        class VertexHandleManagerBase;
        template <typename H> class VertexHandleManagerBaseT;
        class EdgeHandleManager;

        class MapFrame;
        class GLContextManager;

        class MapViewConfig;
        class MapViewBase;
        class MapView2D;
        class MapView3D;
        class MapViewToolBox;

        class ViewEffectsService;

        enum class PasteType;

        class CompilationContext;
        class CompilationRunner;
        class CompilationProfileManager;

        class GameEngineProfileManager;
        class GameEngineProfileEditor;
        class GameEngineProfileListBox;

        class MapInspector;
        class EntityInspector;
        class FaceInspector;

        class EntityAttributeGrid;
        class EntityAttributeModel;
        class EntityAttributeTable;
        class EntityAttributeEditor;

        class EntityBrowser;
        class EntityBrowserView;

        class EntityDefinitionFileChooser;
        class FlagsPopupEditor;

        class UVEditor;

        class FaceAttribsEditor;
        class TextureBrowser;

        class FileTextureCollectionEditor;

        class Console;
        class IssueBrowser;
        class IssueBrowserView;
        class IssueBrowserModel;

        class Action;

        class KeyboardShortcutModel;

        class GameListBox;
        class LayerListBox;

        class ContainerBar;
        class TabBook;

        class BorderLine;
        class SpinControl;
        class ElidedLabel;
        class FlagsEditor;
        class PopupButton;
        class LimitedKeySequenceEdit;
        class MultiCompletionLineEdit;
    }
}

#endif //TRENCHBROOM_VIEW_FORWARD_H
