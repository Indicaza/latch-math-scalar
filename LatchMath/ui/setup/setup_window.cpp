#include "StdAfx.h"
#include "ui/setup/setup_window.h"

using namespace Dewesoft::MUI;

void LatchMathSetupWindow::bindEvents()
{
    latchCriteriaEdit.OnTextChanged += mathEvent(&LatchMathSetupWindow::onLatchCriteriaEditChanged);
    edgeTypeCBox.OnChange += mathEvent(&LatchMathSetupWindow::onEdgeTypeCBoxChanged);
}

void LatchMathSetupWindow::initiate()
{
    // Fill UI from module state
    latchCriteriaEdit.setText(std::to_wstring(module->criteriaLimit));

    edgeTypeCBox.clear();
    edgeTypeCBox.addItem("Rising");
    edgeTypeCBox.addItem("Falling");
    edgeTypeCBox.setSelectedIndex((int) module->edgeType);
}

void LatchMathSetupWindow::onLatchCriteriaEditChanged(TextBox& editBox, EventArgs& args)
{
    try
    {
        module->criteriaLimit = std::stod((std::wstring) latchCriteriaEdit.getText());
    }
    catch (...)
    {
        module->criteriaLimit = 0;
    }
}

void LatchMathSetupWindow::onEdgeTypeCBoxChanged(ComboBox& cBox, EventArgs& args)
{
    module->edgeType = (edgeTypes) edgeTypeCBox.getSelectedIndex();
}
