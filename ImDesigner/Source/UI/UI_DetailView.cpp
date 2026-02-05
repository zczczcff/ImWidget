#include "UI/UI_DetailView.h"
#include "ImWidget/ImBasicWidgetList.h"
#include "ImWidget/ImUserWidgetClass.h"
#include "Tools/JLog.h"
#include "EditorAction.h"
#include "EditorEvents.h"
#include "Model/Model_MainModel.h"

// 全局变量声明
extern Model_MainModel* global_MainModel;

void UI_DetailView::OnKeyDown(ImGuiWidget::ImKeyDownEvent& e)
{
    if (bHasFocus && e.IsCtrl_Z())
    {
        ExecuteAction(EditedFileFullPath + Action::_REQUEST_UNDO);
    }
}

void UI_DetailView::ActionInit()
{
    ResetFileAction();
    AddSequentialProcessor(Action::ProjectView::RENAME_FILE, [this](const std::string& OldFileFullPath, const std::string& NewFileFullPath)
        {
            if (EditedFileFullPath == OldFileFullPath)
            {
                EditedFileFullPath = NewFileFullPath;
                ResetFileAction();
                ResetEvent();
            }
        });
}

void UI_DetailView::EventInit()
{
    ResetEvent();
}

void UI_DetailView::ResetEvent()
{
    for (auto& id : FileEvents)
    {
        Unsubscribe(id);
    }
    FileEvents.clear();

    FileEvents.push_back(Subscribe(EditedFileFullPath + Events::DetailView::UPDATE_PROPERTY_DISPLAY,
        [this](ImGuiWidget::ImObject* Target, const std::string& PropertyName)
        {
            UpdatePropertyDisplay(Target, PropertyName);
        }));
}

void UI_DetailView::ResetFileAction()
{
    for (auto& id : FileActions)
    {
        RemoveProcessor(id);
    }
    FileActions.clear();

    // 订阅控件选中事件，使用单参数（控件路径）
    FileActions.push_back(AddSequentialProcessor(EditedFileFullPath + Action::WIDGET_SELECTED,
        [this](const std::string& widgetPath)
        {
            if (!global_MainModel) return;
            if (widgetPath.empty()) return;

            // 获取编辑的文件
            Model_MainModel::EditedUIFile* editedFile = global_MainModel->GetEditedUIFile(EditedFileFullPath);
            if (!editedFile || !editedFile->EditedFile) return;

            ImGuiWidget::ImUserWidgetClass* targetClass = editedFile->EditedFile;

            // 解析控件路径：格式为 "WidgetTreeVarName/RelativePath" 或 "WidgetTreeVarName"
            size_t slashPos = widgetPath.find('/');
            std::string widgetVarName;
            std::string relativePath;

            if (slashPos == std::string::npos)
            {
                widgetVarName = widgetPath;
                relativePath = ".";
            }
            else
            {
                widgetVarName = widgetPath.substr(0, slashPos);
                relativePath = widgetPath.substr(slashPos + 1);
                if (relativePath.empty()) relativePath = ".";
            }

            // 获取控件树根控件
            ImGuiWidget::ImWidget* rootWidget = targetClass->GetWidgetVariable(widgetVarName);
            if (!rootWidget) return;

            // 通过相对路径查找控件
            ImGuiWidget::ImWidget* targetWidget = nullptr;
            if (relativePath == ".")
            {
                targetWidget = rootWidget;
            }
            else
            {
                targetWidget = rootWidget->FindChildByPath(relativePath);
            }

            if (targetWidget)
            {
                SetCurrentWidget(targetWidget);
            }
        }));
}

