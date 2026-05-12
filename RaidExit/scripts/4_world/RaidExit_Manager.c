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
class RaidExitWithItemZone
{
    string item;
    int pause;
    string message1;
    string message2;
	
	ref array<vector> startPositions = new array<vector>;
	ref array<vector> teleportPositions = new array<vector>;
	
    void RaidExitWithItemZone(array<vector> pos, array<vector> teleportpos, string itemneed, int paused, string m1, string m2)
    {
        startPositions = pos;
        teleportPositions = teleportpos;
        item = itemneed;
        pause = paused;
		message1 = m1;
		message2 = m2;
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

static ref array<ref RaidExitstarter> g_RaidExitstarters;
static ref array<ref RaidExitstarter> GetRaidExitstarters()
{
    if (!g_RaidExitstarters)
        g_RaidExitstarters = new array<ref RaidExitstarter>;

    return g_RaidExitstarters;
}


// ===== ОСНОВНОЙ КЛАСС =====
class RaidExitConfigManager
{
    const static string CONFIG_PATH = "$profile:RaidExit/settings.json";
    const static string CONFIG_DIR = "$profile:RaidExit";

    // Конфигурируемые данные
    ref array<ref RaidExitZone> RaidExitZones = new array<ref RaidExitZone>;
    ref array<ref RaidExitNoBackpackZone> RaidExitNoBackpackZones = new array<ref RaidExitNoBackpackZone>;
    ref array<ref RaidExitWithMoneyZone> RaidExitWithMoneyZones = new array<ref RaidExitWithMoneyZone>;
    ref array<ref RaidExitWithItemZone> RaidExitWithItemZones = new array<ref RaidExitWithItemZone>;
	
	
    ref array<vector> TeleportPositions = new array<vector>;
    ref array<vector> TeleportPositionsBP = new array<vector>;
    ref array<vector> TeleportPositionsM = new array<vector>;

    void RaidExitConfigManager()
    {
        Load();
        SpawnAll();
    }

    // ====== ЗАГРУЗКА / СОХРАНЕНИЕ ======
	void Load()
{
    CreateDefault(); 

    if (FileExist(CONFIG_PATH))
    {
        JsonFileLoader<RaidExitConfigManager>.JsonLoadFile(CONFIG_PATH, this);
        Print("[RaidExitConfig] Settings loaded and merged!");
        
        Save(); 
    }
    else
    {
        Save();
        Print("[RaidExitConfig] Default settings created!");
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
        RaidExitWithMoneyZones.Insert(new RaidExitWithMoneyZone("5200 0 5000", 5000));

        // Примерные координаты телепорта
        TeleportPositions.Insert("4000 0 4000");
        TeleportPositions.Insert("6000 0 6000");
        TeleportPositionsBP.Insert("4000 0 6000");
        TeleportPositionsBP.Insert("6000 0 6000");
        TeleportPositionsM.Insert("4000 0 6000");
        TeleportPositionsM.Insert("6000 0 6000");
		
		// Профайл 1: 2 старта, 1 телепорт, требуемый предмет
    array<vector> s1 = new array<vector>;
    s1.Insert(Vector(1200, 0, 3400));
    s1.Insert(Vector(1210, 0, 3410));

    array<vector> t1 = new array<vector>;
    t1.Insert(Vector(5000, 0, 5000));

    RaidExitWithItemZones.Insert(new RaidExitWithItemZone(s1, t1, "Bandage", 100,"Шапка сообщения","Тело сообщения"));

    // Профайл 2: 1 старт, 1 телепорт
    array<vector> s2 = new array<vector>;
    s2.Insert(Vector(8000, 0, 2000));

    array<vector> t2 = new array<vector>;
    t2.Insert(Vector(1000, 0, 8000));

    RaidExitWithItemZones.Insert(new RaidExitWithItemZone(s2, t2, "AmmoBox", 100,"Шапка сообщения","Тело сообщения"));
		
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

        foreach (RaidExitWithMoneyZone zone3 : RaidExitWithMoneyZones)
            SpawnRaidExitWithMoney(zone3);
        foreach (RaidExitWithItemZone zone4 : RaidExitWithItemZones)
            SpawnRaidExitWithItemZone(zone4);
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
    void SpawnRaidExitWithItemZone(RaidExitWithItemZone zone)
    {
		
    int pause=zone.pause;
	if (pause==0)
	{
    string item=zone.item;
	ref array<vector> teleportPositions = zone.teleportPositions;
    vector pos = GetRandomVectorPosition(zone.startPositions);

        RaidExit_WithItem obj = RaidExit_WithItem.Cast(GetGame().CreateObjectEx("RaidExit_WithItem", pos, ECE_SETUP));
        if (obj)
        {
            obj.SetExitItem(item);
            obj.SetTPpoints(teleportPositions);
            Print("[RaidExitManager] Spawned RaidExit_WithItem at " + pos + " with item: " + item);
        }
	}
	else
	{
		new RaidExitstarter(zone);
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
    vector GetRandomVectorPosition(array<vector> apos)
    {
        if (apos && apos.Count() > 0)
        {
            int index = Math.RandomInt(0, apos.Count());
            return apos[index];
        }

        return "0 0 0";
    }
}

class RaidExitstarter
{
	RaidExitWithItemZone zone; 
    int m_pause;
	void RaidExitstarter(RaidExitWithItemZone z)
	{
	RaidExitWithItemZone zone=z; 
    m_pause=zone.pause;
	
	GetRaidExitstarters().Insert(this);	
	WaitPause();
	}
	void WaitPause()
	{

		int currentTime=GetGame().GetTime()/1000; 
		
		if(currentTime>m_pause)
		{
			Start();
		}
		else
		{
		g_Game.GetCallQueue( CALL_CATEGORY_SYSTEM ).CallLater(WaitPause, 60000, false);
		}
	}
	
	
	void Start()
	{
	string item=zone.item;
    string message1=zone.message1;
    string message2=zone.message2;
	
	array<vector> startPositions = zone.startPositions;

		if (!startPositions || startPositions.Count() == 0) return;
		vector pos = GetRaidExitConfigManager().GetRandomVectorPosition(startPositions);
        RaidExit_WithItem obj = RaidExit_WithItem.Cast(GetGame().CreateObjectEx("RaidExit_WithItem", pos, ECE_SETUP));
        if (obj)
        {
            obj.SetExitItem(item);
            obj.SetTPpoints(zone.teleportPositions);
			
			NotificationSystem.SendNotificationToPlayerIdentityExtended(NULL, 5, message1, message2);

            Print("[RaidExitManager] Spawned RaidExit_WithItem at " + pos + " with item: " + item+ " with pause: "+m_pause);
        }
		GetRaidExitstarters().RemoveItem(this);
	}
}