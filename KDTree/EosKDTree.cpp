#include "stdafx.h"
#include "EosKDTree.h"

EosKDTreeNode* EosKDTree::createTree(float left, float right, float top, float bottom, std::vector<EosMistLineSegment>& lines)
{
	clearTree();
	m_pRoot = new EosKDTreeNode(left, right, bottom, top);
	
	doCreateTree(m_pRoot, lines);
	return m_pRoot;
}

void EosKDTree::clearTree()
{
	clearNode(m_pRoot);
	m_pRoot = nullptr;

	return;
}

void EosKDTree::doCreateTree(EosKDTreeNode* node, std::vector<EosMistLineSegment>& lines)
{
	// 确定分割方向
	float minX = node->m_right;
	float minY = node->m_top;
	float maxX = node->m_left;
	float maxY = node->m_bottom;

	int lineCount = lines.size();
	if (lineCount <= 2)
	{
		for(int i=0; i<lineCount; i++)
		{
			node->m_lines.push_back(lines[i]);
		}

		return;
	}

	// generate points
	std::vector<PointInfo> points;
	for(int i = 0; i < lineCount; i++)
	{
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

	// confirm slip direction
	int count = points.size();
	for (int i = 0; i < count; i++) {
		PointInfo point = points[i];
		if (point.m_pos.x < minX)
			minX = point.m_pos.x;
		if (point.m_pos.x > maxX)
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

	// get mid point
	int index = findMidPoint(points, dir);
	float splitValue = 0;
	if (dir == E_KDTreeDirection::eVertical)
		splitValue = points[index].m_pos.x;
	else
		splitValue = points[index].m_pos.y;

	points.clear();

	spliteAndCreateChildren(node, splitValue, dir, lines);
}

void EosKDTree::clearNode(EosKDTreeNode* node)
{
	if (node == nullptr)
		return;

	if(node->m_pLeftChild != nullptr)
	{
		clearNode(node->m_pLeftChild);
		node->m_pLeftChild = nullptr;
	}

	if(node->m_pRightChild != nullptr)
	{
		clearNode(node->m_pRightChild);
		node->m_pRightChild = nullptr;
	}

	delete node;
}

// jojohello log
//int partitionCount = 0;
int EosKDTree::findMidPoint(std::vector<PointInfo>& points, E_KDTreeDirection dir)
{
	// jojohello log
	//partitionCount = 0;

	return doFindMidPoint(0, points.size() - 1, points, dir);
}


int EosKDTree::doFindMidPoint(int l, int h, std::vector<PointInfo>& points, E_KDTreeDirection dir)
{
	int newL = partition(l, h, points, dir);
	// jojohello log
	//++partitionCount;
	//printf("---------- partition %d ------------------ \n", partitionCount);
	//printf("newL = %d\n", newL);
	//int count = points.size();
	//for (int i = 0; i < count; i++) {
	//	printf("point%d: (%d, %d)\n", i + 1, (int)(points[i].m_pos.x), (int)(points[i].m_pos.y));
	//}

	int midIndex = floor(points.size() * 0.5);
	if (newL == midIndex || newL == midIndex - 1)
		return newL;

	if (newL > midIndex)
		return doFindMidPoint(l, newL, points, dir);
	else
		return doFindMidPoint(newL+1, h, points, dir);
}

int EosKDTree::partition(int l, int h, std::vector<PointInfo>& points, E_KDTreeDirection dir)
{
	if(dir != E_KDTreeDirection::eHorizontal &&
		dir != E_KDTreeDirection::eVertical)
	{
		return -1;
	}

	float v1, v2;
	while(l < h)
	{
		for(int i=h; i>l; --i)
		{
			if(dir == E_KDTreeDirection::eHorizontal)
			{
				v1 = points[l].m_pos.y;
				v2 = points[i].m_pos.y;
			}else
			{
				v1 = points[l].m_pos.x;
				v2 = points[i].m_pos.x;
			}
			
			if(v1 > v2)
			{
				// wrap
				PointInfo temp = points[i];
				points[i] = points[l];
				points[l] = temp;
				break;
			}

			--h;
		}

		for(int i=l; i<h; ++i)
		{
			
			if (dir == E_KDTreeDirection::eHorizontal) {
				v1 = points[h].m_pos.y;
				v2 = points[i].m_pos.y;
			} else {
				v1 = points[h].m_pos.x;
				v2 = points[i].m_pos.x;
			}

			if (v1 < v2) {
				// wrap
				PointInfo temp = points[i];
				points[i] = points[h];
				points[h] = temp;
				break;
			}

			++l;
		}
	}

	return l;
};

void EosKDTree::spliteAndCreateChildren(EosKDTreeNode* parent, float splitValue, E_KDTreeDirection dir, std::vector<EosMistLineSegment>& lines)
{
	if (dir != E_KDTreeDirection::eHorizontal
		&& dir != E_KDTreeDirection::eVertical)
		return;

	// create then childeren node and set the rect
	EosKDTreeNode* pLeftNode = nullptr;
	EosKDTreeNode* pRightNode = nullptr;
	if(dir == E_KDTreeDirection::eHorizontal)
	{
		// left node is to bottom side
		pLeftNode = new EosKDTreeNode(parent->m_left, parent->m_right, parent->m_bottom, splitValue);

		// right node is to up side
		pRightNode = new EosKDTreeNode(parent->m_left, parent->m_right, splitValue, parent->m_top);
	}else
	{
		// left node is to left side
		pLeftNode = new EosKDTreeNode(parent->m_left, splitValue, parent->m_bottom, parent->m_top);
		// right node is to right side
		pRightNode = new EosKDTreeNode(splitValue, parent->m_right, parent->m_bottom, splitValue);
	}

	// caculate and split then lines into the tow children node's rect
	std::vector<EosMistLineSegment> leftLines;
	std::vector<EosMistLineSegment> rightLines;
	float compareValue1, compareValue2;
	int nCount = lines.size();
	for(int i=0; i<nCount; i++)
	{
		if(dir == E_KDTreeDirection::eHorizontal)
		{
			compareValue1 = lines[i].m_startPoint.y;
			compareValue2 = lines[i].m_endPoint.y;
		}else
		{
			compareValue1 = lines[i].m_startPoint.x;
			compareValue2 = lines[i].m_endPoint.x;
		}

		// s1: in the left node rect
		if (compareValue1 <= splitValue && compareValue2 <= splitValue) {
			leftLines.push_back(lines[i]);

		}else if (compareValue1 >= splitValue && compareValue2 >= splitValue) // s2: in the right node rect
		{
			rightLines.push_back(lines[i]);
		}else // s3: cross the two rect, and then be splited into tow line segment
		{
			EosMistPoint leftStartPoint(0, 0);
			EosMistPoint rightStartPoint(0, 0);
			EosMistPoint midPoint(0, 0);

			if(compareValue1 >= splitValue)
			{
				rightStartPoint = lines[i].m_startPoint;
				leftStartPoint = lines[i].m_endPoint;
			}else
			{
				leftStartPoint = lines[i].m_startPoint;
				rightStartPoint = lines[i].m_endPoint;
			}

			if(dir == eVertical)
			{
				float startX = lines[i].m_startPoint.x;
				float startY = lines[i].m_startPoint.y;
				float endX = lines[i].m_endPoint.x;
				float endY = lines[i].m_endPoint.y;
				float midY = startY + (splitValue - startX) * (endY - startY) / (endX - startX);
			
				midPoint.x = splitValue;
				midPoint.y = midY;
			}else
			{
				float startX = lines[i].m_startPoint.x;
				float startY = lines[i].m_startPoint.y;
				float endX = lines[i].m_endPoint.x;
				float endY = lines[i].m_endPoint.y;
				float midX = startX + (splitValue - startY) * (endX- startX) / (endY - startY);
			}

			EosMistLineSegment leftLine(leftStartPoint, midPoint);
			EosMistLineSegment rightLine(rightStartPoint, midPoint);
			leftLines.push_back(leftLine);
			rightLines.push_back(rightLine);
		}

		parent->m_pLeftChild = pLeftNode;
		parent->m_pRightChild = pRightNode;

		doCreateTree(parent->m_pLeftChild, leftLines);
		doCreateTree(parent->m_pRightChild, rightLines);
	}
}