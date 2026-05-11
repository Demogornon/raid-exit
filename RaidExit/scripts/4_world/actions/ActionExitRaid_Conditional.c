class ActionExitRaid_Conditional: ActionInteractBase
{
    void ActionExitRaid_Conditional()
    {
        m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
        m_Text = "#action_raidexit_conditional";
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

        RaidExit_Conditional exitObj = RaidExit_Conditional.Cast(action_data.m_Target.GetObject());
        if (!exitObj)
            return;

        array<ref RaidExitCondition> conditions = exitObj.GetConditions();
        if (!conditions || conditions.Count() == 0)
        {
            // Если условий нет - телепортируем сразу
            TeleportPlayerAfterDelay(player);
            return;
        }

        // Проверка всех условий
        foreach (RaidExitCondition condition : conditions)
        {
            if (!CheckCondition(player, condition))
            {
                NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_unavailable", condition.errorMessage);
                return;
            }
        }

        // Все условия выполнены
        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_teleportation", "#raidexit_tp10sec");
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(TeleportPlayerAfterDelay, 10000, false, player);
    }

    bool CheckCondition(PlayerBase player, RaidExitCondition condition)
    {
        if (condition.conditionType == 0) // Проверка предмета в руках
        {
            EntityAI itemInHands = player.GetItemInHands();
            if (!itemInHands || itemInHands.GetType() != condition.itemClass)
            {
                return false;
            }
            return true;
        }
        else if (condition.conditionType == 1) // Проверка предмета в инвентаре
        {
            EntityAI itemInInventory = player.FindAttachmentBySlotName(condition.itemClass);
            if (!itemInInventory)
            {
                // Пробуем найти в инвентаре
                for (int i = 0; i < player.GetInventory().GetCargoItemCount(); i++)
                {
                    EntityAI cargoItem = player.GetInventory().GetCargoItem(i);
                    if (cargoItem && cargoItem.GetType() == condition.itemClass)
                    {
                        return true;
                    }
                }
                return false;
            }
            return true;
        }

        return false;
    }

    void TeleportPlayerAfterDelay(PlayerBase player)
    {
        if (!player || !player.IsAlive())
        {
            return;
        }

        // Финальная проверка всех условий перед телепортацией
        // Можно добавить повторную проверку если нужно

        vector destination = GetRaidExitConfigManager().GetRandomTeleportPositionConditional();
        player.SetPosition(destination);

        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_tpdone", "#raidexit_successexit");

        Print("[RaidExit_Conditional] Player " + player.GetIdentity().GetName() + " teleported to " + destination);
    }
}
