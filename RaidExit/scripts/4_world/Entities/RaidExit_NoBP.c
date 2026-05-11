class RaidExit_NoBackPack extends House
{
    void RaidExit_NoBackPack()
    {
    }

	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionExitRaid_NoBP);
	}
}