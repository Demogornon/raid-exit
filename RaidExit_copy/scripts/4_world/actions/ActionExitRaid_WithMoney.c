class ActionExitRaid_WithMoney: ActionInteractBase
{
	void ActionExitRaid_WithMoney()
	{
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_Text = "#action_raidexit_money";
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		RaidExit_WithMoney exit = RaidExit_WithMoney.Cast(target.GetObject());
		if (!exit)
			return false;

		int price = exit.GetExitPrice();
		m_Text = "#action_raidexit_money" + " (" + price + ")";

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

		RaidExit_WithMoney exit = RaidExit_WithMoney.Cast(action_data.m_Target.GetObject());
		if (!exit)
			return;

		int price = exit.exitPrice;
		int playerMoney = 0;
		bool hasEnough = false;

		#ifdef TRADERPLUS
		playerMoney = TraderPlusHelper.ModdedGetPlayerMoney(player);
		hasEnough = (playerMoney >= price);
		#endif

		#ifdef EXPANSIONMODMARKET
		if (!hasEnough)
		{
			array<int> monies = new array<int>;
			playerMoney = ExpansionMarketModule.GetInstance().GetPlayerWorth(player, monies, NULL, false);
			hasEnough = (playerMoney >= price);
		}
		#endif

		if (!hasEnough)
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_exit", "#raidexit_notenoughmoney");
			return;
		}

		bool success = false;

		#ifdef TRADERPLUS
		success = TraderPlusHelper.ModdedRemoveMoneyFromPlayer(player, price);
		#endif

		#ifdef EXPANSIONMODMARKET
		if (!success)
		{
			success = ExpansionMarketModule.GetInstance().RemoveMoney(price, player, NULL);
		}
		#endif

		if (!success)
		{
			NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_exit", "#raidexit_errormoney");
			return;
		}

		NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_exit", "#raidexit_tp10sec");
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TeleportPlayer, 10000, false, player);
	}

	void TeleportPlayer(PlayerBase player)
	{
		if (!player || !player.IsAlive())
			return;

		vector targetPos = GetRaidExitConfigManager().GetRandomTeleportPositionM();

		// Телепортируем игрока
		player.SetPosition(targetPos);

		NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_exit", "#raidexit_successexit");
	}

}
