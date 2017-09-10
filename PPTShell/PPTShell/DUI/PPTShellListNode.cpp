#include "stdafx.h"
#include "PPTShellListNode.h"

namespace DuiLib
{

	double CalculateDelay(double state)
	{
		return pow(state, 2);
	}

	void PPTShellListNode::set_parent(PPTShellListNode* parent)
	{
		parent_ = parent;
	}

	PPTShellListNode::PPTShellListNode()
		: parent_(NULL)
	{}

	PPTShellListNode::PPTShellListNode(NodeData t)
		: data_(t)
		, parent_(NULL)
	{}

	PPTShellListNode::PPTShellListNode(NodeData t, PPTShellListNode* parent)
		: data_ (t)
		, parent_ (parent)
	{}

	PPTShellListNode::~PPTShellListNode() 
	{
		for (int i = 0; i < num_children(); ++i)
			delete children_[i]; 
	}

	NodeData& PPTShellListNode::data()
	{
		return data_;
	}

	int PPTShellListNode::num_children() const
	{
		return static_cast<int>(children_.size());
	}

	PPTShellListNode* PPTShellListNode::child(int i)
	{
		return children_[i];
	}

	PPTShellListNode* PPTShellListNode::parent()
	{
		return ( parent_);
	}

	bool PPTShellListNode::has_children() const
	{
		return num_children() > 0;
	}

	bool PPTShellListNode::folder() const
	{
		return data_.folder_;
	}

	void PPTShellListNode::add_child(PPTShellListNode* child)
	{
		child->set_parent(this); 
		children_.push_back(child); 
	}


	void PPTShellListNode::remove_child(PPTShellListNode* child)
	{
		Children::iterator iter = children_.begin();
		for (; iter < children_.end(); ++iter)
		{
			if (*iter == child) 
			{
				children_.erase(iter);
				return;
			}
		}
	}

	PPTShellListNode* PPTShellListNode::get_last_child()
	{
		if (has_children())
		{
			return child(num_children() - 1)->get_last_child();
		}
		return this;
	}

}