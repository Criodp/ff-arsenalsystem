[ComponentEditorProps(category: "JWK/FreedomFighters", description: "")]
class FFA_ArsenalShopComponentClass: JWK_ShopComponentClass
{
}

// probably unnecessary, but we use a custom payment model to avoid side effects with other shops
modded enum JWK_EShopPaymentModel
{
	SPOILS
}

modded enum JWK_EShopCurrency
{

	SPOILS
}

typedef map<string, int> tempSaveInventory;
typedef map<ResourceName, int> tempInventory;

class FFA_ArsenalShopComponent: JWK_ShopComponent
{
	protected IEntity m_LastProcessedEntity;
	override void ReplenishStock_S() {}
	
	void SaveToSaveData(FFA_ArsenalSaveData saveData)
	{
		tempSaveInventory tempSave = new tempSaveInventory();
		foreach(string key, int value : m_aInventory) {
			tempSave.Set(key, value);
		}
		saveData.m_aInventory = tempSave;
	}
	
	void LoadFromSaveData(FFA_ArsenalSaveData saveData)
	{
		tempInventory tempInv = new tempInventory();
		SCR_EntityCatalogManagerComponent catalogManager = SCR_EntityCatalogManagerComponent.GetInstance();
		if (!catalogManager) return;
		array<SCR_ArsenalItem> filteredArsenalItems = {};
		
		catalogManager.GetAllArsenalItems(filteredArsenalItems);
		foreach(ResourceName key, int value : saveData.m_aInventory) {
			if (key.Length() == 16) 
			{
				foreach(SCR_ArsenalItem item : filteredArsenalItems) {
						ResourceName itemName;
						itemName = item.GetItemResourceName();
						if (itemName.Contains(key)) {
							key = itemName
						}
				}
			}
			tempInv.Set(key, value);
		}
		m_aInventory = tempInv;
		if (!m_aInventory) return;
	}
	
	override map<ResourceName, int> GetInventory()
	{
		if (!m_aInventory)
			m_aInventory = new map<ResourceName, int>;
		return m_aInventory;
	}

	override void SetInventoryStock_S(ResourceName item, int stock)
	{
		if (m_iPaymentModel == JWK_EShopPaymentModel.SPOILS) {
			if (m_aInventory.Contains(item) && m_aInventory[item] == stock) return;
			
			m_aInventory.Set(item, stock);
			FFA_ArsenalSaveData save = new FFA_ArsenalSaveData();
			save.m_aInventory = new map<string, int>();
			foreach(string key, int value : m_aInventory) {
				save.m_aInventory.Set(key, value);
			}
			SaveToSaveData(save);
			JWK_Log.Log(this, string.Format(
				"Change arsenal stock (%1, %2)",
				item,
				stock
			));
			if (m_OnStockChanged)
				m_OnStockChanged.Invoke(this, item, stock);
			
			// Might be called on entity init before rpl registration.
			if (m_Rpl.Id() != RplId.Invalid())
				Rpc(RpcDo_SetStock, item, m_aInventory[item]);
		} else {
			super.SetInventoryStock_S(item, stock)
		}
	}	
	
	
	override protected void ChargePurchase_S(int playerID, ResourceName item)
	{
		if (m_iPaymentModel == JWK_EShopPaymentModel.SPOILS) {
			// we do not charge anything, this is just a storage 		
		} else {
			super.ChargePurchase_S(playerID,item);
		}
	}
	
	override void PerformPlayerItemSale_S(int playerID, ResourceName item)
	{
	    IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerID);
	    if (!player) return;
	    
	    SCR_InventoryStorageManagerComponent inventory = JWK_CompTU<SCR_InventoryStorageManagerComponent>.FindIn(player);
	    if (!inventory) return;
	    
	    array<IEntity> inventoryItems = {};
	    inventory.GetItems(inventoryItems);
	    
	    IEntity targetEntity = null;
	    
	    foreach (IEntity ent : inventoryItems) {
	        if (ent.GetPrefabData().GetPrefabName() == item) {
	            targetEntity = ent;
	            break; 
	        }
	    }
	    
	    if (!targetEntity || targetEntity == m_LastProcessedEntity) return; 
	    m_LastProcessedEntity = targetEntity;

	    UnloadEntityToArsenal_S(targetEntity, inventory);
	    SCR_EntityHelper.DeleteEntityAndChildren(targetEntity);
		
	    EPF_PersistenceComponent arsenalPersistence = EPF_PersistenceComponent.Cast(GetOwner().FindComponent(EPF_PersistenceComponent));
	    if (arsenalPersistence) {
	        arsenalPersistence.Save();
	    }
	    
