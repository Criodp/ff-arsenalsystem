class FFA_ArsenalControllerUIComponent : JWK_ShopControllerUIComponent
{
	protected int m_iArsenalStock;

	override bool Buy(ResourceName item)
	{
		return super.Buy(item);
	}
	
	override bool Sell(ResourceName item)
	{
		if (item == ResourceName.Empty) return false;
		if (!m_Shop.IsSellingEnabled()) return false;
		
		IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		if (!player) {
			JWK_Log.Log(this, "DoSell: no controlled entity!", LogLevel.WARNING);
			return false;
		}
		
		JWK.GetLocalPlayerController().ShopSell(
			m_Shop, item
		);
		return true;
	}
}