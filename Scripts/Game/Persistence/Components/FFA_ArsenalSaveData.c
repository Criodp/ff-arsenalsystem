[EPF_ComponentSaveDataType(FFA_ArsenalShopComponent), BaseContainerProps()]
class FFA_ArsenalSaveDataClass : EPF_ComponentSaveDataClass
{
}

[EDF_DbName.Automatic()]
class FFA_ArsenalSaveData : EPF_ComponentSaveData
{
	ref map<string, int> m_aInventory;

	override EPF_EReadResult ReadFrom(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{		
		FFA_ArsenalShopComponent arsenal = FFA_ArsenalShopComponent.Cast(component);
		
		arsenal.SaveToSaveData(this);
		
		return EPF_EReadResult.OK;
	}
	
	override EPF_EApplyResult ApplyTo(IEntity owner, GenericComponent component, EPF_ComponentSaveDataClass attributes)
	{	
		FFA_ArsenalShopComponent arsenal = FFA_ArsenalShopComponent.Cast(component);
				
		arsenal.LoadFromSaveData(this);
				
		return EPF_EApplyResult.OK;
	}
}