#ifndef _OBJECT_HPP_
#pragma once
#define _OBJECT_HPP_
#include <Windows.h>
#include <iostream>
#include <chrono>
namespace ObjectSpace
{
	struct SIZE
	{
		unsigned short width, height;
	};
	class Object
	{
	private:
		HBITMAP m_sprite;
		HBITMAP m_mask;
		COORD m_pos;
		COORD m_velocity;
		COORD m_grabPoint;
		SIZE m_size;
		std::chrono::time_point<std::chrono::system_clock> m_timeChecked;

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


			m_timeChecked = std::chrono::system_clock::now();
			m_velocity = { 0, 10 };
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
		COORD GetPos() const noexcept
		{
			return m_pos;
		}
		COORD GetVelocity() const noexcept
		{
			return m_velocity;
		}
		COORD GetGrabPoint() const noexcept
		{
			return m_grabPoint;
		}
		HBITMAP GetSprite() const noexcept
		{
			return m_sprite;
		}
		HBITMAP GetMask() const noexcept
		{
			return m_mask;
		}
		SIZE GetSize() const noexcept
		{
			return m_size;
		}
		INT GetTimeDelta() const noexcept
		{
			 std::chrono::duration<double> delta = std::chrono::system_clock::now() - m_timeChecked;
			 return (INT)(delta.count() * 100);
		}
		void MoveTo(COORD toPos)
		{
			m_pos.X = toPos.X;// -m_grabPoint.X;
			m_pos.Y = toPos.Y;// -m_grabPoint.Y;
		}
		void UpdateTime()
		{
			m_timeChecked = std::chrono::system_clock::now();
		}
	};
	namespace
	{
		void ComputatePos(Object* obj)// TODO: make this work better
		{
			INT t = obj->GetTimeDelta();
			if (t < 50)
				return;
			INT v = obj->GetVelocity().Y;
			COORD pos = obj->GetPos();
			pos.Y += (v * t + 50) / 100;
			obj->MoveTo(pos);
			obj->UpdateTime();
		}
		inline void MaskBitBlt(HDC *destDC, HDC* srcDC, HDC* maskDC, INT destX, INT destY, INT destWidth, INT destHeight, INT srcOffsetX, INT srcOffsetY, INT maskOffsetX, INT maskOffsetY)
		{
			BitBlt(*destDC,
				destX, destY,
				destWidth, destHeight,
				*srcDC, srcOffsetX, srcOffsetY, SRCINVERT);
			BitBlt(*destDC,
				destX, destY,
				destWidth, destHeight,
				*maskDC, maskOffsetX, maskOffsetY, SRCAND);
			BitBlt(*destDC,
				destX, destY,
				destWidth, destHeight,
				*srcDC, srcOffsetX, srcOffsetY, SRCINVERT);
		}
		void Render(HDC* hdc, Object **objects, SHORT count)
		{
			HDC memDC = CreateCompatibleDC(*hdc);
			HDC maskDC = CreateCompatibleDC(*hdc);
			HGDIOBJ oldBmp = nullptr;
			HGDIOBJ oldMask = nullptr;
			for (SHORT i = 0; i < count; ++i)
			{
				oldBmp = SelectObject(memDC, objects[i]->GetSprite());
				oldMask = SelectObject(maskDC, objects[i]->GetMask());
				/* Do drawing with mask */
				MaskBitBlt(hdc, &memDC, &maskDC, objects[i]->GetPos().X, objects[i]->GetPos().Y,
					objects[i]->GetSize().width, objects[i]->GetSize().height,
					0, 0,
					0, 0);
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