#ifndef _PPTSHELLLISTNODE_H_
#define _PPTSHELLLISTNODE_H_


#include <math.h>
#include "DUICommon.h"

namespace DuiLib
{

	struct NodeData
	{
		int level_;
		bool folder_;
		bool child_visible_;
		bool has_child_;
		CDuiString text_;
		CDuiString value;
		CListContainerElementUI* list_elment_;
	};

	double CalculateDelay(double state);

	class PPTShellListNode
	{
	public:
		PPTShellListNode();
		explicit PPTShellListNode(NodeData t);
		PPTShellListNode(NodeData t, PPTShellListNode* parent);
		~PPTShellListNode();
		NodeData& data();
		int num_children() const;
		PPTShellListNode* child(int i);
		PPTShellListNode* parent();
		bool folder() const;
		bool has_children() const;
		void add_child(PPTShellListNode* child);
		void remove_child(PPTShellListNode* child);
		PPTShellListNode* get_last_child();

	private:
		void set_parent(PPTShellListNode* parent);

	private:
		typedef std::vector <PPTShellListNode*>	Children;

		Children	children_;
		PPTShellListNode*		parent_;

		NodeData    data_;
	};

} // DuiLib

#endif // 