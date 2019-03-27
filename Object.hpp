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
		HBITMAP m_mask;
		COORD m_pos;
		COORD m_velocity;
		COORD m_grabPoint;
		Size m_size;

	public:
		Object() = default;
		Object(Object&) = default;
		Object(Object&&) = default;
		Object(HBITMAP hbmp, HBITMAP mask, COORD pos, COORD grabPoint) : m_sprite(hbmp), m_mask(mask), m_pos(pos), m_grabPoint(grabPoint)
		{
			BITMAP bmp;
			GetObject(m_sprite, sizeof bmp, &bmp);
			m_size.width = (SHORT)bmp.bmWidth;
			m_size.height = (SHORT)bmp.bmHeight;
		}
		Object(LPCSTR path, LPCSTR maskPath, COORD pos, COORD grabPoint) : m_pos(pos), m_grabPoint(grabPoint)
		{
			m_sprite = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (NULL == m_sprite)
			{
				MessageBox(NULL, "Failed to load sprite", "Error", MB_OK);
				return;
			}
			m_mask = (HBITMAP)LoadImage(NULL, maskPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (NULL == m_mask)
			{
				MessageBox(NULL, "Failed to load mask", "Error", MB_OK);
				return;
			}
			BITMAP bmp;
			GetObject(m_sprite, sizeof bmp, &bmp);	
			m_size.width = (SHORT)bmp.bmWidth;
			m_size.height = (SHORT)bmp.bmHeight;
		}
		bool Initialize(LPCSTR path, LPCSTR maskPath, COORD pos, COORD grabPoint)
		{
			m_pos = pos; 
			m_grabPoint = grabPoint;
			m_sprite = (HBITMAP)LoadImage(NULL, path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (NULL == m_sprite)
			{
				MessageBox(NULL, "Failed to load sprite", "Error", MB_OK);
				return false;
			}
			m_mask = (HBITMAP)LoadImage(NULL, maskPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			if (NULL == m_mask)
			{
				MessageBox(NULL, "Failed to load mask", "Error", MB_OK);
				return false;
			}
			BITMAP bmp;
			GetObject(m_sprite, sizeof bmp, &bmp);
			m_size.width = (SHORT)bmp.bmWidth;
			m_size.height = (SHORT)bmp.bmHeight;
			return true;
		}

		bool Initialize(HBITMAP hbmp, HBITMAP mask, COORD pos, COORD grabPoint)
		{
			m_pos = pos;
			m_grabPoint = grabPoint;
			m_sprite = hbmp;
			m_mask = mask;
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
		HBITMAP GetMask()
		{
			return m_mask;
		}
		Size GetSize()
		{
			return m_size;
		}
	};
	namespace
	{
		/* This function render objects. But it going to be changed soon for to support double buffering */
		void Render(HDC* hdc, Object **objects, SHORT count)
		{
			HDC memDC = CreateCompatibleDC(*hdc);
			HDC maskDC = CreateCompatibleDC(*hdc);
			HGDIOBJ oldBmp = NULL;
			HGDIOBJ oldMask = NULL;

			for (SHORT i = 0; i < count; ++i)
			{
				oldBmp = SelectObject(memDC, objects[i]->GetSprite());
				oldMask = SelectObject(maskDC, objects[i]->GetMask());
				/* Do drawing with mask */
				BitBlt(*hdc,
					objects[i]->GetPos().X, objects[i]->GetPos().Y,
					objects[i]->GetSize().width, objects[i]->GetSize().height,
					memDC, 0, 0, SRCINVERT);
				BitBlt(*hdc,
					objects[i]->GetPos().X, objects[i]->GetPos().Y,
					objects[i]->GetSize().width, objects[i]->GetSize().height,
					maskDC, 0, 0, SRCAND);
				BitBlt(*hdc,
					objects[i]->GetPos().X, objects[i]->GetPos().Y,
					objects[i]->GetSize().width, objects[i]->GetSize().height,
					memDC, 0, 0, SRCINVERT);

				SelectObject(memDC, oldBmp);
				SelectObject(maskDC, oldMask);
			}
			DeleteObject(oldMask);
			DeleteObject(oldBmp);
			DeleteDC(memDC);
			DeleteDC(maskDC);
		}
	}
}
#endif