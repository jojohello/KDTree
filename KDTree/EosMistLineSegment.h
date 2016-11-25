#pragma once
#include <stdio.h>
#include <vector>

struct EosMistPoint
{
public:
	EosMistPoint(float inputX, float inputY)
	{
		x = inputX;
		y = inputY;
	}

	float x, y;
};

struct EosMistLineSegment
{
public:
	EosMistLineSegment(float startX, float startY, float endX, float endY)
	: m_startPoint(startX, startY),
		m_endPoint(endX, endY)
	{
		
	}

	EosMistLineSegment(EosMistPoint start, EosMistPoint end)
		: m_startPoint(start.x, start.y),
			m_endPoint(end.x, end.y)
	{

	}

	EosMistPoint m_startPoint;
	EosMistPoint m_endPoint;
};

class EosMistLineSegments
{
public:
	
	EosMistLineSegment* GetLineSegment(int index)
	{
		if(index >= m_lineSegments.size())
		{
			return nullptr;
		}

		return &m_lineSegments[index];
	}
private:
	std::vector<EosMistLineSegment> m_lineSegments;
};
