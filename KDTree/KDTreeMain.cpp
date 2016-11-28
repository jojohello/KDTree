// KDTree.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <cstdlib>
#include <vector>
#include "EosKDTree.h"

std::vector<EosMistLineSegment> createLines(float l, float r, float b, float t, int count);
void testFindMidPoint(float l, float r, float b, float t, std::vector<EosMistLineSegment>& lines);
void testBuildTree(float l, float r, float b, float t, std::vector<EosMistLineSegment>& lines);

int main()
{
	int lineCount = 5;
	float l = 0;
	float r = 100;
	float b = 0;
	float t = 100;
	std::vector<EosMistLineSegment> lines = createLines(l, r, b, t, lineCount);
	/*for(int i=0; i<lineCount; i++)
	{
		printf("line%d: (%f, %f), (%f, %f)\n", i + 1, lines[i].m_startPoint.x, lines[i].m_startPoint.y, lines[i].m_endPoint.x, lines[i].m_endPoint.y);
	}*/

	//testFindMidPoint(l, r, b, t, lines);

	testBuildTree(l, r, b, t, lines);

	printf("press any key to exit");
	_getch();

    return 0;
}

std::vector<EosMistLineSegment> createLines(float l, float r, float b, float t, int count)
{
	std::vector<EosMistLineSegment> ret;
	int minX = floor(l);
	int maxX = floor(r);
	int minY = floor(b);
	int maxY = floor(t);

	for(int i=0; i<count; i++)
	{
		EosMistLineSegment newLine(
			minX + rand() % (maxX - minX),
			minY + rand() % (maxY - minY),
			minX + rand() % (maxX - minX),
			minY + rand() % (maxY - minY)
		);

		ret.push_back(newLine);
	}

	return ret;
}

void testFindMidPoint(float l, float r, float b, float t, std::vector<EosMistLineSegment>& lines)
{
	EosKDTree* tree = new EosKDTree();

	// 赋值先反过来，一遍下面遍历计算的时候用．
	float minX = r;
	float minY = t;
	float maxX = l;
	float maxY = b;

	int lineCount = lines.size();

	std::vector<PointInfo> points;
	for (int i = 0; i < lineCount; i++) {
		EosMistLineSegment line = lines[i];
		PointInfo p1, p2;
		p1.m_pos.x = line.m_startPoint.x;
		p1.m_pos.y = line.m_startPoint.y;
		p1.m_lineSegementIndex = i;
		p1.m_pointIndex = 1;

		p2.m_pos.x = line.m_endPoint.x;
		p2.m_pos.y = line.m_endPoint.y;
		p2.m_lineSegementIndex = i;
		p2.m_pointIndex = 2;

		points.push_back(p1);
		points.push_back(p2);
	}

	// confirm clipt direction
	int count = points.size();
	for (int i = 0; i < count; i++) {
		PointInfo point = points[i];
		if (point.m_pos.x < minX)
			minX = point.m_pos.x;
			maxX = point.m_pos.x;
		if (point.m_pos.y < minY)
			minY = point.m_pos.y;
		if (point.m_pos.y > maxY)
			maxY = point.m_pos.y;
	}
	
	E_KDTreeDirection dir = E_KDTreeDirection::eHorizontal;
	if (abs(maxX - minX) > abs(maxY - minY)) {
		dir = E_KDTreeDirection::eVertical;
	}
	
	printf("==========================================================\n");
	for (int i = 0; i < count; i++) {
		printf("point%d: (%d, %d)\n", i + 1, (int)(points[i].m_pos.x), (int)(points[i].m_pos.y));
	}
	printf("==========================================================\n");

	printf("------------- confirm slice direction ---------------\n");
	printf("minX = %f, maxX = %f, minY = %f, maxY = %f\n", minX, maxX, minY, maxY);
	if (dir == E_KDTreeDirection::eHorizontal)
		printf("dir = Horizontal \n");
	else
		printf("dir = eVertical \n");

	// get mid point
	int index = tree->findMidPoint(points, dir);
	if (dir == E_KDTreeDirection::eHorizontal)
		printf("split on Horizontal..... midPoint = (%d, %d)-------------\n", (int)(points[index].m_pos.x), (int)(points[index].m_pos.y));
	else
		printf("split on Vertical..... midPoint = (%d, %d)--------------\n", (int)(points[index].m_pos.x), (int)(points[index].m_pos.y));
	for (int i = 0; i < count; i++) {
		printf("point%d: (%d, %d)\n", i + 1, (int)(points[i].m_pos.x), (int)(points[i].m_pos.y));
	}
}

void testBuildTree(float l, float r, float b, float t, std::vector<EosMistLineSegment>& lines)
{
	EosKDTree* tree = new EosKDTree();
	tree->createTree(l, r, t, b, lines);
}

