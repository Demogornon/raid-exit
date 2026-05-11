class RaidExit_WithMoney extends House
{
    int exitPrice;

    void RaidExit_WithMoney()
    {
        RegisterNetSyncVariableInt("exitPrice");
    }

    void SetExitPrice(int price)
    {
        exitPrice = price;

        SetSynchDirty();
    }

    int GetExitPrice()
    {
        return exitPrice;
    }

    override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionExitRaid_WithMoney);
	}
}