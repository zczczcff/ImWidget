#include "CommandBase.h"
#include "ImWidget/ImWidgetProperty.h"
namespace ImGuiWidget
{
    class ImWidget;
}
//  Ù–‘±‡º≠√¸¡Ó
class PropertyEditCommand : public ICommand
{
private:
    ImGuiWidget::ImWidget* m_Widget;
    std::string m_PropertyName;
    std::vector<uint8_t> m_OldValue;
    std::vector<uint8_t> m_NewValue;
    ImGuiWidget::PropertyType m_PropertyType;

public:
    PropertyEditCommand(ImGuiWidget::ImWidget* widget,
        const std::string& propertyName,
        void* oldValue, void* newValue,
        ImGuiWidget::PropertyType type);
    virtual ~PropertyEditCommand(){}
    void Execute() override;
    void Undo() override;
    std::string GetDescription() const override;
    bool MergeWith(ICommand* other) override;

private:
    void SerializeValue(void* value, std::vector<uint8_t>& buffer);
    void DeserializeValue(std::vector<uint8_t>& buffer, void* value);
    size_t GetTypeSize(ImGuiWidget::PropertyType type);
};