class RaidExit_WithItem extends House
{
    string itemneed;
	ref array<vector> teleportPositions = new array<vector>;

    void RaidExit_WithItem()
    {
        // RegisterNetSyncVariableString("itemneed");
    }

    void SetExitItem(string item)
    {
        itemneed = item;

        // SetSynchDirty();
    }
    void SetTPpoints(array<vector> points)
    {
        teleportPositions = points;

    }


    override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionExitRaid_WithItem);
	}
	
	
}