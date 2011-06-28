#ifndef	COLOUR_ADD_ON_MANAGER_H
#define COLOUR_ADD_ON_MANAGER_H


#include <list>
#include "AddOn.h"
#include "CommonTypes.h"
#include "ColourAddOnData.h"
#include "LayersRenderingInfo.h"

namespace iviews {
	
	struct AddOnApplication {
		AddOn				* addOn;
		ColourAddOnData		* data;
		ColourAddOnUndoData * undoData;

		~AddOnApplication ();
		AddOnApplication (
			AddOn				* addOn_, 
			ColourAddOnData		* data_		= (ColourAddOnData *)0, 
			ColourAddOnUndoData * undoData_ = (ColourAddOnUndoData *)0
		);

		bool operator== (const AddOn * addOn) const;
		bool operator!= (const AddOn * addOn) const;

		bool operator== (const AddOnApplication & a) const;
		bool operator!= (const AddOnApplication & a) const;

		friend bool operator== (const AddOnApplication * app, const AddOn & addOn);
		friend bool operator== (const AddOnApplication * app1, const AddOnApplication & app2);
	};

	extern bool operator== (const AddOnApplication * app, const AddOn & addOn);
	extern bool operator== (const AddOnApplication * app1, const AddOnApplication & app2);


	class ColourAddOnManager {
	public:

		//data must be an heap Object. 
		//Responsible for delete is ColourAddOnManager
		//Return true if must be hapend a refresh action otherwise false
		bool AddOrRemoveConditionalAddOn (
				AddOn				* addOn, 
				ColourAddOnData		* data,
				LayersRenderingInfo * layers
			);

		void Apply (LayersRenderingInfo * layers); 
		void Exclude (LayersRenderingInfo * layers);
		void RestoreColours (LayersRenderingInfo * layers);
		bool RemoveAll (const AddOn * value, LayersRenderingInfo * layers);
		bool RemoveFirst (const AddOn * value, LayersRenderingInfo * layers);

		bool Ebable (void);
		bool Disable (void);



	private:
		typedef std::list<AddOnApplication *> AddOnApplicationPtrList;
		
		void Clear (void);

		void InsertAndApplyAddOn (
				AddOn				* addOn, 
				ColourAddOnData		* data,
				LayersRenderingInfo * layers
			);


		void Apply (
				AddOnApplicationPtrList::iterator begin, 
				AddOnApplicationPtrList::iterator end,
				LayersRenderingInfo * layers
			);


		template<class Fun>
		AddOnApplicationPtrList::iterator Undo (LayersRenderingInfo * layers, Fun breakCondition) {
			
			AddOnApplicationPtrList::reverse_iterator i;
			for (i = addOns.rbegin(); i != addOns.rend(); ++i) {
				if ((*i)->undoData)
					(*i)->addOn->Undo(layers, (*i)->undoData);
				if (breakCondition(*i)) {
						//.base pointing to the element next to the one 
						//the reverse_iterator is currently pointing to.
						//For this reason we increase i
						++i;
						break;
				}				
			}
			return i.base();
		}


		

		AddOnApplicationPtrList addOns;

	};





}	//namespace iviews

#endif	//COLOUR_ADD_ON_MANAGER_H