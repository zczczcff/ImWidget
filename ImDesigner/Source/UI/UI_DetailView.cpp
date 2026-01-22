#include "UI/UI_DetailView.h"
#include "ImWidget/ImBasicWidgetList.h"
#include "Tools/JLog.h"
#include "EditorAction.h"

void UI_DetailView::OnKeyDown(ImGuiWidget::ImKeyDownEvent& e) 
{
	if (bHasFocus && e.IsCtrl_Z())
	{
		OnRequestUndo.Broadcast();
	}
}

void UI_DetailView::ActionInit()
{
	ResetFileAction();
	AddSequentialProcessor(Action::ProjectView::RENAME_FILE, [this](const std::string& OldFileFullPath,const std::string& NewFileFullPath) 
		{
			if (EditedFileFullPath == OldFileFullPath)
			{
				EditedFileFullPath = NewFileFullPath;
				ResetFileAction();
			}
		});
}

void UI_DetailView::ResetFileAction()
{
	for (auto& id : FileActions)
	{
		RemoveProcessor(id);
	}
	FileActions.clear();

	FileActions.push_back(AddSequentialProcessor(EditedFileFullPath + Action::WIDGET_SELECTED, [this](ImGuiWidget::ImWidget* SelectedWidget)
		{
			SetCurrentWidget(SelectedWidget);
		}));
}

ImGuiWidget::ImHorizontalBox* UI_DetailView::HandleAddStringItem(
	const ImGuiWidget::PropertyInfo& SingleProperty,
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
	InputString->SetOnTextChanged([this,SingleProperty, OldString = SingleString, Target](const std::string& newstring) mutable
		{
			std::vector<std::string> stringvector = *(std::vector<std::string>*)SingleProperty.getter();
			for (auto& s : stringvector)
			{
				if (s == OldString)
				{
					s = newstring;
					OldString = newstring;
				}
			}
			//SingleProperty.setter(&stringvector);
			OnPropertyChanged.Broadcast(SingleProperty, &stringvector, Target);
		});
	DeleteButton->SetOnPressed([this,SingleProperty, ItemBox, StringListBox, InputString, Target]()
		{
			auto text = InputString->GetText();
			std::vector<std::string> stringvector = *(std::vector<std::string>*)SingleProperty.getter();
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
			//SingleProperty.setter(&stringvector);
			OnPropertyChanged.Broadcast(SingleProperty, &stringvector, Target);
			StringListBox->RemoveChild(ItemBox, true);
		});

	ItemBox->AddChildToHorizontalBox(InputString)->SetIfAutoSize(true);
	ItemBox->AddChildToHorizontalBox(DeleteButton)->SetIfAutoSize(false);
	return ItemBox;
}

