#include "Model/Model_UIFile.h"
#include "Model/FileUtil.h"
#include "Model/Command/CommandManager.h"

Model_UIFile::Model_UIFile(const std::string& Path) :
	Target(new ImGuiWidget::ImUserWidgetClass),
	Path(Path),
	m_EditCommandManager(new EditCommandManager)
{
	if (FileUtil::IsFileExist(Path))//文件存在，则读取
	{
		std::ifstream file(Path);
		if (!file.is_open()) return;

		nlohmann::ordered_json j = nlohmann::ordered_json::parse(file);
		Target->FromJson(j);
	}

	m_EditCommandManager->OnPropertyEditUnDoRedo.Add([this](ImGuiWidget::ImObject* target, const std::string& propertyname)
		{
			OnPropertyEditUnDoRedo.Broadcast(target, propertyname);
			UpdateUndoRedoState();
		});

	m_EditCommandManager->OnChildEditUndoRedo.Add([this]()
		{
			OnWidgetTreeChanged.Broadcast();
			UpdateUndoRedoState();
		});

	m_EditCommandManager->OnVarRename.Add([this](const std::string& OldName, const std::string& NewName) 
		{
			OnVarRename.Broadcast(OldName, NewName);
		});

}

void Model_UIFile::Undo()
{
	m_EditCommandManager->Undo();
}

void Model_UIFile::Redo()
{
	m_EditCommandManager->Redo();
}

void Model_UIFile::UpdateUndoRedoState()
{
	OnUndoRedoStateChanged.Broadcast(m_EditCommandManager->CanUndo(), m_EditCommandManager->CanRedo());
}

bool Model_UIFile::RemoveChildWidget(const std::string& varName, ImGuiWidget::ImWidget* WidgetToRemove)
{
	auto it = AllWidgetEditors.find(varName);
	if (it != AllWidgetEditors.end())
	{
		return it->second->RemoveChildWidget(WidgetToRemove);
	}
	return false;
}

bool Model_UIFile::InsertChildTo(const std::string& varName, ImGuiWidget::ImWidget* child, ImGuiWidget::ImWidget* Target, int InsertIndex)
{
	auto it = AllWidgetEditors.find(varName);
	if (it != AllWidgetEditors.end())
	{
		return it->second->InsertChildTo(child, Target, InsertIndex);
	}
	return false;
}

bool Model_UIFile::InsertChildTo(const std::string& varName, const std::string& WidgetRegisterName, ImGuiWidget::ImWidget* Target, int InsertIndex)
{
	auto it = AllWidgetEditors.find(varName);
	if (it != AllWidgetEditors.end())
	{
		return it->second->InsertChildTo(WidgetRegisterName, Target, InsertIndex);
	}
	return false;
}

bool Model_UIFile::EditProperty(const std::string& varName, const ImGuiWidget::PropertyInfo& propInfo, const void* NewValue, ImGuiWidget::ImObject* Target)
{
	auto it = AllWidgetEditors.find(varName);
	if (it != AllWidgetEditors.end())
	{
		
		return it->second->EditProperty(propInfo, NewValue, Target);
	}
	return false;
}

bool Model_UIFile::RenameVar(const std::string& OldName, const std::string& NewName)
{
	return m_EditCommandManager->ExcuteVarRename(Target, OldName, NewName);
}

bool Model_UIFile::AddWidgetVar(const std::string& varName, const std::string& RootWidgetRegisterName)
{

	return false;
}

bool Model_UIFile::CreateWidgetVar(const std::string& RootWidgetRegisterType)
{
	int i = 0;
	while (ExistedVarName.find(RootWidgetRegisterType + std::to_string(i)) != ExistedVarName.end())
	{
		i++;
	}
	const std::string varName = RootWidgetRegisterType + std::to_string(i);
	if (auto NewVar = Target->AddWidgetVar(varName, RootWidgetRegisterType))
	{
		Model_WidgetEditor* NewWidgetEditor = new Model_WidgetEditor(NewVar->var.v_widget, ExistedVarName);
		AllWidgetEditors.insert(std::make_pair(varName, NewWidgetEditor));
		return true;
	}
	else
	{
		return false;
	}
}
