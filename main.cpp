#include "hoeffding.h"
#include "winnow.h"

using namespace TDatastream;

void ProcessData(const TStr& FileNm, PHoeffdingTree HoeffdingTree);
void WinnowTest(const TVec<TIntV>& ExamplesV);
void WinnowTest(const TStr& FileNm);

int main(int argc, char** argv) {
	try {
		// Create environment
		Env = TEnv(argc, argv, TNotify::StdNotify);
		// Pass some parameters 
		const bool ConceptDriftP = Env.IsArgStr("drift"); // use version that handles concept-drift? 
		const double SplitConfidence = Env.GetIfArgPrefixFlt("-splitConfidence:", 1e-6, "Split confidence"); // 1e-6 
		const double TieBreaking = Env.GetIfArgPrefixFlt("-tieBreaking:", 0.01, "Tie breaking"); // 1e-2 
		// const TStr ConfigFNm = Env.GetIfArgPrefixStr("-config:", "sea.config", "Config file");
		// const TStr ConfigFNm = Env.GetIfArgPrefixStr("-config:", "nyel-base-final.config", "Config file");
		// const TStr ConfigFNm = Env.GetIfArgPrefixStr("-config:", "nyel-numeric-final.config", "Config file"); // classification problem 
		// const TStr ConfigFNm = Env.GetIfArgPrefixStr("-config:", "nyel-numeric.config", "Config file"); // regression problem 
		// const TStr ConfigFNm = Env.GetIfArgPrefixStr("-config:", "housing.config", "Config file");
		const TStr ConfigFNm = Env.GetIfArgPrefixStr("-config:", "regression-test.config", "Config file");
		const TStr AttrHeuristic= Env.GetIfArgPrefixStr("-attrEval:", "InfoGain", "Attribute evaluation heuristic");
		const int GracePeriod = Env.GetIfArgPrefixInt("-gracePeriod:", 300, "Grace period"); // 3e2 
		const int DriftCheck = Env.GetIfArgPrefixInt("-driftCheck:", 10000, "Drift check"); // 1e4 
		const int WindowSize = Env.GetIfArgPrefixInt("-windowSize:", 50000, "Window size"); // 1e5 
		// const TStr ExportFNm = Env.GetIfArgPrefixStr("-export:", "titanic.xml", "Decision tree export file");
		
		EAssertR(SplitConfidence > 0, "Split confidence must be positive real number.");
		EAssertR(GracePeriod > 0, "Positive integer");
		EAssertR(DriftCheck > 0, "Positive integer.");
		EAssertR(WindowSize > 0, "Window size should be positive integer.");
		EAssertR(TieBreaking >= 0, "Tie breaking must be nonnegative.");

		// Reset error 
		// TStr FNm = ConceptDriftP ? "err-cvfdt.dat" : "err-vfdt.dat";
		// if (TFile::Exists(FNm)) { TFile::Del(FNm); }

		// usage example 
		PHoeffdingTree ht = THoeffdingTree::New("docs/" + ConfigFNm, GracePeriod, SplitConfidence, TieBreaking, DriftCheck, WindowSize);
		// ht->SetAdaptive(true); // ht->SetAdaptive(ConceptDriftP);
		TTmProfiler Prof;
		Prof.AddTimer("HoeffdingTree");
		Prof.StartTimer(0);
		ProcessData("data/regression-test.dat", ht);
		// ProcessData("data/sea.dat", ht);
		// ProcessData("data/nyel-base-final.dat", ht);
		// ProcessData("data/nyel-numeric-final.dat", ht);
		/// ProcessData("data/nyel-numeric-discreticized.dat", ht);
		// ProcessData("data/housing-100K.dat", ht);
		// ProcessData("data/dlbcl-1K.dat", ht); // process data set 
		/*
		int Label0 = ht->Classify("first,adult,female,no");
		int Label1 = ht->Classify("third,adult,female,no");
		int Label2 = ht->Classify("crew,adult,female,no");
		int Label3 = ht->Classify("crew,adult,male,no");
		int Label4 = ht->Classify("first,adult,male,no");
		int Label5 = ht->Classify("first,child,male,no");
		int Label6 = ht->Classify("second,child,male,no");
		int Label7 = ht->Classify("third,child,male,no");
		printf("Classified as %d and %d.\n", Label1, Label2);
		*/
		/*
		printf("Predict (T, F): %f\n", ht->Predict("t,f,0.0"));
		printf("Predict (T, T): %f\n", ht->Predict("t,t,0.0"));
		printf("Predict (F, F): %f\n", ht->Predict("f,f,0.0"));
		printf("Predict (F, T): %f\n", ht->Predict("f,t,0.0"));
		*/
		Prof.StopTimer(0);
		printf("Time = %f sec\n", Prof.GetTimerSec(0));
		printf("Time = %f min\n", Prof.GetTimerSec(0)/60.0);
		printf("Time = %f h\n", Prof.GetTimerSec(0)/(60.0*60.0));
		//ht->Export("exports/housing-100K.xml");
		ht->Export("exports/regression-test.gv", TExportType::DOT);
		// ht->PrintHist("exports/adult.hist");
		
		/*
		printf("-------- Winnow --------\n");
		WinnowTest("data/winnow.dat");
		*/
		getchar();
	} catch(PExcept Except) {
		// GLib exception 
		printf("%s\n", TStr("[Error] "+Except->GetMsgStr()).CStr()); getchar();
		return 2;
	} catch(...) {
		// other exceptions
		printf("[Error] Exception we cant really handle.\n"); getchar();
		return 1;
	}
	return 0;
}

// process data line-by-line 
void ProcessData(const TStr& FileNm, PHoeffdingTree HoeffdingTree) {
	Assert(TFile::Exists(FileNm));
	TFIn FIn(FileNm);
	TStr Line;
	while (FIn.GetNextLn(Line)) {
		HoeffdingTree->Process(Line, ',');
	}
}

void WinnowTest(const TVec<TIntV>& ExamplesV) {
	const int DimN = ExamplesV.Last().Len();
	printf("DimN = %d\n", DimN);
	PWinnow Winnow = TWinnow::New(DimN-1);
	for (TVec<TIntV>::TIter It = ExamplesV.BegI(); It != ExamplesV.EndI(); ++It) {
		Winnow->Process(*It);
	}
	printf("Classify last as %d.\n", Winnow->Classify(ExamplesV.Last()));
}

void WinnowTest(const TStr& FileNm) {
	TVec<TIntV> ExamplesV;
	TFIn FIn(FileNm);
	TStr Ln;
	TStrV LnV;
	while (!FIn.Eof()) {
		TIntV ExampleV;
		FIn.GetNextLn(Ln);
		Ln.SplitOnAllCh(',', LnV);
		for (TStrV::TIter It = LnV.BegI(); It != LnV.EndI(); ++It) {
			ExampleV.Add(It->GetInt());
		}
		ExamplesV.Add(ExampleV);
	}
	WinnowTest(ExamplesV);
}
