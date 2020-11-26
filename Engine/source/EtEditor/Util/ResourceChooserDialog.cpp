#include "stdafx.h"
#include "ResourceChooserDialog.h"

#include <EtCore/FileSystem/Entry.h>
#include <EtCore/FileSystem/FileUtil.h>

#include <EtEditor/Content/FileResourceManager.h>


namespace et {
namespace edit {


//=========================
// Resource Chooser Dialog
//=========================


//static
//---------------------------------
// ResourceChooserDialog::create
//
ResourceChooserDialog* ResourceChooserDialog::create()
{
	// Load the Builder file and instantiate its widgets.
	Glib::RefPtr<Gtk::Builder> const refBuilder = Gtk::Builder::create_from_resource("/com/leah-lindner/editor/ui/resourceChooser.ui");

	ResourceChooserDialog* dialog = nullptr;
	refBuilder->get_widget_derived("resource_dialog", dialog);
	ET_ASSERT(dialog != nullptr, "No 'resource_dialog' object in resourceChooser.ui");

	return dialog;
}


//----------------------------------------------
// ResourceChooserDialog::ResourceChooserDialog
//
// Settings Dialog default constructor
//
ResourceChooserDialog::ResourceChooserDialog(BaseObjectType* cobject, Glib::RefPtr<Gtk::Builder> const& refBuilder)
	: Gtk::Dialog(cobject)
	, m_RefBuilder(refBuilder)
{
	// area for displaying asset widgets
	m_RefBuilder->get_widget("resource_view", m_FlowBox);
	ET_ASSERT(m_FlowBox != nullptr);

	// filtering
	Gtk::MenuButton* filterButton;
	m_RefBuilder->get_widget("filter", filterButton);
	ET_ASSERT(filterButton != nullptr);
	m_TypeFilter.RegisterListener(this); // no need to unregister ourselves because we own it
	m_TypeFilter.Init(filterButton);

	m_RefBuilder->get_widget("search", m_SearchBar);
	ET_ASSERT(m_SearchBar != nullptr);

	m_SearchBar->signal_search_changed().connect(sigc::mem_fun(*this, &ResourceChooserDialog::OnSearchChanged));

	// toggle view of engine or project resources
	m_RefBuilder->get_widget("engine_button", m_EngineButton);
	ET_ASSERT(m_EngineButton != nullptr);
	m_RefBuilder->get_widget("project_button", m_ProjectButton);
	ET_ASSERT(m_ProjectButton != nullptr);
	
	m_EngineButton->signal_toggled().connect(sigc::mem_fun(*this, &ResourceChooserDialog::ResourceGroupToggled));
	m_ProjectButton->signal_toggled().connect(sigc::mem_fun(*this, &ResourceChooserDialog::ResourceGroupToggled));
	if (m_ProjectSelected)
	{
		m_ProjectButton->set_active(true);
	}
	else
	{
		m_EngineButton->set_active(true);
	}

	// directory tree
	m_RefBuilder->get_widget("directory_tree", m_TreeView);
	ET_ASSERT(m_TreeView != nullptr);

	m_TreeModel = Gtk::TreeStore::create(m_Columns);

	m_TreeView->set_model(m_TreeModel);

	m_TreeSelection = m_TreeView->get_selection();
	m_TreeSelection->signal_changed().connect(sigc::mem_fun(*this, &ResourceChooserDialog::OnDirectorySelectionChanged));

	m_TreeView->append_column("Directory", m_Columns.m_Name);

	RebuildDirectoryTree();
}

//------------------------------------------------
// ResourceChooserDialog::ResourceGroupToggled
//
void ResourceChooserDialog::ResourceGroupToggled()
{
	bool changed = false;

	if (m_ProjectButton->get_active())
	{
		ET_ASSERT(!(m_EngineButton->get_active()));

		if (!m_ProjectSelected)
		{
			m_ProjectSelected = true;
			changed = true;
		}
	}
	else
	{
		ET_ASSERT(!(m_ProjectButton->get_active()));
		ET_ASSERT(m_EngineButton->get_active());

		if (m_ProjectSelected)
		{
			m_ProjectSelected = false;
			changed = true;
		}
	}

	if (changed && (m_TreeView != nullptr))
	{
		RebuildDirectoryTree();
	}
}

//----------------------------------------------------
// ResourceChooserDialog::OnDirectorySelectionChanged
//
void ResourceChooserDialog::OnDirectorySelectionChanged()
{
	Gtk::TreeModel::iterator const& it = m_TreeSelection->get_selected(Glib::RefPtr<Gtk::TreeModel>::cast_static(m_TreeModel));
	if (it != m_TreeModel->children().end())
	{
		core::Directory* const dir = (*it)[m_Columns.m_Directory];
		if (dir != nullptr)
		{
			m_SelectedDirectory = dir->GetName();
			m_SelectedDirectory = core::FileUtil::GetRelativePath(m_SelectedDirectory, m_BaseDirectory->GetName());
		}
		else
		{
			m_SelectedDirectory.clear();
		}
	}
	else
	{
		m_SelectedDirectory.clear();
	}

	RebuildAssetList();
}

//-----------------------------------------
// ResourceChooserDialog::OnSearchChanged
//
void ResourceChooserDialog::OnSearchChanged()
{
	m_SearchTerm = std::string(m_SearchBar->get_text());

	RebuildAssetList();
}

//-------------------------------------------------
// ResourceChooserDialog::OnAssetTypeFilterChanged
//
void ResourceChooserDialog::OnAssetTypeFilterChanged()
{
	RebuildAssetList();
}

//------------------------------------------------
// ResourceChooserDialog::RebuildDirectoryTree
//
void ResourceChooserDialog::RebuildDirectoryTree()
{
	FileResourceManager* const resourceMan = static_cast<FileResourceManager*>(core::ResourceManager::Instance());
	ET_ASSERT(resourceMan != nullptr);

	m_BaseDirectory = m_ProjectSelected ? resourceMan->GetProjectDirectory() : resourceMan->GetEngineDirectory();

	m_TreeModel->clear();

	Gtk::TreeModel::Row row = *(m_TreeModel->append());
	row[m_Columns.m_Name] = m_ProjectSelected ? "Project Resources" : "Engine Resources";
	row[m_Columns.m_Directory] = m_BaseDirectory;

	auto recursiveAddChildren = [this](Gtk::TreeModel::Row& row, auto& fnRef) mutable -> void
		{
			core::Directory* const baseDir = row[m_Columns.m_Directory];
			ET_ASSERT(baseDir != nullptr);

			for (core::Entry* const entry : baseDir->GetChildren())
			{
				if (entry->GetType() == core::Entry::EntryType::ENTRY_DIRECTORY)
				{
					core::Directory* const childDir = static_cast<core::Directory*>(entry);

					Gtk::TreeModel::Row childRow = *(m_TreeModel->append(row.children()));
					childRow[m_Columns.m_Name] = childDir->GetNameOnly().substr(0, childDir->GetNameOnly().size() - 1).c_str();
					childRow[m_Columns.m_Directory] = childDir;

					fnRef(childRow, fnRef);
				}
			}
		};

	recursiveAddChildren(row, recursiveAddChildren);

	m_TreeSelection->select(row);

	m_TreeView->show_all_children();
	m_TreeView->expand_all();
}

//------------------------------------------------
// ResourceChooserDialog::RebuildDirectoryTree
//
void ResourceChooserDialog::RebuildAssetList()
{
	// fetch assets
	FileResourceManager* const resourceMan = static_cast<FileResourceManager*>(core::ResourceManager::Instance());
	ET_ASSERT(resourceMan != nullptr);

	core::AssetDatabase& database = m_ProjectSelected ? resourceMan->GetProjectDatabase() : resourceMan->GetEngineDatabase();
	std::vector<core::I_Asset*> const filteredAssets = database.GetAssetsMatchingPath(m_SelectedDirectory, 
		m_TypeFilter.AreDirectoriesRecursive(), 
		m_SearchTerm);

	m_FilteredAssets.clear();

	// clear flow box
	for (Gtk::Widget* const child : m_FlowBox->get_children())
	{
		m_FlowBox->remove(*child);
	}

	// repopulate
	for (core::I_Asset* const asset : filteredAssets)
	{
		m_FilteredAssets.emplace_back(asset);
		AssetWidget& assetWidget = m_FilteredAssets.back();
		m_FlowBox->add(*(assetWidget.GetAttachment()));
	}

	m_FlowBox->show_all_children();
}


} // namespace edit
} // namespace et
