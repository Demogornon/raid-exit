class RaidExit_NoItemInHands extends House
{
    string m_RequiredItemClass;
    string m_ErrorMessage;
    
    void RaidExit_NoItemInHands()
    {
        m_RequiredItemClass = "";
        m_ErrorMessage = "#raidexit_noiteminhands";
    }
    
    void SetRequiredItem(string itemClass)
    {
        m_RequiredItemClass = itemClass;
    }
    
    string GetRequiredItem()
    {
        return m_RequiredItemClass;
    }
    
    void SetErrorMessage(string error)
    {
        m_ErrorMessage = error;
    }
    
    string GetErrorMessage()
    {
        return m_ErrorMessage;
    }

    override void SetActions()
    {
        super.SetActions();
        
        AddAction(ActionExitRaid_NoItemInHands);
    }
}
