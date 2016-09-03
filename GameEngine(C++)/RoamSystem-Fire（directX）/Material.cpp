#include "Common.h"
#include "Material.h"

CMaterial::CMaterial(){
	SetDiffuseColor(0,0,0,0);
	SetAmbientColor(0,0,0,0);
	SetEmissiveColor(0,0,0,0);
	SetSpecularColor(0,0,0,0);
	SetPower(0.0f);
}

CMaterial::~CMaterial(){}

D3DMATERIAL9* CMaterial::GetMaterial(){
	return &m_Material;
}

void CMaterial::SetDiffuseColor(unsigned char red, unsigned char green, unsigned char blue,unsigned char alpha)
{
  m_Material.Diffuse.r=1.0f / 255.0f * (float)red;
  m_Material.Diffuse.g=1.0f / 255.0f * (float)green;
  m_Material.Diffuse.b=1.0f / 255.0f * (float)blue;
  m_Material.Diffuse.a=1.0f / 255.0f * (float)alpha;
}

void CMaterial::SetAmbientColor(unsigned char red, unsigned char green, unsigned char blue,unsigned char alpha)
{
  m_Material.Ambient.r=1.0f / 255.0f * (float)red;
  m_Material.Ambient.g=1.0f / 255.0f * (float)green;
  m_Material.Ambient.b=1.0f / 255.0f * (float)blue;
  m_Material.Ambient.a=1.0f / 255.0f * (float)alpha;
}

void CMaterial::SetEmissiveColor(unsigned char red, unsigned char green, unsigned char blue,unsigned char alpha)
{
  m_Material.Emissive.r=1.0f / 255.0f * (float)red;
  m_Material.Emissive.g=1.0f / 255.0f * (float)green;
  m_Material.Emissive.b=1.0f / 255.0f * (float)blue;
  m_Material.Emissive.a=1.0f / 255.0f * (float)alpha;
}

void CMaterial::SetSpecularColor(unsigned char red, unsigned char green, unsigned char blue,unsigned char alpha)
{
  m_Material.Specular.r=1.0f / 255.0f * (float)red;
  m_Material.Specular.g=1.0f / 255.0f * (float)green;
  m_Material.Specular.b=1.0f / 255.0f * (float)blue;
}

void CMaterial::SetPower(float fPower)
{
  m_Material.Power=fPower;
}