class ActionExitRaid_WithItem: ActionInteractBase
{
	void ActionExitRaid_WithItem()
	{
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_Text = "#action_raidexit_withitem";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		return true;
	}

	override void OnExecuteClient( ActionData action_data )
	{
	}

	override void OnExecuteServer(ActionData action_data)
	{
		PlayerBase player = action_data.m_Player;
		if (!player)
			return;
		RaidExit_WithItem exit = RaidExit_WithItem.Cast(action_data.m_Target.GetObject());
		if (!exit)
			return;

		EntityAI itemInHands = player.GetItemInHands();
		if (itemInHands && itemInHands.GetType()==exit.itemneed)
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_teleportation", "#raidexit_tp10sec");
			GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TeleportPlayerAfterDelay, 10000, false, player, exit);
		}	
		else
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_unavailable", "#action_raidexit_withitem2");
			return;
		}

	}

	void TeleportPlayerAfterDelay(PlayerBase player, RaidExit_WithItem exit)
	{
		if (!player || !player.IsAlive())
		{
			return;
		}
		EntityAI itemInHands = player.GetItemInHands();
		if (itemInHands && itemInHands.GetType()==exit.itemneed)
		{
		itemInHands.DeleteSafe();
		vector destination = GetRaidExitConfigManager().GetRandomVectorPosition(exit.teleportPositions);
		player.SetPosition(destination);

		NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_tpdone", "#raidexit_successexit");

		Print("[RaidExit_WithItem] Player " + player.GetIdentity().GetName() + " teleported to " + destination);
		}
		else
		{
			return;
		}
	}

}
