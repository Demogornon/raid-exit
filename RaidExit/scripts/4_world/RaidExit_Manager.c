class RaidExitCondition
{
    // Тип проверки: 0 = предмет в руках, 1 = предмет в инвентаре
    int conditionType;
    
    // Класс предмета для проверки (например "AKM", "Ammobox_308WinTracer")
    string itemClass;
    
    // Сообщение об ошибке (ключ из stringtable)
    string errorMessage;
    
    void RaidExitCondition(int type, string item, string error)
    {
        conditionType = type;
        itemClass = item;
        errorMessage = error;
    }
}

class RaidExitConditionalZone
{
    vector position;
    ref array<ref RaidExitCondition> conditions;
    
    void RaidExitConditionalZone(vector pos)
    {
        position = pos;
        conditions = new array<ref RaidExitCondition>;
    }
}

class RaidExitZone
{
    vector position;
    float radius;

    void RaidExitZone(vector pos, float rad)
    {
        position = pos;
        radius = rad;
    }
}

class RaidExitNoBackpackZone
{
    vector position;

    void RaidExitNoBackpackZone(vector pos)
    {
        position = pos;
    }
}

class RaidExitNoItemInHandsZone
{
    vector position;
    string requiredItemClass;
    string errorMessage;

    void RaidExitNoItemInHandsZone(vector pos, string itemClass, string error)
    {
        position = pos;
        requiredItemClass = itemClass;
        errorMessage = error;
    }
}

class RaidExitWithMoneyZone
{
    vector position;
    int price;

    void RaidExitWithMoneyZone(vector pos, int cost)
    {
        position = pos;
        price = cost;
    }
}

// ===== ГЛОБАЛЬНАЯ ССЫЛКА НА МЕНЕДЖЕР =====
static ref RaidExitConfigManager g_RaidExitConfigManager;
static ref RaidExitConfigManager GetRaidExitConfigManager()
{
    if (!g_RaidExitConfigManager)
        g_RaidExitConfigManager = new RaidExitConfigManager();

    return g_RaidExitConfigManager;
}

// ===== ОСНОВНОЙ КЛАСС =====
class RaidExitConfigManager
{
    const static string CONFIG_PATH = "$profile:RaidExit/settings.json";
    const static string CONFIG_DIR = "$profile:RaidExit";

    // Конфигурируемые данные
    ref array<ref RaidExitZone> RaidExitZones = new array<ref RaidExitZone>;
    ref array<ref RaidExitNoBackpackZone> RaidExitNoBackpackZones = new array<ref RaidExitNoBackpackZone>;
    ref array<ref RaidExitNoItemInHandsZone> RaidExitNoItemInHandsZones = new array<ref RaidExitNoItemInHandsZone>;
    ref array<ref RaidExitWithMoneyZone> RaidExitWithMoneyZones = new array<ref RaidExitWithMoneyZone>;
    ref array<ref RaidExitConditionalZone> RaidExitConditionalZones = new array<ref RaidExitConditionalZone>;

    // 🔹 Новое: массив координат телепорта
    ref array<vector> TeleportPositions = new array<vector>;
    ref array<vector> TeleportPositionsBP = new array<vector>;
    ref array<vector> TeleportPositionsM = new array<vector>;
    ref array<vector> TeleportPositionsConditional = new array<vector>;
    ref array<vector> TeleportPositionsNoItemInHands = new array<vector>;

    void RaidExitConfigManager()
    {
        Load();
        SpawnAll();
    }

    // ====== ЗАГРУЗКА / СОХРАНЕНИЕ ======
    void Load()
    {
        if (!FileExist(CONFIG_PATH))
        {
            CreateDefault();
            Save();
            Print("[RaidExitConfig] Default settings created!");
        }
        else
        {
            JsonFileLoader<RaidExitConfigManager>.JsonLoadFile(CONFIG_PATH, this);
            Print("[RaidExitConfig] Settings loaded!");
        }
    }

    void Save()
    {
        CreateDirectory(CONFIG_DIR);
        JsonFileLoader<RaidExitConfigManager>.JsonSaveFile(CONFIG_PATH, this);
    }

    void CreateDirectory(string path)
    {
        if (!FileExist(path))
        {
            MakeDirectory(path);
            Print("[RaidExitConfig] Directory created: " + path);
        }
    }

    void CreateDefault()
    {
        // Примерные данные
        RaidExitZones.Insert(new RaidExitZone("5000 0 5000", 10));
        RaidExitNoBackpackZones.Insert(new RaidExitNoBackpackZone("5100 0 5000"));
        RaidExitNoItemInHandsZones.Insert(new RaidExitNoItemInHandsZone("5150 0 5000", "AKM", "#raidexit_noiteminhands"));
        RaidExitWithMoneyZones.Insert(new RaidExitWithMoneyZone("5200 0 5000", 5000));

        // Примерные координаты телепорта
        TeleportPositions.Insert("4000 0 4000");
        TeleportPositions.Insert("6000 0 6000");
        TeleportPositionsBP.Insert("4000 0 6000");
        TeleportPositionsBP.Insert("6000 0 6000");
        TeleportPositionsM.Insert("4000 0 6000");
        TeleportPositionsM.Insert("6000 0 6000");
        TeleportPositionsConditional.Insert("4000 0 6000");
        TeleportPositionsConditional.Insert("6000 0 6000");
        TeleportPositionsNoItemInHands.Insert("4000 0 6000");
        TeleportPositionsNoItemInHands.Insert("6000 0 6000");
    }

