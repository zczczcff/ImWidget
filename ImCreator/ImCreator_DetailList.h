#pragma once
#include "ImWidget/ImUserWidget.h"
#include "ImWidget/ImVerticalBox.h"
#include "ImWidget/ImExpandableBox.h"
#include "ImWidget/ImHorizontalBox.h"
#include "ImWidget/ImTextBlock.h"
#include "Imwidget/ImCheckBox.h"
#include "ImWidget/ImColorPicker.h"

class DetailList :public ImGuiWidget::ImUserWidget
{
private:
	ImGuiWidget::ImVerticalBox* m_VerticalBox;
	ImGuiWidget::ImWidget* CurrentWidget;
public:
	DetailList(const std::string& WidgetName)
		:ImUserWidget(WidgetName),
		m_VerticalBox(new ImGuiWidget::ImVerticalBox(WidgetName + "_VerticalBox")),
		CurrentWidget(nullptr)
	{
		SetRootWidget(m_VerticalBox);
	}

	void HandleSingleProperty(const ImGuiWidget::PropertyInfo& SingleProperty, ImGuiWidget::ImVerticalBox* CurrentVerticalBox)
	{
		switch (SingleProperty.type)
		{
		case ImGuiWidget::PropertyType::Bool:
		{
			ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetName + "_ItemBox");
			ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetName + "_PropertyName");
			PropertyName->SetText(SingleProperty.name);
			ImGuiWidget::ImCheckBox* BoolSetBox = new ImGuiWidget::ImCheckBox(m_WidgetName + "_BoolSetBox");
			BoolSetBox->SetChecked(*(bool*)SingleProperty.getter());
			BoolSetBox->SetOnToggled([SingleProperty](bool NewSetting)
				{
					SingleProperty.setter(&NewSetting);
				});
			ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
			ItemBox->AddChildToHorizontalBox(BoolSetBox)->SetIfAutoSize(false);
			CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);
			break;
		}
		case ImGuiWidget::PropertyType::Color:
		{
			ImGuiWidget::ImExpandableBox* StructBox = new ImGuiWidget::ImExpandableBox(m_WidgetName + "_StructBox");
			ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetName + "_PropertyName");
			PropertyName->SetText(SingleProperty.name);
			ImGuiWidget::ImColorPicker* ColorPalette = new ImGuiWidget::ImColorPicker(m_WidgetName + "_ColorPalette");
			ColorPalette->SetColor(*(ImU32*)SingleProperty.getter());
			ColorPalette->SetOnColorChanged([SingleProperty](ImU32 NewColor) {SingleProperty.setter(&NewColor); });

			StructBox->SetHead(PropertyName);
			StructBox->SetBody(ColorPalette);

			CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
			break;
		}
		case ImGuiWidget::PropertyType::Float:
		{
			ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetName + "_ItemBox");
			ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetName + "_PropertyName");
			PropertyName->SetText(SingleProperty.name);
			ImGuiWidget::ImFloatInput* FloatInput = new ImGuiWidget::ImFloatInput(m_WidgetName + "_FloatInput");
			FloatInput->SetValue(*(float*)SingleProperty.getter());
			FloatInput->SetOnFloatValueChanged([SingleProperty](float value)
				{
					SingleProperty.setter(&value);
				});

			ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
			ItemBox->AddChildToHorizontalBox(FloatInput)->SetIfAutoSize(true);
			CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);
			break;
		}
		case ImGuiWidget::PropertyType::Int:
		{
			ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetName + "_ItemBox");
			ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetName + "_PropertyName");
			PropertyName->SetText(SingleProperty.name);
			ImGuiWidget::ImIntInput* FloatInput = new ImGuiWidget::ImIntInput(m_WidgetName + "_IntInput");
			FloatInput->SetValue(*(int*)SingleProperty.getter());
			FloatInput->SetOnIntValueChanged([SingleProperty](int value)
				{
					SingleProperty.setter(&value);
				});

			ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
			ItemBox->AddChildToHorizontalBox(FloatInput)->SetIfAutoSize(true);
			CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);
			break;
		}
		case ImGuiWidget::PropertyType::String:
		{
			ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetName + "_ItemBox");
			ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetName + "_PropertyName");
			PropertyName->SetText(SingleProperty.name);
			ImGuiWidget::ImInputText* Input = new ImGuiWidget::ImInputText(m_WidgetName + "_Input");
			Input->SetText(*(std::string*)SingleProperty.getter());
			Input->SetOnTextChanged([SingleProperty](const std::string& text)
				{
					SingleProperty.setter((void*)&text);
				});

			ItemBox->AddChildToHorizontalBox(PropertyName)->SetIfAutoSize(true);
			ItemBox->AddChildToHorizontalBox(Input)->SetIfAutoSize(true);
			CurrentVerticalBox->AddChildToVerticalBox(ItemBox)->SetIfAutoSize(false);
			break;
		}
		case ImGuiWidget::PropertyType::Struct:
		{
			ImGuiWidget::ImExpandableBox* StructBox = new ImGuiWidget::ImExpandableBox(m_WidgetName + "_StructBox");
			ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetName + "_PropertyName");
			PropertyName->SetText(SingleProperty.name);
			ImGuiWidget::ImVerticalBox* StructPropertyBox = new ImGuiWidget::ImVerticalBox(m_WidgetName + "_StructPropertyBox");
			StructBox->SetHead(PropertyName);
			StructBox->SetBody(StructPropertyBox);
			for (auto& SubSingleProperty : ((ImGuiWidget::PropertyStruct*)(SingleProperty.getter()))->GetProperties())
			{
				HandleSingleProperty(SubSingleProperty, StructPropertyBox);
			}
			CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
			break;
		}
		case ImGuiWidget::PropertyType::Vec2:
		{
			ImGuiWidget::ImExpandableBox* StructBox = new ImGuiWidget::ImExpandableBox(m_WidgetName + "_StructBox");
			ImGuiWidget::ImTextBlock* PropertyName = new ImGuiWidget::ImTextBlock(m_WidgetName + "_PropertyName");
			PropertyName->SetText(SingleProperty.name);
			ImGuiWidget::ImHorizontalBox* ItemBox = new ImGuiWidget::ImHorizontalBox(m_WidgetName + "_ItemBox");
			ImGuiWidget::ImTextBlock* Vec_X = new ImGuiWidget::ImTextBlock(m_WidgetName + "_Vec_X");
			Vec_X->SetText("X:");
			ImGuiWidget::ImTextBlock* Vec_Y = new ImGuiWidget::ImTextBlock(m_WidgetName + "_Vec_Y");
			Vec_Y->SetText("Y:");
			ImGuiWidget::ImFloatInput* X_Input = new ImGuiWidget::ImFloatInput(m_WidgetName + "_X_Imput");
			ImGuiWidget::ImFloatInput* Y_Input = new ImGuiWidget::ImFloatInput(m_WidgetName + "_Y_Imput");

			ImVec2 currentv = *(ImVec2*)SingleProperty.getter();
			X_Input->SetValue(currentv.x);
			Y_Input->SetValue(currentv.y);
			X_Input->SetOnFloatValueChanged([SingleProperty](float NewX) 
				{
					ImVec2 v = *(ImVec2*)SingleProperty.getter();
					v.x = NewX;
					SingleProperty.setter(&v);
				});

			Y_Input->SetOnFloatValueChanged([SingleProperty](float NewY)
				{
					ImVec2 v = *(ImVec2*)SingleProperty.getter();
					v.y = NewY;
					SingleProperty.setter(&v);
				});
			ItemBox->AddChildToHorizontalBox(Vec_X);
			ItemBox->AddChildToHorizontalBox(X_Input);
			ItemBox->AddChildToHorizontalBox(Vec_Y);
			ItemBox->AddChildToHorizontalBox(Y_Input);

			StructBox->SetHead(PropertyName);
			StructBox->SetBody(ItemBox);
			CurrentVerticalBox->AddChildToVerticalBox(StructBox)->SetIfAutoSize(false);
			break;
		}
		default:
			break;
		}
	}

	void SetCurrentWidget(ImGuiWidget::ImWidget* widget)
	{
		CurrentWidget = widget;
		m_VerticalBox->RemoveAllChild();
		if (!widget)return;

		ImGuiWidget::ImTextBlock* WidgetName = new ImGuiWidget::ImTextBlock(m_WidgetName + "_WidgetName");
		WidgetName->SetText(widget->GetWidgetName());
		m_VerticalBox->AddChildToVerticalBox(WidgetName)->SetIfAutoSize(false);

		if (auto Slot = widget->GetSlot())
		{
			ImGuiWidget::ImExpandableBox* SlotBox = new ImGuiWidget::ImExpandableBox(m_WidgetName + "_SlotBox");
			ImGuiWidget::ImTextBlock* SlotName = new ImGuiWidget::ImTextBlock(m_WidgetName + "_SlotName");
			SlotName->SetText("SlotProperty");
			ImGuiWidget::ImVerticalBox* SlotPropertyBox = new ImGuiWidget::ImVerticalBox(m_WidgetName + "_StructPropertyBox");
			SlotBox->SetHead(SlotName);
			SlotBox->SetBody(SlotPropertyBox);
			for (auto& SubSingleProperty : Slot->GetProperties())
			{
				HandleSingleProperty(SubSingleProperty, SlotPropertyBox);
			}
			m_VerticalBox->AddChildToVerticalBox(SlotBox)->SetIfAutoSize(false);
		}


		for (auto& SingleProperty:widget->GetProperties())
		{
			HandleSingleProperty(SingleProperty, m_VerticalBox);
		}
	}
};