ImGuiWidget::ImHorizontalBox* UI_DetailView::HandleAddStringItem(
    const ROP::Property<ImGuiWidget::PropertyType>& prop,
    std::string& SingleString,
    ImGuiWidget::ImVerticalBox* StringListBox,
    ImGuiWidget::ImObject* Target,
    ImGuiWidget::ImWidget* WidgetOwner)
{
    ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_ItemBox");
    ImGuiWidget::ImInputText* InputString = new ImGuiWidget::ImInputText(m_WidgetID + "_InputString");
    ImGuiWidget::ImButton* DeleteButton = new ImGuiWidget::ImButton(m_WidgetID + "_DeleteButton");
    ImGuiWidget::ImTextBlock* DeleteButtonText = new ImGuiWidget::ImTextBlock(m_WidgetID + "_DeleteButtonText");

    DeleteButtonText->SetText("X");
    DeleteButton->SetContent(DeleteButtonText);
    InputString->SetText(SingleString);

    InputString->SetOnTextChanged([this, prop, OldString = SingleString, Target](const std::string& newstring) mutable
        {
            std::vector<std::string> stringvector = prop.GetValue<std::vector<std::string>>();
            for (auto& s : stringvector)
            {
                if (s == OldString)
                {
                    s = newstring;
                    OldString = newstring;
                    break;
                }
            }
            ExecutePropertyEditAction(prop, &stringvector, Target);
        });

    DeleteButton->SetOnPressed([this, prop, ItemBox, StringListBox, InputString, Target]()
        {
            auto text = InputString->GetText();
            std::vector<std::string> stringvector = prop.GetValue<std::vector<std::string>>();
            for (auto it = stringvector.begin(); it != stringvector.end();)
            {
                if (*it == text)
                {
                    it = stringvector.erase(it);
                    break;
                }
                else
                {
                    ++it;
                }
            }
            ExecutePropertyEditAction(prop, &stringvector, Target);
            StringListBox->RemoveChild(ItemBox, true);
        });

    ItemBox->AddChildToHorizontalBox(InputString)->SetIfAutoSize(true);
    ItemBox->AddChildToHorizontalBox(DeleteButton)->SetIfAutoSize(false);
    return ItemBox;
}

std::string UI_DetailView::GetPropertyDisplayName(const ROP::Property<ImGuiWidget::PropertyType>& prop)
{
    std::string displayName = prop.GetName();
    std::string description = prop.GetDescription();

    if (!description.empty())
    {
        // 属性可根据重要需要格式显示名称，暂时省略
        // displayName += " - " + description;
    }

    return displayName;
}

bool UI_DetailView::IsOptionalProperty(const ROP::Property<ImGuiWidget::PropertyType>& prop)
{
    try
    {
        // 尝试转换为OptionalProperty来判断是否为可选属性
        auto optionalProp = prop.GetObject()->ToOptionalProperty(prop);
        return optionalProp.IsValid() && optionalProp.IsOptional();
    }
    catch (...)
    {
        return false;
    }
}

std::vector<std::string> UI_DetailView::GetOptionalPropertyOptions(const ROP::Property<ImGuiWidget::PropertyType>& prop)
{
    std::vector<std::string> options;

    try
    {
        auto optionalProp = prop.GetObject()->ToOptionalProperty(prop);
        if (optionalProp.IsValid() && optionalProp.IsOptional())
        {
            options = optionalProp.GetOptionList();
        }
    }
    catch (...)
    {
        // 忽略异常
    }

    return options;
}

