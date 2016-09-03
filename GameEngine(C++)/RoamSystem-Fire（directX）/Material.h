#pragma once

class CMaterial  
{
private:
	D3DMATERIAL9  m_Material;
public:
	CMaterial();
	~CMaterial();
	D3DMATERIAL9 *GetMaterial();
	void SetDiffuseColor(unsigned char red, unsigned char green, unsigned char blue,unsigned char alpha);
	void SetAmbientColor(unsigned char red, unsigned char green, unsigned char blue,unsigned char alpha);
	void SetSpecularColor(unsigned char red, unsigned char green, unsigned char blue,unsigned char alpha);
	void SetEmissiveColor(unsigned char red, unsigned char green, unsigned char blue,unsigned char alpha);
	void SetPower(float fPower);
};