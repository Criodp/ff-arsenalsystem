class FFA_UnloadToArsenalAction : LTS_BaseLootTransferAction {
	
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		UpdateNearestVehicle();
		if (!m_pNearestVehicle) return;
		
		if (!m_pNearestVehicle.FindComponent(CarControllerComponent) && 
			!m_pNearestVehicle.FindComponent(HelicopterControllerComponent))
		{
			return; 
		}
		
		FFA_ArsenalShopComponent targetArsenal = FFA_ArsenalShopComponent.Cast(GetOwner().FindComponent(FFA_ArsenalShopComponent));
		if (!targetArsenal) 
			targetArsenal = FFA_ArsenalShopComponent.Cast(GetOwner().GetParent().FindComponent(FFA_ArsenalShopComponent));
		
		if (!targetArsenal) return;

		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity);
		if (playerId == 0) 
		{
			Print("FFA TRACE (CLIENT): ERROR - Character has no player ID!", LogLevel.ERROR);
			return;
		}
	
		// Get the network Player Controller entity using the ID
		PlayerController pcEntity = GetGame().GetPlayerManager().GetPlayerController(playerId);
		if (!pcEntity) 
		{
			Print("FFA TRACE (CLIENT): ERROR - Player Controller entity not found!", LogLevel.ERROR);
			return;
		}
	
		// Find the custom component on the Player Controller entity
		JWK_PlayerControllerComponent playerController = JWK_PlayerControllerComponent.Cast(pcEntity.FindComponent(JWK_PlayerControllerComponent));
		if (!playerController) 
		{
			Print("FFA TRACE (CLIENT): ERROR - JWK_PlayerControllerComponent is missing on PC!", LogLevel.ERROR);
			return;
		}

		RplComponent vehicleRpl = RplComponent.Cast(m_pNearestVehicle.FindComponent(RplComponent));
		if (!vehicleRpl) return;

		playerController.RequestUnloadVehicleToArsenal(targetArsenal, vehicleRpl.Id());
	}

	override bool HasLocalEffectOnlyScript() { return true; }
	override bool CanBroadcastScript() { return false; }
}