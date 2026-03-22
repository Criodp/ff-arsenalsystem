class FFA_ArsenalInterfaceNavbarUIComponent : JWK_ShopInterfaceNavbarUIComponent
{	
	override protected void UpdateTradeButtons()
	{
		string label;
		
		if (m_iTradeMode == JWK_EShopInterfaceTradeMode.BUY) {
			label = "#FFA-ArsenalUI-GetItem";
		
		} else if (m_iTradeMode == JWK_EShopInterfaceTradeMode.SELL) {
			label = "#FFA-ArsenalUI-GiveItem";
		}
		
		if (m_iTradeQuantity > 1) {
			label += " " + m_iTradeQuantity;
		}
		
		m_ButtonTrade.SetLabel(label);
		m_ButtonTrade.SetVisible(true);
	}
}