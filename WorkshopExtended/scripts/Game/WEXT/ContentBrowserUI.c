modded class ContentBrowserUI
{
	//------------------------------------------------------------------------------------------------
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		
		// A bit of a hacky way to add a new tab to layout:
		// This should be possible to do through modding layout file, but atm it doesn't work for some reason,
		// Should figure out later what's happening here.
		m_TabViewComponent.AddTab(
			"{5242DD0FBCE21DF5}UI/AddonListTab/ListAddonsMenu.layout",
			"All (List)",
			enabled: true);
		m_TabViewComponent.AddTab(
			"{0FFE02F22D314775}UI/AddonListTab/ListAddonsSubMenu.layout",
			"Downloaded (List)",
			enabled: true);
	}
}