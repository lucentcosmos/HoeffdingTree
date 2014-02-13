#include "winnow.h"

namespace TDatastream {
	///////////////////////////////
	// Winnow
	void TWinnow::Process(const TIntV& Example) {
		const bool Label = Example.Last() == 1;
		const bool Pred = Dot(WeightV, Example) > Treshold;
		if (Label != Pred) {
			const double Factor = Label ? Alpha : 1/Alpha;
			for (int ElN = 0; ElN < WeightV.Len(); ++ElN) {
				if (Example[ElN]) WeightV[ElN] *= Factor;
			}
		}
	}
	int TWinnow::Classify(const TIntV& Example) {
		return Dot(WeightV, Example) > Treshold;
	}
	double TWinnow::Dot(const TFltV& WeightV, const TIntV& BoolV) {
		EAssert(WeightV.Len() == BoolV.Len()-1); // last BoolV is class label 
		double Val = 0;
		for (int ElN = 0; ElN < WeightV.Len(); ++ElN) {
			Val += WeightV[ElN]*BoolV[ElN];
		}
		return Val;
	}
} // namespace TDatastream