void UI_DetailView::HandleSingleProperty(
    const ROP::Property<ImGuiWidget::PropertyType>& prop,
    ImGuiWidget::ImVerticalBox* CurrentVerticalBox,
    ImGuiWidget::ImObject* Target,
    ImGuiWidget::ImWidget* WidgetOwner,
    std::unordered_map<std::string, std::function<void()>>& Updaters)
{
    std::string propName = prop.GetName();

    if (Updaters.find(propName) != Updaters.end())
    {
        AddLogLineEx(u8"重复属性[", propName, u8"]已被忽略");
        return;
    }

    std::function<void()> Updater;
    ImGuiWidget::PropertyType propType = prop.GetType();

    switch (propType)
    {
    case ImGuiWidget::PropertyType::Bool:
    {
        ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_ItemBox");
        ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
        PropertyName->SetText(GetPropertyDisplayName(prop));

        ImGuiWidget::ImCheckBox* BoolSetBox = new ImGuiWidget::ImCheckBox(m_WidgetID + "_BoolSetBox");
        BoolSetBox->SetChecked(prop.GetValue<bool>());

        BoolSetBox->SetOnToggled([this, prop, Target](bool NewSetting)
            {
                ExecutePropertyEditAction(prop, &NewSetting, Target);
            });

        ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
        ItemBox->AddChildToHorizontalBox(BoolSetBox)->SetIfAutoSize(false);
        CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);

        Updater = [BoolSetBox, prop]()
        {
            try
            {
                BoolSetBox->SetChecked(prop.GetValue<bool>());
            }
            catch (...)
            {
                // 捕获异常
            }
        };
        break;
    }
    case ImGuiWidget::PropertyType::Color:
    {
        ImGuiWidget::ImExpandableBox* StructBox = new ImGuiWidget::ImExpandableBox(m_WidgetID + "_StructBox");
        ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
        PropertyName->SetText(GetPropertyDisplayName(prop));

        ImGuiWidget::ImColorPicker* ColorPalette = new ImGuiWidget::ImColorPicker(m_WidgetID + "_ColorPalette");
        ColorPalette->SetColor(prop.GetValue<ImU32>());

        ColorPalette->SetOnColorChanged([this, prop, Target](ImU32 NewColor)
            {
                ExecutePropertyEditAction(prop, &NewColor, Target);
            });

        StructBox->SetHead(PropertyName);
        StructBox->SetBody(ColorPalette);

        Updater = [ColorPalette, prop]()
        {
            try
            {
                ColorPalette->SetColor(prop.GetValue<ImU32>());
            }
            catch (...)
            {
                // 捕获异常
            }
        };

        CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
        break;
    }
    case ImGuiWidget::PropertyType::Float:
    {
        ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_ItemBox");
        ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
        PropertyName->SetText(GetPropertyDisplayName(prop));

        ImGuiWidget::ImFloatInput* FloatInput = new ImGuiWidget::ImFloatInput(m_WidgetID + "_FloatInput");
        FloatInput->SetValue(prop.GetValue<float>());

        FloatInput->SetOnFloatValueChanged([this, prop, Target](float value)
            {
                ExecutePropertyEditAction(prop, &value, Target);
            });

        ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
        ItemBox->AddChildToHorizontalBox(FloatInput)->SetIfAutoSize(true);

        Updater = [FloatInput, prop]()
        {
            try
            {
                FloatInput->SetValue(prop.GetValue<float>());
            }
            catch (...)
            {
                // 捕获异常
            }
        };

        CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);
        break;
    }
    case ImGuiWidget::PropertyType::Int:
    {
        ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_ItemBox");
        ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
        PropertyName->SetText(GetPropertyDisplayName(prop));

        ImGuiWidget::ImIntInput* IntInput = new ImGuiWidget::ImIntInput(m_WidgetID + "_IntInput");
        IntInput->SetValue(prop.GetValue<int>());

        IntInput->SetOnIntValueChanged([this, prop, Target](int value)
            {
                ExecutePropertyEditAction(prop, &value, Target);
            });

        ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
        ItemBox->AddChildToHorizontalBox(IntInput)->SetIfAutoSize(true);

        Updater = [IntInput, prop]()
        {
            try
            {
                IntInput->SetValue(prop.GetValue<int>());
            }
            catch (...)
            {
                // 捕获异常
            }
        };

        CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);
        break;
    }
    case ImGuiWidget::PropertyType::String:
    {
        ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_ItemBox");
        ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
        PropertyName->SetText(GetPropertyDisplayName(prop));

        ImGuiWidget::ImInputText* Input = new ImGuiWidget::ImInputText(m_WidgetID + "_Input");
        Input->SetText(prop.GetValue<std::string>());

        Input->SetOnTextChanged([this, prop, Target](const std::string& text)
            {
                ExecutePropertyEditAction(prop, (void*)&text, Target);
            });

        ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
        ItemBox->AddChildToHorizontalBox(Input)->SetIfAutoSize(true);

        Updater = [Input, prop]()
        {
            try
            {
                Input->SetText(prop.GetValue<std::string>());
            }
            catch (...)
            {
                // 捕获异常
            }
        };

        CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);
        break;
    }
    case ImGuiWidget::PropertyType::Vec2:
    {
        ImGuiWidget::ImExpandableBox* StructBox = new ImGuiWidget::ImExpandableBox(m_WidgetID + "_StructBox");
        ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
        PropertyName->SetText(GetPropertyDisplayName(prop));

        ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_ItemBox");
        ImGuiWidget::ImTextBlock* Vec_X = new ImGuiWidget::ImTextBlock(m_WidgetID + "_Vec_X");
        Vec_X->SetText("X:");
        ImGuiWidget::ImTextBlock* Vec_Y = new ImGuiWidget::ImTextBlock(m_WidgetID + "_Vec_Y");
        Vec_Y->SetText("Y:");

        ImGuiWidget::ImFloatInput* X_Input = new ImGuiWidget::ImFloatInput(m_WidgetID + "_X_Imput");
        ImGuiWidget::ImFloatInput* Y_Input = new ImGuiWidget::ImFloatInput(m_WidgetID + "_Y_Imput");

        ImVec2 currentv = prop.GetValue<ImVec2>();
        X_Input->SetValue(currentv.x);
        Y_Input->SetValue(currentv.y);

        X_Input->SetOnFloatValueChanged([this, prop, Target](float NewX)
            {
                ImVec2 v = prop.GetValue<ImVec2>();
                v.x = NewX;
                ExecutePropertyEditAction(prop, (void*)&v, Target);
            });

        Y_Input->SetOnFloatValueChanged([this, prop, Target](float NewY)
            {
                ImVec2 v = prop.GetValue<ImVec2>();
                v.y = NewY;
                ExecutePropertyEditAction(prop, (void*)&v, Target);
            });

        ItemBox->AddChildToHorizontalBox(Vec_X);
        ItemBox->AddChildToHorizontalBox(X_Input);
        ItemBox->AddChildToHorizontalBox(Vec_Y);
        ItemBox->AddChildToHorizontalBox(Y_Input);

        StructBox->SetHead(PropertyName);
        StructBox->SetBody(ItemBox);

        Updater = [prop, X_Input, Y_Input]()
        {
            try
            {
                ImVec2 currentv = prop.GetValue<ImVec2>();
                X_Input->SetValue(currentv.x);
                Y_Input->SetValue(currentv.y);
            }
            catch (...)
            {
                // 捕获异常
            }
        };

        CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
        break;
    }
    case ImGuiWidget::PropertyType::Struct:
    {
        ImGuiWidget::ImExpandableBox* StructBox = new ImGuiWidget::ImExpandableBox(m_WidgetID + "_StructBox");
        ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
        PropertyName->SetText(GetPropertyDisplayName(prop));

        ImGuiWidget::ImVerticalBox* StructPropertyBox = new ImGuiWidget::ImVerticalBox(m_WidgetID + "_StructPropertyBox");
        StructBox->SetHead(PropertyName);
        StructBox->SetBody(StructPropertyBox);

        ImGuiWidget::ImObject* SubTarget = prop.GetValue<ImGuiWidget::ImObject*>();
        if (SubTarget)
        {
            std::unordered_map<std::string, std::function<void()>> SubUpdaters;
            auto allProps = SubTarget->GetAllPropertiesOrdered();

            for (const auto& subProp : allProps)
            {
                HandleSingleProperty(subProp, StructPropertyBox, SubTarget, WidgetOwner, SubUpdaters);
            }

            PropertyInfor* NewPropertyInfor = new PropertyInfor{ WidgetOwner, std::move(SubUpdaters) };
            CachedPropertyInfors.insert(std::make_pair(SubTarget, NewPropertyInfor));
        }

        Updater = []() {}; // 结构体暂不支持直接更新
        CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
        break;
    }
    case ImGuiWidget::PropertyType::StringArray:
    {
        ImGuiWidget::ImExpandableBox* StructBox = new ImGuiWidget::ImExpandableBox(m_WidgetID + "_StructBox");
        ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
        PropertyName->SetText(GetPropertyDisplayName(prop));

        ImGuiWidget::ImVerticalBox* StringListBox = new ImGuiWidget::ImVerticalBox(m_WidgetID + "_StringListBox");

        std::vector<std::string> stringArray;
        try
        {
            stringArray = prop.GetValue<std::vector<std::string>>();
        }
        catch (...)
        {
            stringArray = {};
        }

        for (auto& SingleString : stringArray)
        {
            StringListBox->AddChildToVerticalBox(
                HandleAddStringItem(prop, SingleString, StringListBox, Target, WidgetOwner)
            )->SetIfAutoSize(false);
        }

        ImGuiWidget::ImButton* AddItemButton = new ImGuiWidget::ImButton(m_WidgetID + "_AddItemButton");
        ImGuiWidget::ImTextBlock* AddButtonText = new ImGuiWidget::ImTextBlock(m_WidgetID + "_AddButtonText");
        AddButtonText->SetText("+");
        AddItemButton->SetContent(AddButtonText);

        AddItemButton->SetOnPressed([prop, StringListBox, this, Target, WidgetOwner]()
            {
                std::vector<std::string> stringvector;
                try
                {
                    stringvector = prop.GetValue<std::vector<std::string>>();
                }
                catch (...)
                {
                    stringvector = {};
                }

                std::string NewString = "NewString_" + std::to_string(stringvector.size());
                stringvector.push_back(NewString);
                ExecutePropertyEditAction(prop, (void*)&stringvector, Target);

                auto buttonptr = StringListBox->ExtractChildAt(StringListBox->GetChildNum() - 1);
                StringListBox->AddChildToVerticalBox(
                    HandleAddStringItem(prop, NewString, StringListBox, Target, WidgetOwner)
                )->SetIfAutoSize(false);
                StringListBox->AddChildToVerticalBox(buttonptr)->SetIfAutoSize(false);
            });

        StringListBox->AddChildToVerticalBox(AddItemButton)->SetIfAutoSize(false);

        StructBox->SetHead(PropertyName);
        StructBox->SetBody(StringListBox);

        Updater = [prop, StringListBox, this, Target, WidgetOwner]()
        {
            StringListBox->RemoveAllChild(true);
            std::vector<std::string> stringArray;
            try
            {
                stringArray = prop.GetValue<std::vector<std::string>>();
            }
            catch (...)
            {
                stringArray = {};
            }

            for (auto& SingleString : stringArray)
            {
                StringListBox->AddChildToVerticalBox(
                    HandleAddStringItem(prop, SingleString, StringListBox, Target, WidgetOwner)
                )->SetIfAutoSize(false);
            }
        };

        CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
        break;
    }
    case ImGuiWidget::PropertyType::Enum:
    {
        ImGuiWidget::ImHorizontalBox* StructBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_StructBox");
        ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
        PropertyName->SetText(GetPropertyDisplayName(prop));

        ImGuiWidget::ImComboBox* Options = new ImGuiWidget::ImComboBox(m_WidgetID + "_ComboBox");

        std::vector<std::string> allOptions;
        std::string selectedOption;

        try
        {
            auto optionalProp = Target->ToOptionalProperty(prop);
            if (optionalProp.IsValid() && optionalProp.IsOptional())
            {
                allOptions = optionalProp.GetOptionList();
                selectedOption = optionalProp.GetOptionString();
            }
        }
        catch (...)
        {
            // 如果无法转换为OptionalProperty，则使用回退方式
            allOptions = GetOptionalPropertyOptions(prop);
            if (!allOptions.empty())
            {
                try
                {
                    int currentIndex = prop.GetValue<int>();
                    if (currentIndex >= 0 && currentIndex < allOptions.size())
                    {
                        selectedOption = allOptions[currentIndex];
                    }
                }
                catch (...)
                {
                    // 捕获异常
                }
            }
        }

        Options->SetItems(allOptions);
        Options->SetSelectedItem(selectedOption);

        Options->SetOnSelectionChanged([this, prop, allOptions, Target](int NewIndex)
            {
                if (NewIndex >= 0 && NewIndex < allOptions.size())
                {
                    try
                    {
                        auto optionalProp = Target->ToOptionalProperty(prop);
                        if (optionalProp.IsValid() && optionalProp.IsOptional())
                        {
                            optionalProp.SetOptionByIndex(NewIndex);
                        }
                        else
                        {
                            // 直接设置新值
                            // 应该通过action发送，暂时不处理
                            //prop.SetValue<int>(NewIndex);
                        }
                        ExecutePropertyEditAction(prop, &NewIndex, Target);
                    }
                    catch (...)
                    {
                        // 捕获异常
                    }
                }
            });

        StructBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
        StructBox->AddChildToHorizontalBox(Options)->SetIfAutoSize(true);

        Updater = [prop, Options, this, Target]()
        {
            try
            {
                auto optionalProp = Target->ToOptionalProperty(prop);
                if (optionalProp.IsValid() && optionalProp.IsOptional())
                {
                    std::vector<std::string> allOptions = optionalProp.GetOptionList();
                    std::string selectedOption = optionalProp.GetOptionString();
                    Options->SetItems(allOptions);
                    Options->SetSelectedItem(selectedOption);
                }
            }
            catch (...)
            {
                // 捕获异常
            }
        };

        CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
        break;
    }
    default:
    {
        AddLogLineEx(u8"未知属性类型:widget:", WidgetOwner->GetWidgetName(), u8", 属性:", prop.GetName());
        break;
    }
    }

    if (Updater)
    {
        Updaters.insert(std::make_pair(propName, Updater));
    }
}

