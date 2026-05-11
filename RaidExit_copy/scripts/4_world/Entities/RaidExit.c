class RaidExit extends House
{
	PointLightBase	m_Light;
	SoundOnVehicle m_raidexit;
    Particle m_ParticleSmoke;

    void RaidExit()
    {
		if ( GetGame().IsClient() )
		{
			PlayParticle();
			
			m_Light = PointLightBase.Cast( ScriptedLightBase.CreateLight(Raid_Light, "0 1 0", 0.5) );
			m_Light.AttachOnObject(this, "0 1 0", "0 0 0");
			
			m_raidexit = PlaySoundLoop("raidexit_smoke", 20);
		}
    }

    void PlayParticle()
    {
        if (!m_ParticleSmoke)
        {
            m_ParticleSmoke = Particle.PlayOnObject(ParticleList.GRENADE_M18_GREEN_LOOP, this);
        }
    }

    void ~RaidExit()
    {
        if (m_ParticleSmoke)
        {
            m_ParticleSmoke.Stop();
        }
		if (m_Light) 
		{	
			m_Light.FadeOut();
		}
		if (m_raidexit) 
		{
			GetGame().ObjectDelete( m_raidexit );
		}
    }
}

class Raid_Light extends PointLightBase
{		
	void Raid_Light()
	{
		SetVisibleDuringDaylight(true);
		SetRadiusTo(8);
		SetBrightnessTo(1.3);
		SetCastShadow(false);
		SetFadeOutTime(4);
		SetDiffuseColor(0.0, 0.8, 0.1);
		SetAmbientColor(0.0, 0.8, 0.1);
		SetFlareVisible(false);
	}
}
