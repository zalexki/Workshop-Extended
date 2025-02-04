class WEXT_AddonBarComponent : ScriptedWidgetComponent
{
	protected const ResourceName ADSDON_BAR_ICON_LAYOUT = "{9B80BD4A534C651C}UI/AddonBarIcon.layout";
	
	static const ResourceName LAYOUT_BASE = "{E51B373A4E72E920}UI/AddonBar.layout";
	static const ResourceName LAYOUT_MAIN_MENU = "{44F366DD5417EC0D}UI/AddonBar_MainMenu.layout";
	
	protected ref WEXT_AddonBarWidgets widgets = new WEXT_AddonBarWidgets();
	
	[Attribute("14", UIWidgets.EditBox)]
	protected int m_iMaxIcons;
	
	override void HandlerAttached(Widget w)
	{
		widgets.Init(w);
		
		SCR_AddonManager mgr = SCR_AddonManager.GetInstance();
		
		if (!mgr)
			return;
		
		SCR_AddonManager.GetInstance().m_OnAddonsEnabledChanged.Insert(Callback_OnAddonsEnabledChanged);
		
		UpdateAllWidgets();
		
		widgets.m_PresetsButtonComponent.m_OnClicked.Insert(Callback_OnPresetsButton);
	}
	
	override void HandlerDeattached(Widget w)
	{
		SCR_AddonManager mgr = SCR_AddonManager.GetInstance();
		
		if (mgr)
			SCR_AddonManager.GetInstance().m_OnAddonsEnabledChanged.Remove(Callback_OnAddonsEnabledChanged);
	}
	
	protected void Callback_OnAddonsEnabledChanged()
	{
		UpdateAllWidgets();
	}
	
	void UpdateAllWidgets()
	{
		SCR_AddonManager mgr = SCR_AddonManager.GetInstance();
		
		if (!mgr)
			return;
		
		array<ref SCR_WorkshopItem> enabledAddons = SCR_AddonManager.SelectItemsBasic(mgr.GetOfflineAddons(), EWorkshopItemQuery.ENABLED);
		
		// Delete prev widgets
		while (widgets.m_AddonIconsLayout.GetChildren())
			widgets.m_AddonIconsLayout.RemoveChild(widgets.m_AddonIconsLayout.GetChildren());
		
		// Add new widgets		
		int nAddonsEnabled = 0;
		for (int i = 0; i < enabledAddons.Count() && i < m_iMaxIcons; i++)
		{
			SCR_WorkshopItem item = enabledAddons[i];
			
			// Ignore this addon
			if (item.GetId() == SCR_AddonManager.WEXT_GUID)
				continue;
			
			Widget w = GetGame().GetWorkspace().CreateWidgets(ADSDON_BAR_ICON_LAYOUT, widgets.m_AddonIconsLayout);
			
			WEXT_AddonBarIconComponent comp = WEXT_AddonBarIconComponent.Cast(w.FindHandler(WEXT_AddonBarIconComponent));
			comp.SetWorkshopItem(item);
			
			nAddonsEnabled++;
		}
		
		// Mod count text
		widgets.m_ModsCountText.SetVisible(nAddonsEnabled != 0);
		if (nAddonsEnabled != 0)
		{
			string addonCountText;
			if (nAddonsEnabled == 1)
				addonCountText = "1 mod enabled";
			else
				addonCountText = string.Format("%1 mods enabled", nAddonsEnabled);
			widgets.m_ModsCountText.SetText(addonCountText);
		}
	}
	
	void Callback_OnPresetsButton()
	{
		GetGame().GetMenuManager().OpenDialog(ChimeraMenuPreset.AddonPresetDialog);
	}
}





//-----------------------------------------------------------------------------------------------------------------------------
// Inject the addons bar to menus.
// This is a hack until overriding of layouts is fixed.
// Normally we should override layout in our addon and 
// embed our layout there through resource browser.

modded class ContentBrowserUI
{
	static const string INJECT_PATH_WORKSHOP_MENU = "MenuBase0.SizeBase.VerticalLayout0.Header.Overlay";
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		Widget tgtWidget = GetRootWidget().FindWidget(INJECT_PATH_WORKSHOP_MENU);
		GetGame().GetWorkspace().CreateWidgets(WEXT_AddonBarComponent.LAYOUT_BASE, tgtWidget);
	}
}

modded class ContentBrowserDetailsMenu
{
	static const string INJECT_PATH_WORKSHOP_DETAILS_MENU = "m_Menu.SizeBase.VerticalLayout0.Header.Overlay";
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		Widget tgtWidget = GetRootWidget().FindWidget(INJECT_PATH_WORKSHOP_DETAILS_MENU);
		GetGame().GetWorkspace().CreateWidgets(WEXT_AddonBarComponent.LAYOUT_BASE, tgtWidget);
	}
	
	override void Init(SCR_WorkshopItem item)
	{
		super.Init(item); // Otherwise it complains that method init is protected?!
	}
}

modded class SCR_ScenarioMenu
{
	const string INJECT_PATH_SCENARIO_MENU = "MenuBase1.SizeBase.VerticalLayout0.Header.Overlay";
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		Widget tgtWidget = GetRootWidget().FindWidget(INJECT_PATH_SCENARIO_MENU);
		GetGame().GetWorkspace().CreateWidgets(WEXT_AddonBarComponent.LAYOUT_BASE, tgtWidget);
	}
}

modded class PlayMenuUI
{
	const string INJECT_PATH_PLAY_MENU = "MenuBase1.SizeBase.VerticalLayout0.Header.Overlay";
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		Widget tgtWidget = GetRootWidget().FindWidget(INJECT_PATH_PLAY_MENU);
		GetGame().GetWorkspace().CreateWidgets(WEXT_AddonBarComponent.LAYOUT_BASE, tgtWidget);
	}
}

modded class MainMenuUI
{
	const string INJECT_PATH_MAIN_MENU = "Overlay0.SizeLayout0.VerticalLayout0.HeaderSize.Header.Frame0";
	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		Widget tgtWidget = GetRootWidget().FindWidget(INJECT_PATH_MAIN_MENU);
		GetGame().GetWorkspace().CreateWidgets(WEXT_AddonBarComponent.LAYOUT_MAIN_MENU, tgtWidget);
	}
}