class FFA_ArsenalAction : JWK_ShopAction 
{
 	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
 	{
		if (!CanBePerformedScript(pUserEntity)) return;
		
		FFA_ArsenalShopComponent arsenal = FFA_ArsenalShopComponent.Cast(GetShop());
		if (!arsenal) return;
		
		JWK_EFeedback reason;
		if (!arsenal.CanBeUsedByPlayer(SCR_PlayerController.GetLocalControlledEntity(), reason)) {
			JWK.GetHint().ShowFeedback(reason);
			return;
		}
		
		PerformActionInternalMy(arsenal, pOwnerEntity, pUserEntity);
 	}
	
	protected void PerformActionInternalMy(FFA_ArsenalShopComponent arsenal, IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		FFA_ArsenalContext context = FFA_ArsenalContext.Cast(JWK.GetUI().GetContext(FFA_ArsenalContext));
		
		if (!context) {
			return;
		}

		context.SetShop(arsenal);
		JWK.GetUI().OpenContext(FFA_ArsenalContext);
	}
	
	override bool CanBePerformedScript(IEntity user)
 	{
		FFA_ArsenalShopComponent arsenal =  FFA_ArsenalShopComponent.Cast(GetShop());
		if (!arsenal) {
			return false;
		}
		
		Managed genericInventoryManager = user.FindComponent(SCR_InventoryStorageManagerComponent);
		if (!genericInventoryManager) return false;
		
		return true;
 	}
}