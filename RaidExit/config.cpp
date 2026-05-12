class CfgPatches
{
    class RaidExit
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Data", "DZ_Sounds_Effects"};
    };
};

class CfgMods
{
    class RaidExit
    {
        type = "mod";
        dependencies[] = {"Mission","World","Game"};
        class defs
        {
            class missionScriptModule
            {
                value = "";
                files[] = {"RaidExit/scripts/5_mission"};
            };
			class worldScriptModule
            {
                value = "";
                files[] = {"RaidExit/scripts/4_world"};
            };
			class gameScriptModule
			{
				value = "";
				files[] = {"RaidExit/scripts/3_game"};
			};
        };
    };
};

class CfgVehicles
{
	class HouseNoDestruct;
	class RaidExit: HouseNoDestruct
	{
		scope=1;
		model="\RaidExit\empty.p3d";
	};
	class RaidExit_NoBackPack: HouseNoDestruct
	{
		scope=1;
		model="\RaidExit\empty.p3d";
	};
	class RaidExit_WithMoney: HouseNoDestruct
	{
		scope=1;
		model="\RaidExit\empty.p3d";
	};
	class RaidExit_WithItem: HouseNoDestruct
	{
		scope=1;
		model="\RaidExit\empty.p3d";
	};
};

class CfgSounds
{
	class default
	{
		name="";
		titles[]={};
	};
	class raidexit_smoke: default
	{
		sound[]=
		{
			"DZ\sounds\weapons\grenades\smokegranade\SmokeGrenade_RDG2_active_loop",
			3,
			1,
			30
		};
	};
};
