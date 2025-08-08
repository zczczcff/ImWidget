#include "ImMultiLineTextBlock.h"

namespace ImGuiWidget
{
    std::set<ImMultiLineTextBlock*> ImMultiLineTextBlock::s_SelectedWidgets = {};
    ImMultiLineTextBlock::GlobalDragState ImMultiLineTextBlock::s_DragState = {};
}
