class RaidExit_Conditional extends House
{
    ref array<ref RaidExitCondition> m_Conditions;
    
    void RaidExit_Conditional()
    {
        m_Conditions = new array<ref RaidExitCondition>;
    }
    
    void SetConditions(array<ref RaidExitCondition> conditions)
    {
        m_Conditions = conditions;
    }
    
    array<ref RaidExitCondition> GetConditions()
    {
        return m_Conditions;
    }

    override void SetActions()
    {
        super.SetActions();
        
        AddAction(ActionExitRaid_Conditional);
    }
}
