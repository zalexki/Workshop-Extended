class WEXT_ListAddonsMenu : SCR_SubMenuBase
{
	protected ref WEXT_ListAddonsMenuWidgets widgets = new WEXT_ListAddonsMenuWidgets();
	
	//! Refreshes all lists
	protected void RefreshAll()
	{
		WorkshopApi wkAPI = GetGame().GetBackendApi().GetWorkshop();
		wkAPI.SetPageItems(10);
		array<WorkshopItem> items = new array<WorkshopItem>;
		wkAPI.GetPageItems(items);
		
		// Register items in Addon Manager
		array<ref SCR_WorkshopItem> itemsRegistered = new array<ref SCR_WorkshopItem>;
		foreach (auto i : items)
		{
			SCR_WorkshopItem itemRegistered = SCR_AddonManager.GetInstance().Register(i);
			itemsRegistered.Insert(itemRegistered);
		}
		
		// Convert to basic array for sorting...
		array<SCR_WorkshopItem> itemsWeakPtrs = {};
		foreach (auto i : itemsRegistered) {
			itemsWeakPtrs.Insert(i);
		}
		
		// Sort by name...
		SCR_Sorting<SCR_WorkshopItem, WEXT_CompareWorkshopItemName>.HeapSort(itemsWeakPtrs);
		
		// Convert back to array<ref ...>
		array<ref SCR_WorkshopItem> itemsSorted = {};
		foreach (auto i : itemsWeakPtrs) {
			itemsSorted.Insert(i);
		}
			
		//filter by query (enabled)
		//auto enabledItems = SCR_AddonManager.SelectItemsBasic(itemsSorted, EWorkshopItemQuery.ENABLED);
		
		CreateListLines(widgets.m_AddonsList, widgets.m_AddonsListScroll, itemsSorted);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void CreateListLines(VerticalLayoutWidget vLayout, ScrollLayoutWidget scroll, array<ref SCR_WorkshopItem> items)
	{
		// Record scroll pos
		float scrollx, scrolly;
		scroll.GetSliderPos(scrollx, scrolly);
		
		// Delete previous lines
		while (vLayout.GetChildren())
		{
			vLayout.RemoveChild(vLayout.GetChildren());
		}
		
		foreach (SCR_WorkshopItem item : items)
		{
			// Don't add ourselves to the mod list, it makes no sense
			if (item.GetId() == SCR_AddonManager.WEXT_GUID)
				continue;
			
			Widget w = GetGame().GetWorkspace().CreateWidgets("{E363BEA40C56718E}UI/WEXT_ListAddonLine.layout", vLayout);
			WEXT_ListAddonLineComponent comp = WEXT_ListAddonLineComponent.Cast(w.FindHandler(WEXT_ListAddonLineComponent));
			comp.Init(item);
			comp.m_OnEnableButton.Insert(OnEnableButton);
			comp.m_OnDisableButton.Insert(OnDisableButton);
			comp.m_OnMouseEnter.Insert(OnLineMouseEnter);
			comp.m_OnMouseLeave.Insert(OnLineMouseLeave);
		}
		
		// Restore scroll pos
		scroll.SetSliderPos(scrollx, scrolly);
	}
	
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen(SCR_SuperMenuBase parentMenu)
	{
		super.OnMenuOpen(parentMenu);
	
		widgets.Init(GetRootWidget());
		
		
		// Subscribe to addon manager events
		SCR_AddonManager.GetInstance().m_OnAddonOfflineStateChanged.Insert(Callback_OnAddonOfflineStateChanged);
		
		RefreshAll();
		
		SCR_AddonManager.GetInstance().m_OnAddonsEnabledChanged.Insert(Callback_OnAddonListStateChanged);
	}
	
	
	//------------------------------------------------------------------------------------------------
	override void OnMenuClose(SCR_SuperMenuBase parentMenu)
	{
		super.OnMenuClose(parentMenu);
		
		// Unsubscribe from addon manager events
		SCR_AddonManager.GetInstance().m_OnAddonOfflineStateChanged.Remove(Callback_OnAddonOfflineStateChanged);
		SCR_AddonManager.GetInstance().m_OnAddonsEnabledChanged.Remove(Callback_OnAddonListStateChanged);
	}
	
	
	
	//------------------------------------------------------------------------------------------------
	protected void OnLineMouseEnter(WEXT_ListAddonLineComponent comp)
	{
		auto item = comp.GetWorkshopItem();
		widgets.m_AddonInfoPanelComponent.SetWorkshopItem(item);
	}
	
	
	//------------------------------------------------------------------------------------------------
	protected void OnLineMouseLeave(WEXT_ListAddonLineComponent comp)
	{
		// Since this event is handled not natively, but it scripted, it is buggy, so 
		// OnMouseLeave can be called before OnMouseEnter. So we don't erase data from info
		// panel.
		//widgets.m_AddonInfoPanelComponent.SetWorkshopItem(null);
	}	
	
	
	
	
	
	
	//------------------------------------------------------------------------------------------------
	// B U T T O N S
	
	
	//------------------------------------------------------------------------------------------------
	protected void OnEnableButton(WEXT_ListAddonLineComponent comp)
	{
		SCR_WorkshopItem item = comp.GetWorkshopItem();
		item.SetEnabled(true);
	}
	
	
	//------------------------------------------------------------------------------------------------
	protected void OnDisableButton(WEXT_ListAddonLineComponent comp)
	{
		SCR_WorkshopItem item = comp.GetWorkshopItem();
		item.SetEnabled(false);
	}	
	
	protected void OnToolsButton()
	{
		new WEXT_ToolsDialog();
	}
	
	
	//------------------------------------------------------------------------------------------------
	// O T H E R
	
	
	
	//------------------------------------------------------------------------------------------------
	//! Called by SCR_AddonManager when some addon is downloaded or uninstalled
	protected void Callback_OnAddonOfflineStateChanged(SCR_WorkshopItem item, bool newState)
	{
		// Some addon was installed or uninstalled, we must refresh the page
		RefreshAll();
	}
	
	protected void Callback_OnAddonListStateChanged()
	{
		// Something got enabled or disabled, refresh the page
		RefreshAll();
	}
}