void UI_DetailView::HandleSingleProperty(
	const ImGuiWidget::PropertyInfo& SingleProperty, 
	ImGuiWidget::ImVerticalBox* CurrentVerticalBox,
	ImGuiWidget::ImObject* Target,
	ImGuiWidget::ImWidget* WidgetOwner,
	std::unordered_map<std::string, std::function<void()>>& Updaters)
{
	if (Updaters.find(SingleProperty.name) != Updaters.end())
	{
		AddLogLineEx(u8"警告：属性[", SingleProperty.name, u8"]已被添加");
		return;
	}
	std::function<void()> Updater;
	switch (SingleProperty.type)
	{
	case ImGuiWidget::PropertyType::Bool:
	{
		ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_ItemBox");
		ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
		PropertyName->SetText(SingleProperty.name);
		ImGuiWidget::ImCheckBox* BoolSetBox = new ImGuiWidget::ImCheckBox(m_WidgetID + "_BoolSetBox");
		BoolSetBox->SetChecked(*(bool*)SingleProperty.getter());
		BoolSetBox->SetOnToggled([this,SingleProperty, Target](bool NewSetting)
			{
				//SingleProperty.setter(&NewSetting);
				OnPropertyChanged.Broadcast(SingleProperty, &NewSetting,Target);
			});
		ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
		ItemBox->AddChildToHorizontalBox(BoolSetBox)->SetIfAutoSize(false);
		CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);
		Updater = [BoolSetBox, SingleProperty]() { BoolSetBox->SetChecked(*(bool*)SingleProperty.getter()); };
		break;
	}
	case ImGuiWidget::PropertyType::Color:
	{
		ImGuiWidget::ImExpandableBox* StructBox = new ImGuiWidget::ImExpandableBox(m_WidgetID + "_StructBox");
		ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
		PropertyName->SetText(SingleProperty.name);
		ImGuiWidget::ImColorPicker* ColorPalette = new ImGuiWidget::ImColorPicker(m_WidgetID + "_ColorPalette");
		ColorPalette->SetColor(*(ImU32*)SingleProperty.getter());
		ColorPalette->SetOnColorChanged([this,SingleProperty, Target](ImU32 NewColor)
			{
				//SingleProperty.setter(&NewColor); 
				OnPropertyChanged.Broadcast(SingleProperty, &NewColor, Target);
			});

		StructBox->SetHead(PropertyName);
		StructBox->SetBody(ColorPalette);
		Updater = [ColorPalette, SingleProperty]() { ColorPalette->SetColor(*(ImU32*)SingleProperty.getter()); };
		CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
		break;
	}
	case ImGuiWidget::PropertyType::Float:
	{
		ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_ItemBox");
		ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
		PropertyName->SetText(SingleProperty.name);
		ImGuiWidget::ImFloatInput* FloatInput = new ImGuiWidget::ImFloatInput(m_WidgetID + "_FloatInput");
		FloatInput->SetValue(*(float*)SingleProperty.getter());
		FloatInput->SetOnFloatValueChanged([this,SingleProperty, Target](float value)
			{
				//SingleProperty.setter(&value);
				OnPropertyChanged.Broadcast(SingleProperty, &value, Target);
			});

		ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
		ItemBox->AddChildToHorizontalBox(FloatInput)->SetIfAutoSize(true);
		Updater = [FloatInput, SingleProperty]() { FloatInput->SetValue(*(float*)SingleProperty.getter()); };
		CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);
		break;
	}
	case ImGuiWidget::PropertyType::Int:
	{
		ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_ItemBox");
		ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
		PropertyName->SetText(SingleProperty.name);
		ImGuiWidget::ImIntInput* IntInput = new ImGuiWidget::ImIntInput(m_WidgetID + "_IntInput");
		IntInput->SetValue(*(int*)SingleProperty.getter());
		IntInput->SetOnIntValueChanged([this,SingleProperty, Target](int value)
			{
				//SingleProperty.setter(&value);
				OnPropertyChanged.Broadcast(SingleProperty, &value, Target);
			});

		ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
		ItemBox->AddChildToHorizontalBox(IntInput)->SetIfAutoSize(true);
		Updater = [IntInput, SingleProperty]() { IntInput->SetValue(*(int*)SingleProperty.getter()); };
		CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);
		break;
	}
	case ImGuiWidget::PropertyType::String:
	{
		ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_ItemBox");
		ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
		PropertyName->SetText(SingleProperty.name);
		ImGuiWidget::ImInputText* Input = new ImGuiWidget::ImInputText(m_WidgetID + "_Input");
		Input->SetText(*(std::string*)SingleProperty.getter());
		Input->SetOnTextChanged([this,SingleProperty, Target](const std::string& text)
			{
				//SingleProperty.setter((void*)&text);
				OnPropertyChanged.Broadcast(SingleProperty, (void*)&text, Target);
			});

		ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
		ItemBox->AddChildToHorizontalBox(Input)->SetIfAutoSize(true);
		Updater = [Input, SingleProperty]() { Input->SetText(*(std::string*)SingleProperty.getter()); };
		CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);
		break;
	}
	case ImGuiWidget::PropertyType::Struct:
	{
		ImGuiWidget::ImExpandableBox* StructBox = new ImGuiWidget::ImExpandableBox(m_WidgetID + "_StructBox");
		ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
		PropertyName->SetText(SingleProperty.name);
		ImGuiWidget::ImVerticalBox* StructPropertyBox = new ImGuiWidget::ImVerticalBox(m_WidgetID + "_StructPropertyBox");
		StructBox->SetHead(PropertyName);
		StructBox->SetBody(StructPropertyBox);
		ImGuiWidget::ImObject* SubTarget = (ImGuiWidget::ImObject*)(SingleProperty.getter());

		std::unordered_map<std::string, std::function<void()>> SubUpdaters;
		for (auto& SubSingleProperty : SubTarget->GetProperties())
		{
			HandleSingleProperty(SubSingleProperty, StructPropertyBox, SubTarget,WidgetOwner, SubUpdaters);
		}
		PropertyInfor* NewPropertyInfor = new PropertyInfor{ WidgetOwner,std::move(SubUpdaters) };
		CachedPropertyInfors.insert(std::make_pair(SubTarget, NewPropertyInfor));

		Updater = []() {};//暂时应该不会直接更新整个结构
		CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
		break;
	}
	case ImGuiWidget::PropertyType::Vec2:
	{
		ImGuiWidget::ImExpandableBox* StructBox = new ImGuiWidget::ImExpandableBox(m_WidgetID + "_StructBox");
		ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
		PropertyName->SetText(SingleProperty.name);
		ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_ItemBox");
		ImGuiWidget::ImTextBlock* Vec_X = new ImGuiWidget::ImTextBlock(m_WidgetID + "_Vec_X");
		Vec_X->SetText("X:");
		ImGuiWidget::ImTextBlock* Vec_Y = new ImGuiWidget::ImTextBlock(m_WidgetID + "_Vec_Y");
		Vec_Y->SetText("Y:");
		ImGuiWidget::ImFloatInput* X_Input = new ImGuiWidget::ImFloatInput(m_WidgetID + "_X_Imput");
		ImGuiWidget::ImFloatInput* Y_Input = new ImGuiWidget::ImFloatInput(m_WidgetID + "_Y_Imput");

		ImVec2 currentv = *(ImVec2*)SingleProperty.getter();
		X_Input->SetValue(currentv.x);
		Y_Input->SetValue(currentv.y);
		X_Input->SetOnFloatValueChanged([this,SingleProperty, Target](float NewX)
			{
				ImVec2 v = *(ImVec2*)SingleProperty.getter();
				v.x = NewX;
				//SingleProperty.setter(&v);
				OnPropertyChanged.Broadcast(SingleProperty, (void*)&v, Target);
			});

		Y_Input->SetOnFloatValueChanged([this,SingleProperty, Target](float NewY)
			{
				ImVec2 v = *(ImVec2*)SingleProperty.getter();
				v.y = NewY;
				SingleProperty.setter(&v);
				OnPropertyChanged.Broadcast(SingleProperty, (void*)&v, Target);
			});
		ItemBox->AddChildToHorizontalBox(Vec_X);
		ItemBox->AddChildToHorizontalBox(X_Input);
		ItemBox->AddChildToHorizontalBox(Vec_Y);
		ItemBox->AddChildToHorizontalBox(Y_Input);

		StructBox->SetHead(PropertyName);
		StructBox->SetBody(ItemBox);
		Updater = [SingleProperty, X_Input, Y_Input]() {
			ImVec2 currentv = *(ImVec2*)SingleProperty.getter();
			X_Input->SetValue(currentv.x);
			Y_Input->SetValue(currentv.y);
		};
		CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
		break;
	}
	case ImGuiWidget::PropertyType::StringArray:
	{
		ImGuiWidget::ImExpandableBox* StructBox = new ImGuiWidget::ImExpandableBox(m_WidgetID + "_StructBox");
		ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
		PropertyName->SetText(SingleProperty.name);
		ImGuiWidget::ImVerticalBox* StringListBox = new ImGuiWidget::ImVerticalBox(m_WidgetID + "_StringListBox");

		for (auto& SingleString : *(std::vector<std::string>*)SingleProperty.getter())
		{
			StringListBox->AddChildToVerticalBox(HandleAddStringItem(SingleProperty, SingleString, StringListBox, Target,WidgetOwner))->SetIfAutoSize(false);
		}
		ImGuiWidget::ImButton* AddItemButton = new ImGuiWidget::ImButton(m_WidgetID + "_AddItemButton");
		ImGuiWidget::ImTextBlock* AddButtonText = new ImGuiWidget::ImTextBlock(m_WidgetID + "_AddButtonText");
		AddButtonText->SetText("+");
		AddItemButton->SetContent(AddButtonText);
		AddItemButton->SetOnPressed([SingleProperty, StringListBox, this, Target, WidgetOwner]()
			{
				std::vector<std::string> stringvector = *(std::vector<std::string>*)SingleProperty.getter();
				std::string NewString = "NewString_" + std::to_string(stringvector.size());
				stringvector.push_back(NewString);
				//SingleProperty.setter(&stringvector);
				OnPropertyChanged.Broadcast(SingleProperty, (void*)&stringvector, Target);
				auto buttonptr = StringListBox->ExtractChildAt(StringListBox->GetChildNum() - 1);
				StringListBox->AddChildToVerticalBox(HandleAddStringItem(SingleProperty, NewString, StringListBox, Target,WidgetOwner))->SetIfAutoSize(false);
				StringListBox->AddChildToVerticalBox(buttonptr)->SetIfAutoSize(false);
			});
		StringListBox->AddChildToVerticalBox(AddItemButton)->SetIfAutoSize(false);

		StructBox->SetHead(PropertyName);
		StructBox->SetBody(StringListBox);
		Updater = [SingleProperty, StringListBox,this, Target, WidgetOwner]()
		{
			StringListBox->RemoveAllChild(true);
			for (auto& SingleString : *(std::vector<std::string>*)SingleProperty.getter())
			{
				StringListBox->AddChildToVerticalBox(HandleAddStringItem(SingleProperty, SingleString, StringListBox, Target, WidgetOwner))->SetIfAutoSize(false);
			}
		};
		CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
		break;
	}
	case ImGuiWidget::PropertyType::Enum:
	{
		ImGuiWidget::ImHorizontalBox* StructBox = new ImGuiWidget::ImHorizontalBox(m_WidgetID + "_StructBox");
		ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_PropertyName");
		PropertyName->SetText(SingleProperty.name);
		ImGuiWidget::ImComboBox* Options = new ImGuiWidget::ImComboBox(m_WidgetID + "_ComboBox");

		std::vector<std::string> AllOptionsCopy = *(std::vector<std::string>*)SingleProperty.getter();
		std::string SelectedOption = AllOptionsCopy.back();
		AllOptionsCopy.pop_back();
		Options->SetItems(AllOptionsCopy);
		Options->SetSelectedItem(SelectedOption);

		Options->SetOnSelectionChanged([this,SingleProperty, AllOptionsCopy, Target](int NewIndex)
			{
				std::string OptionSelect = AllOptionsCopy[NewIndex];
				//SingleProperty.setter(&OptionSelect);
				OnPropertyChanged.Broadcast(SingleProperty, (void*)&OptionSelect, Target);
			});
		StructBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
		StructBox->AddChildToHorizontalBox(Options)->SetIfAutoSize(true);
		Updater = [SingleProperty, Options]()
		{
			std::vector<std::string> AllOptionsCopy = *(std::vector<std::string>*)SingleProperty.getter();
			std::string SelectedOption = AllOptionsCopy.back();
			AllOptionsCopy.pop_back();
			Options->SetItems(AllOptionsCopy);
			Options->SetSelectedItem(SelectedOption);
		};
		CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
		break;
	}
	default:
	{
		AddLogLineEx(u8"警告：未知属性类型:widget:",WidgetOwner->GetWidgetName());
		break;
	}
		
	}

	Updaters.insert(std::make_pair(SingleProperty.name, Updater));
}

