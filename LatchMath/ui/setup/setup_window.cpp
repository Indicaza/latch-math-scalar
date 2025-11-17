#include "StdAfx.h"
#include "ui/setup/setup_window.h"

using namespace Dewesoft::MUI;

void LatchMathSetupWindow::bindEvents()
{
    latchCriteriaEdit.OnTextChanged += mathEvent(&LatchMathSetupWindow::onLatchCriteriaEditChanged);
    edgeTypeCBox.OnChange += mathEvent(&LatchMathSetupWindow::onEdgeTypeCBoxChanged);
    criteriaChannelCBox.OnChange += mathEvent(&LatchMathSetupWindow::onCriteriaChannelCBoxChanged);
}

void LatchMathSetupWindow::initiate()
{
    // fill edge type
    edgeTypeCBox.clear();
    edgeTypeCBox.addItem("Rising");
    edgeTypeCBox.addItem("Falling");
    edgeTypeCBox.setSelectedIndex((int) module->edgeType);

    // fill criteria limit
    latchCriteriaEdit.setText(std::to_wstring(module->criteriaLimit));

    // Populate criteria channel dropdown from SharedModule slot
    criteriaChannelCBox.clear();

    ChannelSlot& slot = sharedModule->getInputSlot("Criteria channel");

    for (std::string& name : availableChannelsFor(slot))
        criteriaChannelCBox.addItem(name);

    // Set selected item if channel already assigned
    if (slot.getAssignedChannel())
        criteriaChannelCBox.setSelectedIndex(criteriaChannelCBox.getIndexOf(slot.getAssignedChannel().name()));
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

void LatchMathSetupWindow::onCriteriaChannelCBoxChanged(ComboBox& cBox, EventArgs& args)
{
    sharedModule->getInputSlot("Criteria channel").assignChannel(criteriaChannelCBox.getSelectedItem());
}