void UI_DetailView::SetCurrentWidget(ImGuiWidget::ImWidget* widget)
{
    CurrentWidget = widget;
    if (!widget)
    {
        SetRootWidget(nullptr, false);
        return;
    }

    auto it = CachedDetails.find(widget);
    if (it != CachedDetails.end())
    {
        SetRootWidget(it->second, false);
        return;
    }

    // 创建新的详情视图
    ImGuiWidget::ImVerticalBox* VBox = new ImGuiWidget::ImVerticalBox(widget->GetWidgetName() + "detaailvbox");
    ImGuiWidget::ImTextBlock* WidgetName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_WidgetName");
    WidgetName->SetText(widget->GetRegisterTypeName());
    VBox->AddChildToVerticalBox(WidgetName)->SetIfAutoSize(false);

    // 处理插槽属性
    if (auto Slot = widget->GetSlotAt())
    {
        ImGuiWidget::ImExpandableBox* SlotBox = new ImGuiWidget::ImExpandableBox(m_WidgetID + "_SlotBox");
        ImGuiWidget::ImTextBlock* SlotName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_SlotName");
        SlotName->SetText("SlotProperty");
        ImGuiWidget::ImVerticalBox* SlotPropertyBox = new ImGuiWidget::ImVerticalBox(m_WidgetID + "_StructPropertyBox");
        SlotBox->SetHead(SlotName);
        SlotBox->SetBody(SlotPropertyBox);

        std::unordered_map<std::string, std::function<void()>> Updaters;
        auto slotProps = Slot->GetAllPropertiesOrdered();

        for (const auto& prop : slotProps)
        {
            HandleSingleProperty(prop, SlotPropertyBox, Slot, widget, Updaters);
        }

        PropertyInfor* NewPropertyInfor = new PropertyInfor{ widget, std::move(Updaters) };
        CachedPropertyInfors.insert(std::make_pair(Slot, NewPropertyInfor));
        VBox->AddChildToVerticalBox(SlotBox)->SetIfAutoSize(false);
    }

    // 处理控件本身属性
    std::unordered_map<std::string, std::function<void()>> Updaters;
    auto properties = widget->GetAllPropertiesOrdered();

    for (const auto& prop : properties)
    {
        HandleSingleProperty(prop, VBox, widget, widget, Updaters);
    }

    PropertyInfor* NewPropertyInfor = new PropertyInfor{ widget, std::move(Updaters) };
    CachedPropertyInfors.insert(std::make_pair(widget, NewPropertyInfor));
    CachedDetails.insert(std::make_pair(widget, VBox));

    SetRootWidget(VBox, false);
}

