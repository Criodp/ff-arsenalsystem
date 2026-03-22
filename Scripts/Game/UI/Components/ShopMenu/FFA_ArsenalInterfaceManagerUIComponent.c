class FFA_ArsenalInterfaceManagerUIComponent : JWK_ShopInterfaceManagerUIComponent
{
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);
		
	}
	
	override protected void OnNavbarAction(JWK_EShopInterfaceNavbarAction action)
	{
		if (action == JWK_EShopInterfaceNavbarAction.CLOSE) {
			JWK_UIContextTU<FFA_ArsenalContext>.Get().HideLayout();
		
		} else if (action == JWK_EShopInterfaceNavbarAction.LOADOUT_VIEW) {
			const JWK_ShopContext shopContext = JWK_UIContextTU<FFA_ArsenalContext>.Get();
			shopContext.OpenLoadoutManager();
		}
	}
	
	override protected void OnShopChanged(JWK_BaseShopComponent shop)
	{
	    if (m_Controller)
	        m_Controller.GetOnShopChanged().Remove(OnShopChanged);

	    if (!m_Navbar && m_wRoot)
	    {
	        m_Navbar = JWK_ShopInterfaceNavbarUIComponent.Cast(m_wRoot.FindHandler(JWK_ShopInterfaceNavbarUIComponent));
	    }

	    if (!m_Navbar)
	    {
	        return;
	    }

	    const JWK_ShopContext shopContext = JWK_UIContextTU<JWK_ShopContext>.Get();
	    if (shopContext && shop)
	    {
	        bool isLoadoutEnabled = shopContext.IsLoadoutManagerAvailable() && shop.IsLoadoutViewEnabled();
	        m_Navbar.SetLoadoutViewEnabled(isLoadoutEnabled);
	    }
	}
}