class RaidExitTrigger : CylinderTrigger
{
    ref map<PlayerBase, ref Timer> m_TeleportTimers = new map<PlayerBase, ref Timer>();

    void RaidExitTrigger()
    {
    }

    override bool CanAddObjectAsInsider(Object object)
    {
        PlayerBase player = PlayerBase.Cast(object);
        if (!player)
            return false;

        return player.IsAlive() && !player.IsUnconscious();
    }

    override void OnEnter(Object obj)
    {
        PlayerBase player = PlayerBase.Cast(obj);
        if (!player || !player.IsAlive())
            return;

        if (m_TeleportTimers.Contains(player))
            return;

        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_exit", "#raidexit_tp10sec_trigger");

        Timer timer = new Timer(CALL_CATEGORY_SYSTEM);
        m_TeleportTimers.Insert(player, timer);

        timer.Run(10.0, this, "PerformTeleport", new Param1<PlayerBase>(player), false);
    }

    void PerformTeleport(PlayerBase player)
    {
        if (m_TeleportTimers.Contains(player))
            m_TeleportTimers.Remove(player);

        if (!player || !player.IsAlive())
            return;

        if (!GetInsiderForObject(player))
        {
            NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_exit", "#raidexit_triggerleave");
            return;
        }

        vector destination = GetRaidExitConfigManager().GetRandomTeleportPosition();
        player.SetPosition(destination);

        NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_exit", "#raidexit_successexit");

        Print("[RaidExitTrigger] Player " + player.GetIdentity().GetName() + " teleported to " + destination);
    }

    override void OnLeave(Object obj)
    {
        PlayerBase player = PlayerBase.Cast(obj);
        if (!player)
            return;

        if (m_TeleportTimers.Contains(player))
        {
            m_TeleportTimers.Get(player).Stop();
            m_TeleportTimers.Remove(player);

            NotificationSystem.SendNotificationToPlayerIdentityExtended(player.GetIdentity(), 5, "#raidexit_exit", "#raidexit_triggerleave");
        }
    }
}
