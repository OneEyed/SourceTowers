#ifndef MKS_IMAGEFX_H
#define MKS_IMAGEFX_H

#include <vgui_controls/Panel.h>
using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Panel that holds a single image
//-----------------------------------------------------------------------------
class ImageFX : public Panel
{
	DECLARE_CLASS_SIMPLE( ImageFX, Panel );

public:
	ImageFX(Panel *parent, const char *imageName, const char *szPanelName);
	~ImageFX();

//Derived Functions
public:
	virtual void Paint();
	virtual void ApplySchemeSettings(IScheme *pScheme);

//Functions for use with this object.
public:
	void SetImage(const char *imageName)	{ m_szImageName = (char*)imageName; };
	char *GetImageName()					{ return m_szImageName; };

	void SetImageSize( float w, float h );
	void SetImageSize( Vector2D size )	{ SetImageSize( size.x, size.y ); };
	void SetScale( float scale )		{ m_flScale		= scale;		SetImageSize(m_flHeight,m_flWidth); };
	void SetScale( float w, float h )	{ m_flScaleW=w; m_flScaleH=h;   SetImageSize(m_flHeight,m_flWidth); };
	void SetAngle( float angle )		{ m_flAngle		= angle;	};
	void SetFade( float fade )			{ m_flFade		= fade;		};
	void SetRadius( float radius )		{ m_Radius		= radius;	};
	void SetPosEx( Vector2D pos )		{ m_vPos		= pos;		};
	void SetPosEx( float x, float y )	{ m_vPos.x=x;	m_vPos.y=y;	};

	void SetCustomPoints( bool points )			{ m_bCustomPoints = points; };
	bool IsCustomPoints()						{ return m_bCustomPoints;	};
	void SetPoints( const Vertex_t points[], int count )
	{
		for(int x=0; x<count; x++)
			m_vxPoints[x] = points[x];
	};
	Vertex_t* GetPoints( void )					{ return m_vxPoints;		};

	float		GetScale()						{ return m_flScale;							};
	void		GetScale( float &w, float &h )	{ w = m_flScaleW; h = m_flScaleH;			};
	float		GetAngle()						{ return m_flAngle;							};
	float		GetFade()						{ return m_flFade;							};
	Vector2D	GetImageSize()					{ return Vector2D( m_flWidth, m_flHeight ); };
	void		GetImageSize(float &w, float &h){ w = m_flWidth; h = m_flHeight;			};
	Vector2D	GetPosEx()						{ return m_vPos;							};
	void		GetPosEx(float &x, float &y)	{ x = m_vPos.x; y = m_vPos.y;				};

//Rotation Effect Helpers
private:
	void ScreenToAxis( Vector2D &vec );
	void AxisToScreen( Vector2D &vec );
	void SetAxisOrigin( Vector2D &vec );
	void Rotate( Vector2D &vec, float radian = 0.0f );

private:
	int m_iImageId;
	char *m_szImageName;

	float m_flScale;
	float m_flScaleW;
	float m_flScaleH;

	float m_flWidth;
	float m_flHeight;
	float m_Radius;
	float m_flAngle;
	float m_flRadOffset;
	float m_flFade;

	bool m_bCustomPoints;

	Vertex_t m_vxPoints[4];

	Vector2D m_vPos;
};

#endif // MKS_IMAGEFX_H