#ifndef _OBJECT_HPP_
#pragma once
#define _OBJECT_HPP_
#include <Windows.h>
namespace ObjectSpace
{
	struct Size
	{
		short width, height;
	};
	class Object
	{
	private:
		HBITMAP m_sprite;
		COORD m_pos;
		COORD m_velocity;
		COORD m_grabPoint;
		Size m_size;

	public:
		Object() = default;
		Object(Object&) = default;
		Object(Object&&) = default;
		Object(HBITMAP hbmp, COORD pos, COORD grabPoint) : m_sprite(hbmp), m_pos(pos), m_grabPoint(grabPoint)
		{
			BITMAP bmp;
			GetObject(m_sprite, sizeof bmp, &bmp);
			m_size.width = (SHORT)bmp.bmWidth;
			m_size.height = (SHORT)bmp.bmHeight;
		}
		Object(LPCSTR path, COORD pos, COORD grabPoint) : m_pos(pos), m_grabPoint(grabPoint)
		{
			m_sprite = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (NULL == m_sprite)
			{
				MessageBox(NULL, "Failed to load sprite", "Error", MB_OK);
				return;
			}
			BITMAP bmp;
			GetObject(m_sprite, sizeof bmp, &bmp);
			m_size.width = (SHORT)bmp.bmWidth;
			m_size.height = (SHORT)bmp.bmHeight;
		}
		bool Initialize(LPCSTR path, COORD pos, COORD grabPoint)
		{
			m_pos = pos; 
			m_grabPoint = grabPoint;
			m_sprite = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (NULL == m_sprite)
			{
				MessageBox(NULL, "Failed to load sprite", "Error", MB_OK);
				return false;
			}
			BITMAP bmp;
			GetObject(m_sprite, sizeof bmp, &bmp);
			m_size.width = (SHORT)bmp.bmWidth;
			m_size.height = (SHORT)bmp.bmHeight;
			return true;
		}

		bool Initialize(HBITMAP hbmp, COORD pos, COORD grabPoint)
		{
			m_pos = pos;
			m_grabPoint = grabPoint;
			m_sprite = hbmp;
			BITMAP bmp;
			GetObject(m_sprite, sizeof bmp, &bmp);
			m_size.width = (SHORT)bmp.bmWidth;
			m_size.height = (SHORT)bmp.bmHeight;
		}

		~Object()
		{
			if (m_sprite)
				DeleteObject(m_sprite);
		}
		COORD GetPos()
		{
			return m_pos;
		}
		COORD GetVelocity()
		{
			return m_velocity;
		}
		COORD GetGrabPoint()
		{
			return m_grabPoint;
		}
		HBITMAP GetSprite()
		{
			return m_sprite;
		}
		Size GetSize()
		{
			return m_size;
		}
	};
	namespace
	{
		void Render(HDC* hdc, Object **objects, SHORT count)
		{
			HDC memDC = CreateCompatibleDC(*hdc);
			HGDIOBJ oldBmp = NULL;
			for (SHORT i = 0; i < count; ++i)
			{
				oldBmp = SelectObject(memDC, objects[i]->GetSprite());
				BitBlt(*hdc,
					objects[i]->GetPos().X, objects[i]->GetPos().Y,
					objects[i]->GetSize().width, objects[i]->GetSize().height,
					memDC, 0, 0, SRCCOPY);
				SelectObject(memDC, oldBmp);
			}
			DeleteObject(oldBmp);
			DeleteDC(memDC);
		}
	}
}
#endif