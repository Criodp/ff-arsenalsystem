modded class JWK_PlayerControllerComponent {
	void RequestUnloadVehicleToArsenal(FFA_ArsenalShopComponent arsenal, RplId vehicleId)
	{
		Rpc(RpcAsk_UnloadVehicleToArsenal, JWK_Rpl.GetId(arsenal), vehicleId);
	}
	
	// Server
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void RpcAsk_UnloadVehicleToArsenal(RplId arsenalId, RplId vehicleId)
	{
		FFA_ArsenalShopComponent arsenal = JWK_CompTU<FFA_ArsenalShopComponent>.FindRpl(arsenalId);
		if (!arsenal) return;
		
		RplComponent vehicleRpl = RplComponent.Cast(Replication.FindItem(vehicleId));
		if (!vehicleRpl) return;
		
		IEntity vehicle = vehicleRpl.GetEntity();
		if (!vehicle) return;

		arsenal.PerformVehicleUnload_S(vehicle, GetOwner());
	}
	
	// Helper method to trigger the RPC from the server
	void SendUnloadMessage_S(int itemsCount)
	{
		Rpc(RpcDo_ShowUnloadMessage, itemsCount);
	}

	// RPC that executes only on the client who owns this Player Controller
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	protected void RpcDo_ShowUnloadMessage(int itemsCount)
	{
		string localizedMessage = WidgetManager.Translate("#FFA-ArsenalUI-MovedItems");
		// Format the message text
		string message = string.Format(localizedMessage, itemsCount);
		
		// Show a standard Reforger hint on the screen (Text, Title, Duration in seconds)
		SCR_HintManagerComponent.GetInstance().ShowCustomHint(message, "#FFA-ArsenalUI-Title", 5.0);
	}

}