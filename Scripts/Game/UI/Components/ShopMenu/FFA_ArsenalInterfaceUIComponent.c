class FFA_ArsenalInterfaceUIComponent : JWK_ShopInterfaceUIComponent
{
	override protected JWK_ShopContextItemInfo CreateItemInfo(ResourceName resource, bool forSelection)
	{
		JWK_ShopContextItemInfo result = super.CreateItemInfo(resource, forSelection);
		if (!result) return null;
		
		result.m_iMoneyBuyPrice = 0;
		result.m_iMoneySellPrice = 0;
		result.m_mResourceCost.Clear();
		
		return result;
	}

	override protected void UpdateTradeButton()
		{
			if (!m_SelectedItemInfo) {
	        m_Navbar.SetTradeEnabled(false);
	        return;
	    }
	    
	    const string inventoryTab = GetCurrentInventoryTab();
	    
	    if (inventoryTab == SHOP_INVENTORY_TAB_ID) {
	        m_Navbar.SetTradeMode(JWK_EShopInterfaceTradeMode.BUY);
	        
	        m_Navbar.SetTradeQuantity(JWK.GetShopManager().GetItemAttributes(m_rSelectedItem).m_iBundleSize);
	        
	        bool buyEnabled = (m_SelectedItemInfo.m_iShopStock > 0);
	        m_Navbar.SetTradeEnabled(buyEnabled);
	        
	    } else {
	        m_Navbar.SetTradeMode(JWK_EShopInterfaceTradeMode.SELL);
	        m_Navbar.SetTradeQuantity(1);
	        
	        m_Navbar.SetTradeEnabled(true);
	    }
	}

	override protected void DoBuy()
	{
		if (!m_Controller.Buy(m_rSelectedItem)) return;
		
		if (JWK_VehicleUtils.IsVehiclePrefab(m_rSelectedItem)) {
			m_Navbar.GetOnActionRequested().Invoke(JWK_EShopInterfaceNavbarAction.CLOSE);
			return;
		}
		
		ReloadGrid();
		SelectItem(m_rSelectedItem);
	}
	
	override protected void DoSell()
	{
		if (!m_Controller.Sell(m_rSelectedItem)) return;
		
		ReloadGrid();
		SelectItem(m_rSelectedItem);
	}

	override protected void ReloadGrid()
	{		
	    string activeTab = GetCurrentInventoryTab();
	
	    if (activeTab == SHOP_INVENTORY_TAB_ID) {
	        JWK_BaseShopComponent baseShop = m_Controller.GetShop();
	        if (!baseShop) {
	            return;
	        }
	
	        map<ResourceName, int> storageList = baseShop.GetInventory();
	        
	        ReinitItemsModel(storageList);
	        
	    } else if (activeTab == PLAYER_INVENTORY_TAB_ID) {
	        map<ResourceName, int> playerInventory = new map<ResourceName, int>();
	        CollectPlayerInventory(playerInventory);
	        ReinitItemsModel(playerInventory);
	    }
	}
	
	override protected void CollectPlayerInventory(map<ResourceName, int> result)
	{
	    const IEntity player = SCR_PlayerController.GetLocalControlledEntity();
		const map<ResourceName, int> shopInventory = m_Controller.GetShop().GetInventory();
		
		SCR_InventoryStorageManagerComponent inventory = JWK_CompTU<SCR_InventoryStorageManagerComponent>
			.FindIn(player);
		if (!inventory) return;
		
		array<IEntity> items = {};
		inventory.GetItems(items);
		
		foreach (IEntity item : items) {
			InventoryItemComponent invItem = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
			if (!invItem) continue;
			
			// Ensure the item is actually assigned to a valid inventory slot
			if (!invItem.GetParentSlot()) continue;
			
			// Filter out hidden engine proxies or dummy items inside vests/backpacks that lack a name
			ItemAttributeCollection attributes = invItem.GetAttributes();
			if (!attributes) continue;
			
			UIInfo uiInfo = attributes.GetUIInfo();
			if (!uiInfo || uiInfo.GetName() == string.Empty) continue;
			
			EntityPrefabData prefabData = item.GetPrefabData();
			if (!prefabData) continue;
			
			ResourceName resName = prefabData.GetPrefabName();
			
			result.Set(resName, result.Get(resName) + 1); 
		}
	}
	
	override protected string GetCurrentInventoryTab()
	{
		if (!m_InventoryTabs) return string.Empty;
		
		return m_InventoryTabs.GetContent(m_InventoryTabs.GetShownTab()).m_sTabIdentifier;
	}
	
	override void HandlerAttached(Widget w)
	{
	    super.HandlerAttached(w);
	    
	    if (m_InventoryTabs)
	    {
	        m_InventoryTabs.GetOnTabChange().Remove(OnInventoryTabChanged); 
	        
	        m_InventoryTabs.GetOnTabChange().Insert(FFA_OnTabChanged);
	    }
	
	}
	
	protected void FFA_OnTabChanged(SCR_TabViewComponent tabView, Widget widget)
	{
	    SelectItem(ResourceName.Empty); 
	    
	    int selectedTab = tabView.GetShownTab();
	    
	    m_CurrentItemsModel.Clear();
	    
	    if (selectedTab == 0)
	    {
	        LoadStorageInventory();
	    }
	    else if (selectedTab == 1)
	    {
	        LoadPlayerInventory();
	    }
	    
	    ReinitItemsView();
	}
	
	protected void LoadStorageInventory()
	{
		JWK_BaseShopComponent baseShop = m_Controller.GetShop();
	    if (!baseShop) return;
	
	    map<ResourceName, int> storageList = baseShop.GetInventory();
		
	    JWK_ItemManagerComponent itemMgr = JWK.GetItemManager();
	
	    for (int i = 0; i < storageList.Count(); i++)
	    {
	        ResourceName prefab = storageList.GetKey(i);
	
	        if (itemMgr.IsItemDisabled(prefab)) continue;
	        
	        JWK_ShopContextItem shopContextItem = JWK_ShopContextItem.CreateFromPrefab(prefab);
	        if (!shopContextItem) continue;
	        
	        m_CurrentItemsModel.Insert(shopContextItem);
	    }
	}
	
	protected void LoadPlayerInventory()
	{
	    IEntity player = SCR_PlayerController.GetLocalControlledEntity();
	    if (!player) return;
	    
	    SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
	    if (!inventory) return;
	    
	    array<IEntity> items = {};
	    inventory.GetItems(items);
	    
	    map<ResourceName, int> playerItemsMap = new map<ResourceName, int>();
	    
	    foreach (IEntity item : items) 
	    {
	        EntityPrefabData prefabData = item.GetPrefabData();
	        if (!prefabData) continue;
	        
	        ResourceName resName = prefabData.GetPrefabName();
			if (resName == ResourceName.Empty) continue;
			
	        InventoryItemComponent invItem = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
	        if (invItem)
	        {
	            ItemAttributeCollection attr = invItem.GetAttributes();
	            if (attr)
	            {
	                UIInfo info = attr.GetUIInfo();
	                if (!info || info.GetName() == "") continue;
	            }
	        }
	        else 
	        {
	            continue;
	        }

	        if (playerItemsMap.Contains(resName))
	            playerItemsMap.Set(resName, playerItemsMap.Get(resName) + 1); 
	        else
	            playerItemsMap.Insert(resName, 1);
	    }
	
	    JWK_ItemManagerComponent itemMgr = JWK.GetItemManager();
	
	    for (int i = 0; i < playerItemsMap.Count(); i++)
	    {
	        ResourceName res = playerItemsMap.GetKey(i);
	        if (itemMgr.IsItemDisabled(res)) continue;
	        
	        JWK_ShopContextItem shopContextItem = JWK_ShopContextItem.CreateFromPrefab(res);
	        if (!shopContextItem) continue;
	        
	        m_CurrentItemsModel.Insert(shopContextItem);
	    }
	}
	
	override protected void SetupCard(JWK_GridCellCardComponent card, ResourceName item, bool useSellPrice)
	{
		card.ResetContents();
		card.SetUserData(null);
		
		if (item == ResourceName.Empty) return;
		
		JWK_ShopContextItemInfo info = CreateItemInfo(item, false);
		if (!info) return;
		UIInfo uiInfo = JWK_UIUtils.GetItemUIInfo(item);
		if (!uiInfo) return;
		card.SetTitle(uiInfo.GetName());
		card.SetPreviewPrefab(item);
	
		if (useSellPrice) {
			card.SetMoneyPrice(info.m_iMoneySellPrice);
		} else {
			card.SetMoneyPrice(info.m_iMoneyBuyPrice);
		}
		
		card.SetResourcePrice(
			JWK_ELogisticsResourceType.SUPPLIES,
			info.GetResourceCost(JWK_ELogisticsResourceType.SUPPLIES)
		);
		card.SetResourcePrice(
			JWK_ELogisticsResourceType.FUEL,
			info.GetResourceCost(JWK_ELogisticsResourceType.FUEL)
		);
		card.SetHighlighted(m_rSelectedItem == item);
		card.SetUnavailable(info.m_iShopStock == 0);
		card.SetUserData(info);
	}
	
	override protected void OnShopStockChanged(JWK_BaseShopComponent shop, ResourceName resource, int stock)
	{
		super.OnShopStockChanged(shop, resource, stock);

		bool shouldReloadGrid = (stock <= 0 || GetCurrentInventoryTab() == PLAYER_INVENTORY_TAB_ID);	

		if (shouldReloadGrid)
		{
			ReloadGrid();
		}
		
		GetGame().GetCallqueue().CallLater(DelayedInfoUpdate, 200, false, resource);
	}
	
	protected void DelayedInfoUpdate(ResourceName res)
	{
		if (m_rSelectedItem == res)
		{
			SelectItem(res);
		}
	}
}