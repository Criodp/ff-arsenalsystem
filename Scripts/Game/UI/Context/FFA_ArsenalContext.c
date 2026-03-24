[BaseContainerProps()]
class FFA_ArsenalContext : JWK_ShopContext {
	override protected void InitWidgets()
	{
	    m_ShopController = JWK_WidgetHandlerTU<FFA_ArsenalControllerUIComponent>.Find(m_wRoot);
	    
	    if (!m_ShopController) {
	        JWK_Log.Log(this, "FFA_ArsenalControllerUIComponent not found!", LogLevel.ERROR);
	    }

	    m_ShopInterfaceManager = JWK_WidgetHandlerTU<FFA_ArsenalInterfaceManagerUIComponent>.Find(m_wRoot);
	    
	    if (!m_ShopInterfaceManager) {
	        JWK_Log.Log(this, "FFA_ArsenalInterfaceManagerUIComponent not found!", LogLevel.ERROR);
	    }

	    m_wPlayerMoney = JWK_WidgetTU<TextWidget>.FindAny(m_wRoot, "PlayerMoney");

	    static const int ResourcesToUseNum = 2;
	    static const array<int> ResourcesToUse = {
	        JWK_ELogisticsResourceType.SUPPLIES,
	        JWK_ELogisticsResourceType.FUEL
	    };
	    static const array<string> ResourceWidgetIDs = {
	        "Supplies",
	        "Fuel"
	    };
	    
	    m_wResourcesFunds.Clear();
	    m_wResourceFundsValues.Clear();
	    for (int i = 0; i < ResourcesToUseNum; i++) {
	        int type = ResourcesToUse.Get(i);
	        string str = ResourceWidgetIDs.Get(i);
	        
	        m_wResourcesFunds.Insert(type, m_wRoot.FindAnyWidget("ResourceFunds" + str));
	        m_wResourceFundsValues.Insert(
	            type, JWK_WidgetTU<TextWidget>.FindAny(m_wRoot, "ResourceFunds" + str + "Value")
	        );
	    }
	}
	
	override protected void OnShow()
	{
		RegisterInputs(GetGame().GetInputManager());
		
		JWK.GetUI().NotifyContextOpened(this);
		
		InitWidgets();
		InitShop();
		InitHooks();
		UpdatePlayerFunds();
	}

	override protected void InitHooks()
	{
	}

	override protected void UpdatePlayerFunds()
	{
		if (m_wPlayerMoney) {
			m_wPlayerMoney.SetVisible(false);
		}

		foreach (int type, Widget w : m_wResourcesFunds) {
			if (w) w.SetVisible(false);
		}
	}
}