void UI_DetailView::ExecutePropertyEditAction(const ROP::Property<ImGuiWidget::PropertyType>& prop,
    const void* NewValue, ImGuiWidget::ImObject* Target)
{
    // 注意：这里需要将ROP::Property转换为某种可序列化格式
    // 但ROP::Property已经包含所有需要信息，所以可以从中提取关键信息
    ExecuteAction(EditedFileFullPath + Action::DetailView::_REQUEST_EDIT_PROPERTY,
        prop.GetName(), prop.GetClassName(), NewValue, Target);

    // 更新属性显示
    UpdatePropertyDisplay(Target, prop.GetName());
}

void UI_DetailView::UpdatePropertyDisplay(ImGuiWidget::ImObject* Target, const std::string& PropertyName)
{
    auto it = CachedPropertyInfors.find(Target);
    if (it == CachedPropertyInfors.end())
    {
        AddLogLineEx(u8"更新属性显示失败：Target[", Target, "], Name[", PropertyName, "]");
        return;
    }

    auto UpdaterIt = it->second->Updaters.find(PropertyName);
    if (UpdaterIt == it->second->Updaters.end())
    {
        // 可能是嵌套属性，不需要更新
        AddLogLineEx(u8"更新属性显示失败：Widget[", it->second->WidgetOwner->GetWidgetName(),
            "] Target[", Target, "], Name[", PropertyName, "]");
        return;
    }

    try
    {
        UpdaterIt->second();
    }
    catch (...)
    {
        AddLogLineEx(u8"更新属性时发生异常：", PropertyName);
    }
}
