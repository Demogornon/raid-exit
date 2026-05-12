modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);    

        actions.Insert(ActionExitRaid_NoBP);
        actions.Insert(ActionExitRaid_WithMoney);
        actions.Insert(ActionExitRaid_WithItem);
    }
}