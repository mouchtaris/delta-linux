/**
 *	EdgesParameters.h
 *	
 *	Koutsopoulos Nikolaos <koutsop@ics.forth.gr	>
 *	June 2010
 */

#ifndef EDGES_PARAMETERS_H
#define EDGES_PARAMETERS_H


namespace iviews {

class EdgesParameters {
public:
	EdgesParameters (void);

	bool	HasHiddenAllEdges (void) const;
	void	HideAllEdges(bool hide); 

	bool	HasHiddenInnerEdges (void) const;
	void	HideInnerEdges(bool hide); 

	bool	HasHiddenIncomingEdges (void) const;
	void	HideIncomingEdges(bool hide); 

	bool	HasHiddenOutgoingEdges (void) const;
	void	HideOutgoingEdges(bool hide); 


private:
	bool	isAllEdgesHidden;
	bool	isInnerEdgesHidden;
	bool	isIncomingEdgesHidden;
	bool	isOutgoingEdgesHidden;
};

} //namespace iviews 

#endif	//EDGES_PARAMETERS_H