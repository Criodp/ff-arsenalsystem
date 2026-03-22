class FFA_ArsenalItemDetailsUIComponent : JWK_ShopItemDetailsUIComponent {

	override void SyncBuying(JWK_ShopContextItemInfo itemInfo, JWK_ShopItemDetailsWidgets widgets)
	{
        widgets.m_wPriceRow.SetVisible(false);

        bool useStock = (itemInfo.m_iShopStock != -1);
        widgets.m_wStockRow.SetVisible(useStock);
        
        if (useStock) {
            widgets.m_wStockLabel.SetText("#FFA-ArsenalUI-ArsenalStock");
            
            string shopStock = itemInfo.m_iShopStock.ToString();
            if (itemInfo.m_iShopStockMax > 0)
                shopStock += "/" + itemInfo.m_iShopStockMax;
            
            widgets.m_wStockValue.SetText(shopStock);
        }
	}

	override void SyncSelling(JWK_ShopContextItemInfo itemInfo, JWK_ShopItemDetailsWidgets widgets)
	{
		widgets.m_wPriceRow.SetVisible(false);
        widgets.m_wStockRow.SetVisible(true);

        widgets.m_wStockLabel.SetText("#JWK-ShopUI-PlayerStock");
        widgets.m_wStockValue.SetText(itemInfo.m_iPlayerStock.ToString());
	}
	
}