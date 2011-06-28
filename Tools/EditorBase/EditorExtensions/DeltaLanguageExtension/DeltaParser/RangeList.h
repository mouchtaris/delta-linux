/**
 *	RangeList.h
 *
 *	Giannis Georgalis <jgeorgal@ics.forth.gr>
 *	November 2006
 */
#ifndef DELTA_RANGELIST_H
#define DELTA_RANGELIST_H

#include "Common.h"

#include <list>
#include <boost/functional.hpp>

namespace delta {

////////////////////////////////////////////////////////////////////////

template<class TRange>
class RangeListT : protected std::list<TRange> {
public:
	typedef std::list<TRange> BaseType;

	typedef BaseType::iterator					iterator;
	typedef BaseType::const_iterator			const_iterator;
	typedef BaseType::reverse_iterator			reverse_iterator;
	typedef BaseType::const_reverse_iterator	const_reverse_iterator;

	RangeListT (void) {}
	~RangeListT (void) {}

	//******************************************************************
	// Exported list interface
	//
	iterator begin (void) { return this->BaseType::begin(); }
	iterator end (void) { return this->BaseType::end(); }

	const_iterator begin (void) const { return this->BaseType::begin(); }
	const_iterator end (void) const { return this->BaseType::end(); }

	reverse_iterator rbegin (void) { return this->BaseType::rbegin(); }
	reverse_iterator rend (void) { return this->BaseType::rend(); }

	const_reverse_iterator rbegin (void) const { return this->BaseType::rbegin(); }
	const_reverse_iterator rend (void) const { return this->BaseType::rend(); }

	iterator erase (iterator iter) { return this->BaseType::erase(iter); }

	//******************************************************************

	void AddRange (const TRange& chunk);

	//******************************************************************

	void Swap (RangeListT& rl) { this->BaseType::swap(rl); }

	void Merge (RangeListT& rl)
	{
		std::for_each(rl.begin(), rl.end(),
			boost::bind1st(boost::mem_fn(&RangeListT::AddRange), this));
		rl.clear();
	}

	void Clear (void) { this->BaseType::clear(); }
	bool Empty (void) const { return this->BaseType::empty(); }

	//******************************************************************

	void UpdatePositionsWithoutRemove (uint atPos, int offset)
	{
		for (iterator i = this->begin(); i != this->end(); ++i)
			i->Update(atPos, offset);
	}

	//******************************************************************

	void UpdatePositions (uint atPos, int offset)
	{
		bool changedBoundaries = false;
		for (iterator i = this->begin(); i != this->end();) {
			i->Update(atPos, offset);
			if (i->Empty()) {
				i = this->erase(i);
				changedBoundaries = true;
			}
			else
				++i;
		}

		if (changedBoundaries) {
			// TODO: insert code here to merge fwd
		}
	}

	//******************************************************************

	const TRange& GetRange (uint atPos) const
	{
		static const TRange nullRange;

		for (const_iterator i = this->begin(); i != this->end(); ++i) {
			if (i->Contains(atPos))
				return *i;
			else if (i->IsRightOf(atPos))
				break;
		}
		return nullRange;
	}

	//******************************************************************

	template<class TRange>
	const TRange RemoveElementContainingRange (const TRange& range)
	{
		for (iterator i = this->begin(); i != this->end(); ++i) {
			if (i->Contains(range)) {
				const TRange result = *i;
				this->erase(i);
				return result;
			}
			else if (i->IsRightOf(range))
				break;
		}
		return TRange();
	}

	//******************************************************************

	template<class TRange>
	const TRange RemoveElementsInRange (const TRange& range)
	{
		TRange result;
		iterator i = this->begin();
		while (i != this->end()) {
			if (range.Contains(*i)) {

				if (!result.right)	// Means result.Empty() == true (in this case)
					result.left = i->left;
				result.right = i->right;
				i = this->erase(i);
			}
			else if (i->IsRightOf(range))
				break;
			else
				++i;
		}
		return result;
	}

	//******************************************************************

	template<class TRange>
	const TRange RemoveWholeRange (void)
	{
		if (this->Empty())
			return TRange();

		TRange result;
		result.left = this->front().left;
		result.right = this->back().right;
		this->Clear();
		return result;
	}

private:
	inline void merge_forward (iterator iter);
};

////////////////////////////////////////////////////////////////////////
// Implementation
//
template<class TRange>
void RangeListT<TRange>::AddRange (const TRange& chunk)
{
	if (this->empty() || chunk.left > this->back().right + 1)
		this->push_back(chunk);
	else if (chunk.left == this->back().right + 1)
		this->back().right = chunk.right;
	else {
		for (iterator i = this->begin(); i != this->end(); ++i) {
			if (i->Contains(chunk.left)) {
				if (chunk.right > i->right) {
					i->right = chunk.right;
					merge_forward(i);
				}
				return;
			}
			else if (i->right >= chunk.left && i->left <= chunk.right + 1) {
				i->left = chunk.left;
				if (chunk.right > i->right) {
					i->right = chunk.right;
					merge_forward(i);
				}
				return;
			}
			else if (i->left > chunk.left) {
				this->insert(i, chunk);
				return;
			}
		}
	}
}

//**********************************************************************

template<class TRange>
void RangeListT<TRange>::merge_forward (iterator iter)
{
	assert(iter != this->end());
	iterator fIter = iter;
	for (++fIter; fIter != this->end();) {
		if (fIter->left <= iter->right + 1) {
			iter->right = std::max(fIter->right, iter->right);
			fIter = this->erase(fIter);
		}
		else
			break;
	}
}

////////////////////////////////////////////////////////////////////////

} // namespace delta

#endif // DELTA_RANGELIST_H