void UI_DetailView::SetCurrentWidget(ImGuiWidget::ImWidget* widget)
{
	CurrentWidget = widget;
	if (!widget)
	{
		SetRootWidget(nullptr, false);
		//m_RootScrollBox->SetContent(nullptr, false);
		return;
	}
	auto it = CachedDetails.find(widget);
	if (it != CachedDetails.end())
	{
		SetRootWidget(it->second, false);
		//m_RootScrollBox->SetContent(it->second, false);
		return;
	}
	//if (!VBox)return;

	//VBox->RemoveAllChild();
	
	//控件名称
	ImGuiWidget::ImVerticalBox* VBox = new ImGuiWidget::ImVerticalBox(widget->GetWidgetName() + "detaailvbox");
	ImGuiWidget::ImTextBlock* WidgetName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_WidgetName");
	WidgetName->SetText(widget->GetRegisterTypeName());
	VBox->AddChildToVerticalBox(WidgetName)->SetIfAutoSize(false);

	//控件父项槽属性
	if (auto Slot = widget->GetSlotAt())
	{
		ImGuiWidget::ImExpandableBox* SlotBox = new ImGuiWidget::ImExpandableBox(m_WidgetID + "_SlotBox");
		ImGuiWidget::ImTextBlock* SlotName = new ImGuiWidget::ImTextBlock(m_WidgetID + "_SlotName");
		SlotName->SetText("SlotProperty");
		ImGuiWidget::ImVerticalBox* SlotPropertyBox = new ImGuiWidget::ImVerticalBox(m_WidgetID + "_StructPropertyBox");
		SlotBox->SetHead(SlotName);
		SlotBox->SetBody(SlotPropertyBox);
		std::unordered_map<std::string, std::function<void()>> Updaters;
		for (auto& SubSingleProperty : Slot->GetProperties())
		{
			HandleSingleProperty(SubSingleProperty, SlotPropertyBox, Slot, widget, Updaters);
		}
		PropertyInfor* NewPropertyInfor = new PropertyInfor{ widget,std::move(Updaters) };
		CachedPropertyInfors.insert(std::make_pair(Slot, NewPropertyInfor));
		VBox->AddChildToVerticalBox(SlotBox)->SetIfAutoSize(false);
	}

	//控件自身属性
	std::unordered_map<std::string, std::function<void()>> Updaters;
	auto properties = widget->GetProperties();
	for (auto& SingleProperty : properties)
	{
		HandleSingleProperty(SingleProperty, VBox, widget, widget, Updaters);
	}
	PropertyInfor* NewPropertyInfor = new PropertyInfor{ widget,std::move(Updaters) };
	CachedPropertyInfors.insert(std::make_pair(widget, NewPropertyInfor));
	CachedDetails.insert(std::make_pair(widget, VBox));

	//m_RootScrollBox->SetContent(VBox, false);
	SetRootWidget(VBox, false);
}

void UI_DetailView::UpdatePropertyDisplay(ImGuiWidget::ImObject* Target, const std::string& PropertyName)
{
	auto it = CachedPropertyInfors.find(Target);
	if (it == CachedPropertyInfors.end())
	{
		AddLogLineEx(u8"更新属性显示失败：Target[", Target, "],Name[", PropertyName, "]");
		return;
	}

	auto UpdaterIt = it->second->Updaters.find(PropertyName);
	if (UpdaterIt == it->second->Updaters.end())
	{
		AddLogLineEx(u8"更新属性显示失败：Widget[",it->second->WidgetOwner->GetWidgetName(),"]Target[", Target, "],Name[", PropertyName, "]");
		return;
	}
	UpdaterIt->second();
}
