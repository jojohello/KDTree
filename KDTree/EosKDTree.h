#pragma once
#include <vector>
#include "EosMistLineSegment.h"
#include "math/EosVector2.h"

class EosKDTreeNode;
class EosKDTreeNodeData;

enum E_KDTreeDirection
{
	eNone =0,			// 并没有切割.
	eHorizontal,		// 水平切割.
	eVertical,			// 垂直切割.
};

struct PointInfo
{
public:
	Vector2 m_pos;
	int m_lineSegementIndex;
	int m_pointIndex;
};

class EosKDTree
{
public:
	EosKDTreeNode* CreateTree(float left, float right, float top, float bottom, std::vector<EosMistLineSegment>& lines);
	void clearTree();

	//temp
	int findMidPoint(std::vector<PointInfo>& points, E_KDTreeDirection dir);
private:
	void doCreateTree(EosKDTreeNode* node, std::vector<EosMistLineSegment>& lines);
	void spliteAndCreateChildren(EosKDTreeNode* parent, float splitValue, E_KDTreeDirection dir, std::vector<EosMistLineSegment>& lines);
	void clearNode(EosKDTreeNode* node);
	
	int doFindMidPoint(int l, int h, std::vector<PointInfo>& points, E_KDTreeDirection dir);
	int partition(int l, int h, std::vector<PointInfo>& points, E_KDTreeDirection dir);
private:
	EosKDTreeNode* m_pRoot;
};

class EosKDTreeNode
{
public:
	EosKDTreeNode(float l, float r, float b, float t)
	{
		m_left = l;
		m_right = r;
		m_bottom = b;
		m_top = t;

		m_eDir = E_KDTreeDirection::eNone;
	}
	
	void setClipInfo(float x, float y, E_KDTreeDirection dir)
	{
		m_x = x;
		m_y = y;
		m_eDir = dir;
	}

	/*void setData(EosKDTreeNodeData* p)
	{
		if(m_pData != nullptr)
		{
			delete m_pData;
			m_pData = nullptr;
		}

		m_pData = p;
	}*/
public:
	float m_left;
	float m_right;
	float m_top;
	float m_bottom;

	float m_x;
	float m_y;

	E_KDTreeDirection m_eDir;
	//EosKDTreeNodeData* m_pData;
	EosKDTreeNode*  m_pLeftChild;
	EosKDTreeNode*	m_pRightChild;
	std::vector<EosMistLineSegment> m_lines;
};

// 父类，继承后可以写入各种方便计算用的数据
//class EosKDTreeNodeData
//{
//public:
//	EosKDTreeNodeData(){};
//};
