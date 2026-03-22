class FFA_BoxTovehicleLootTransferAction : LTS_BaseLootTransferAction {

	//------------------------------------------------------------------------------------------------
	override protected ScriptedInventoryStorageManagerComponent GetTargetStorageManager()
	{
		return ScriptedInventoryStorageManagerComponent.Cast(m_pNearestVehicle.FindComponent(SCR_VehicleInventoryStorageManagerComponent));

	}
	
	//------------------------------------------------------------------------------------------------
	override protected BaseInventoryStorageComponent GetTargetStorage()
	{
		return BaseInventoryStorageComponent.Cast(m_pNearestVehicle.FindComponent(BaseInventoryStorageComponent));
	
	}

	//------------------------------------------------------------------------------------------------
	override protected ScriptedInventoryStorageManagerComponent GetSourceStorageManager()
	{
		return ScriptedInventoryStorageManagerComponent.Cast(GetOwner().FindComponent(SCR_InventoryStorageManagerComponent));
	}

}