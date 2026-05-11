class ActionExitRaid_NoBP: ActionInteractBase
{
	void ActionExitRaid_NoBP()
	{
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_Text = "#action_raidexit_nobackpack";
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

		EntityAI backpack = player.FindAttachmentBySlotName("Back");
		if (backpack)
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_unavailable", "#raidexit_nobackpack1");
			return;
		}

		EntityAI itemInHands = player.GetItemInHands();
		if (itemInHands && itemInHands.GetInventory().HasInventorySlot(InventorySlots.BACK))
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_unavailable", "#raidexit_nobackpack2");
			return;
		}

		NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_teleportation", "#raidexit_tp10sec");

		GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TeleportPlayerAfterDelay, 10000, false, player);
	}

	void TeleportPlayerAfterDelay(PlayerBase player)
	{
		if (!player || !player.IsAlive())
		{
			return;
		}

		// Финальная проверка рюкзака перед телепортацией
		EntityAI backpack = player.FindAttachmentBySlotName("Back");
		if (backpack)
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_tpcancel", "#raidexit_nobackpack3");
			return;
		}

		EntityAI itemInHands = player.GetItemInHands();
		if (itemInHands && itemInHands.GetInventory().HasInventorySlot(InventorySlots.BACK))
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_tpcancel", "#raidexit_nobackpack4");
			return;
		}

		vector destination = GetRaidExitConfigManager().GetRandomTeleportPositionBP();
		player.SetPosition(destination);

		NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_tpdone", "#raidexit_successexit");

		Print("[RaidExit_NoBackPack] Player " + player.GetIdentity().GetName() + " teleported to " + destination);
	}

}