    // ====== СПАВН ВСЕХ ОБЪЕКТОВ ======
    void SpawnAll()
    {
        if (RaidExitZones && RaidExitZones.Count() > 0)
        {
            int index = Math.RandomInt(0, RaidExitZones.Count());
            SpawnRaidExit(RaidExitZones[index]);
        }


        foreach (RaidExitNoBackpackZone zone2 : RaidExitNoBackpackZones)
            SpawnRaidExitNoBackPack(zone2);

        foreach (RaidExitNoItemInHandsZone zone5 : RaidExitNoItemInHandsZones)
            SpawnRaidExitNoItemInHands(zone5);

        foreach (RaidExitWithMoneyZone zone3 : RaidExitWithMoneyZones)
            SpawnRaidExitWithMoney(zone3);
        
        foreach (RaidExitConditionalZone zone4 : RaidExitConditionalZones)
            SpawnRaidExitConditional(zone4);
    }

    // --- RaidExit с триггером ---
    void SpawnRaidExit(RaidExitZone zone)
    {
        vector pos = zone.position;
        float radius = zone.radius;

        RaidExit obj = RaidExit.Cast(GetGame().CreateObjectEx("RaidExit", pos, ECE_SETUP));
        if (obj)
        {
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(CreateRaidExitTrigger, 100, false, obj, radius);
            Print("[RaidExitManager] Spawned RaidExit with trigger at " + pos);
        }
    }

    void CreateRaidExitTrigger(RaidExit obj, float radius)
    {
        if (!obj)
            return;

        vector pos = obj.GetPosition();
        RaidExitTrigger trigger;

        if (Class.CastTo(trigger, GetGame().CreateObjectEx("RaidExitTrigger", pos, ECE_NONE)))
        {
            trigger.SetCollisionCylinderTwoWay(radius, -1, 2);
            trigger.SetPosition(pos);
            trigger.Update();
            Print("[RaidExitManager] Created trigger at " + pos + " (radius: " + radius + ")");
        }
    }

    // --- RaidExit_NoBackPack ---
    void SpawnRaidExitNoBackPack(RaidExitNoBackpackZone zone)
    {
        vector pos = zone.position;

        RaidExit_NoBackPack obj = RaidExit_NoBackPack.Cast(GetGame().CreateObjectEx("RaidExit_NoBackPack", pos, ECE_SETUP));
        if (obj)
            Print("[RaidExitManager] Spawned RaidExit_NoBackPack at " + pos);
    }

    // --- RaidExit_NoItemInHands ---
    void SpawnRaidExitNoItemInHands(RaidExitNoItemInHandsZone zone)
    {
        vector pos = zone.position;

        RaidExit_NoItemInHands obj = RaidExit_NoItemInHands.Cast(GetGame().CreateObjectEx("RaidExit_NoItemInHands", pos, ECE_SETUP));
        if (obj)
        {
            obj.SetRequiredItem(zone.requiredItemClass);
            obj.SetErrorMessage(zone.errorMessage);
            Print("[RaidExitManager] Spawned RaidExit_NoItemInHands at " + pos + " for item: " + zone.requiredItemClass);
        }
    }

    // --- RaidExit_WithMoney ---
    void SpawnRaidExitWithMoney(RaidExitWithMoneyZone zone)
    {
        vector pos = zone.position;
        int cost = zone.price;

        RaidExit_WithMoney obj = RaidExit_WithMoney.Cast(GetGame().CreateObjectEx("RaidExit_WithMoney", pos, ECE_SETUP));
        if (obj)
        {
            obj.SetExitPrice(cost)
            Print("[RaidExitManager] Spawned RaidExit_WithMoney at " + pos + " with price: " + cost);
        }
    }

    // --- RaidExit_Conditional ---
    void SpawnRaidExitConditional(RaidExitConditionalZone zone)
    {
        vector pos = zone.position;

        RaidExit_Conditional obj = RaidExit_Conditional.Cast(GetGame().CreateObjectEx("RaidExit_Conditional", pos, ECE_SETUP));
        if (obj)
        {
            obj.SetConditions(zone.conditions);
            Print("[RaidExitManager] Spawned RaidExit_Conditional at " + pos + " with " + zone.conditions.Count() + " conditions");
        }
    }

    // ====== ВСПОМОГАТЕЛЬНАЯ ФУНКЦИЯ ======

    // Получение случайной позиции телепорта
    vector GetRandomTeleportPosition()
    {
        if (TeleportPositions && TeleportPositions.Count() > 0)
        {
            int index = Math.RandomInt(0, TeleportPositions.Count());
            return TeleportPositions[index];
        }

        return "0 0 0";
    }
    vector GetRandomTeleportPositionBP()
    {
        if (TeleportPositionsBP && TeleportPositionsBP.Count() > 0)
        {
            int index = Math.RandomInt(0, TeleportPositionsBP.Count());
            return TeleportPositionsBP[index];
        }

        return "0 0 0";
    }
    vector GetRandomTeleportPositionM()
    {
        if (TeleportPositionsM && TeleportPositionsM.Count() > 0)
        {
            int index = Math.RandomInt(0, TeleportPositionsM.Count());
            return TeleportPositionsM[index];
        }

        return "0 0 0";
    }
    
    vector GetRandomTeleportPositionConditional()
    {
        if (TeleportPositionsConditional && TeleportPositionsConditional.Count() > 0)
        {
            int index = Math.RandomInt(0, TeleportPositionsConditional.Count());
            return TeleportPositionsConditional[index];
        }

        return "0 0 0";
    }
    
    vector GetRandomTeleportPositionNoItemInHands()
    {
        if (TeleportPositionsNoItemInHands && TeleportPositionsNoItemInHands.Count() > 0)
        {
            int index = Math.RandomInt(0, TeleportPositionsNoItemInHands.Count());
            return TeleportPositionsNoItemInHands[index];
        }

        return "0 0 0";
    }
}
