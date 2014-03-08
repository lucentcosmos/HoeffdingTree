#ifndef WINNOW_H
#define WINNOW_H

#include <base.h>
#include <mine.h>

namespace TDatastream {
	///////////////////////////////
	// Winnow
	ClassTP(TWinnow, PWinnow) // {
	private:
		TWinnow(const int& FeaturesN, const double& Treshold, const double& Alpha)
			: Treshold(Treshold), Alpha(Alpha) { WeightV.Reserve(FeaturesN, FeaturesN); }
	public:
		static PWinnow New(const int& FeaturesN, const double& Treshold, const double& Alpha = 2.0) {
			return new TWinnow(FeaturesN, Treshold, Alpha);
		}
		static PWinnow New(const int& FeaturesN, const double& Alpha = 2.0) {
			return new TWinnow(FeaturesN, FeaturesN/2.0, Alpha);
		}
		void Process(const TIntV& Example);
		int Classify(const TIntV& Example);
		void SetWeighs(TFltV& Model) const {
			Model = WeightV;
		}
	private:
		static double Dot(const TFltV& WeightV, const TIntV& BoolV);
	private:
		TFltV WeightV;
		double Treshold;
		double Alpha;
	};

} // namespace TDatastream

#endif
