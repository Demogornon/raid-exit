class ActionExitRaid_NoItemInHands: ActionInteractBase
{
    void ActionExitRaid_NoItemInHands()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_Text = "#action_raidexit_noiteminhands";
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

        RaidExit_NoItemInHands exitObj = RaidExit_NoItemInHands.Cast(action_data.m_Target.GetObject());
        if (!exitObj)
            return;

        string requiredItem = exitObj.GetRequiredItem();
        string errorMessage = exitObj.GetErrorMessage();

        // Проверка предмета в руках
        EntityAI itemInHands = player.GetItemInHands();
        if (itemInHands && itemInHands.GetType() == requiredItem)
        {
            NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_unavailable", errorMessage);
            return;
        }

        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_teleportation", "#raidexit_tp10sec");

        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TeleportPlayerAfterDelay, 10000, false, player, exitObj);
    }

    void TeleportPlayerAfterDelay(PlayerBase player, RaidExit_NoItemInHands exitObj)
    {
        if (!player || !player.IsAlive())
        {
            return;
        }

        // Финальная проверка предмета в руках перед телепортацией
        string requiredItem = exitObj.GetRequiredItem();
        string errorMessage = exitObj.GetErrorMessage();
        
        EntityAI itemInHands = player.GetItemInHands();
        if (itemInHands && itemInHands.GetType() == requiredItem)
        {
            NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_tpcancel", errorMessage);
            return;
        }

        vector destination = GetRaidExitConfigManager().GetRandomTeleportPositionNoItemInHands();
        player.SetPosition(destination);

        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_tpdone", "#raidexit_successexit");

        Print("[RaidExit_NoItemInHands] Player " + player.GetIdentity().GetName() + " teleported to " + destination);
    }

}