	    EPF_PersistenceComponent playerPersistence = EPF_PersistenceComponent.Cast(player.FindComponent(EPF_PersistenceComponent));
	    if (playerPersistence) {
	        playerPersistence.Save();
	    }
	}
	
	override void PerformPlayerItemPurchase_S(int playerID, ResourceName item)
	{
	    IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerID);
	    if (!player) return;

	    int currentStock = GetStock(item);
	    if (currentStock <= 0) {
	        return; 
	    }
	
	    SCR_InventoryStorageManagerComponent inventory = JWK_CompTU<SCR_InventoryStorageManagerComponent>.FindIn(player);
	    if (!inventory) return;
	
		array<IEntity> itemsBefore = {};
    	inventory.GetItems(itemsBefore);
		
	    if (!inventory.TrySpawnPrefabToStorage(item)) 
	    {
	        return; 
	    }
		
	    array<IEntity> itemsAfter = {};
	    inventory.GetItems(itemsAfter);
	    
	    IEntity spawnedItem = null;
	    foreach (IEntity ent : itemsAfter)
	    {
	        if (ent.GetPrefabData().GetPrefabName() == item && !itemsBefore.Contains(ent))
	        {
	            spawnedItem = ent;
	            break;
	        }
	    }
	
	    if (spawnedItem)
	    {
	        array<IEntity> defaultChildren = {};
	        BaseInventoryStorageComponent storage = BaseInventoryStorageComponent.Cast(spawnedItem.FindComponent(BaseInventoryStorageComponent));
	        
	        if (storage)
	        {
	            storage.GetAll(defaultChildren);
	            foreach (IEntity child : defaultChildren)
	            {
	                inventory.TryDeleteItem(child);
	            }
	        }
	    }
	
	    currentStock = currentStock - 1;
	    
	    if (currentStock > 0) {
	        m_aInventory.Set(item, currentStock);
	    } else {
	        m_aInventory.Remove(item);
	    }
		
		//try fix to zero count item
		if (m_Rpl.Id() != RplId.Invalid())
	    {
	        Rpc(RpcDo_SetStock, item, currentStock);
	    }
	    
	    SetInventory_S(m_aInventory);
	
	    EPF_PersistenceComponent arsenalPersistence = EPF_PersistenceComponent.Cast(GetOwner().FindComponent(EPF_PersistenceComponent));
	    if (arsenalPersistence) arsenalPersistence.Save();
	    
	    EPF_PersistenceComponent playerPersistence = EPF_PersistenceComponent.Cast(player.FindComponent(EPF_PersistenceComponent));
	    if (playerPersistence) playerPersistence.Save();
	}
	
	void PerformArsenalMove(ResourceName item, EntityID vehicleId) {
		Vehicle vehicle = Vehicle.Cast(GetGame().GetWorld().FindEntityByID(vehicleId));
		ScriptedInventoryStorageManagerComponent sourceStorage = ScriptedInventoryStorageManagerComponent.Cast(vehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));
		array<IEntity> inventoryItems = {};
		sourceStorage.GetItems(inventoryItems);
		foreach (IEntity ent : inventoryItems) {
			if (ent.GetPrefabData().GetPrefabName() != item) continue;
			if (!sourceStorage.TryDeleteItem(ent)) continue;
					
			AddInventoryStock_S(item, 1);
			return;
		}
	}
	
	void UnloadEntityToArsenal_S(IEntity targetEntity, ScriptedInventoryStorageManagerComponent sourceInventory)
	{
	    if (!targetEntity || !sourceInventory) return;
	
	    ResourceName itemPrefab = targetEntity.GetPrefabData().GetPrefabName();
	    
	    array<IEntity> allChildren = {};
	    BaseInventoryStorageComponent storage = BaseInventoryStorageComponent.Cast(targetEntity.FindComponent(BaseInventoryStorageComponent));
	    if (storage)
	    {
	        storage.GetAll(allChildren);
	    }
	
	    foreach (IEntity child : allChildren)
	    {
	        if (!child) continue;
	        
	        // Ensure the child is a valid inventory item
	        InventoryItemComponent invItem = InventoryItemComponent.Cast(child.FindComponent(InventoryItemComponent));
	        if (!invItem) continue;
	        
	        // Check if the item has UI attributes
	        ItemAttributeCollection attributes = invItem.GetAttributes();
	        if (!attributes) continue;
	        
	        // Filter out hidden engine proxies or dummy items that lack a name
	        UIInfo uiInfo = attributes.GetUIInfo();
	        if (!uiInfo || uiInfo.GetName() == string.Empty) continue;
	        
	        ResourceName childPrefab = child.GetPrefabData().GetPrefabName();
	        AddInventoryStock_S(childPrefab, 1);
	    }
	    
		AddInventoryStock_S(itemPrefab, 1);
	}
	
	void PerformVehicleUnload_S(IEntity vehicle, IEntity playerControllerEntity)
	{
		if (vehicle == GetOwner() || (GetOwner().GetParent() && vehicle == GetOwner().GetParent()))
		{
			return;
		}
	
		ScriptedInventoryStorageManagerComponent vehicleInventory = ScriptedInventoryStorageManagerComponent.Cast(vehicle.FindComponent(ScriptedInventoryStorageManagerComponent));
		if (!vehicleInventory) return;
	
		array<IEntity> itemsInVehicle = {};
		vehicleInventory.GetItems(itemsInVehicle);
		
		int count = 0;
		foreach (IEntity item : itemsInVehicle)
		{
			UnloadEntityToArsenal_S(item, vehicleInventory);

			SCR_EntityHelper.DeleteEntityAndChildren(item);
			count++;
		}
	
		if (count > 0)
		{
			EPF_PersistenceComponent arsenalPersistence = EPF_PersistenceComponent.Cast(GetOwner().FindComponent(EPF_PersistenceComponent));
			if (arsenalPersistence) arsenalPersistence.Save();
			
			JWK_PlayerControllerComponent pc = JWK_PlayerControllerComponent.Cast(playerControllerEntity.FindComponent(JWK_PlayerControllerComponent));
			if (pc)
			{
				pc.SendUnloadMessage_S(count);
			}
		}
		else
		{
			JWK_PlayerControllerComponent pcEmpty = JWK_PlayerControllerComponent.Cast(playerControllerEntity.FindComponent(JWK_PlayerControllerComponent));
			if (pcEmpty)
			{
				pcEmpty.SendUnloadMessage_S(0); 
			}
		}
	}
}