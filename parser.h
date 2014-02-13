#ifndef PARSE_H
#define PARSE_H

#include<base.h>

namespace TDatastream {
	enum class TTokType : char { DFORMAT, DISCRETE, NUMERIC, COLON, COMMA, EQU, END, ID, LPARENTHESIS, RPARENTHESIS, SEMIC };

	struct TToken {
		TToken(const TStr& Val = "", const TTokType& Type = TTokType::END, const int& LineN = 0)
			: Val(Val), Type(Type), LineN(LineN) { }
		TStr Val;
		TTokType Type;
		int LineN;
	};

	// simple lexical analyzer
	class TLexer {
	public:
		TLexer(const TStr& FileNm)
			: LineN(1), BackP(false) { SIn = TFIn::New(FileNm); }
		TToken GetNextTok();
		inline void PutBack() { // need this for lookahead 
			BackP = true;
		}
		static bool IsValid(const char& Ch) {
			return TCh::IsAlNum(Ch) || (Ch == '_' || Ch == '-' || Ch == '&' || Ch == '<' || Ch == '>' || Ch == '=' || Ch == '.');
		}
	private:
		void EatWs(); // eats whitespace 
		void SkipLn(); // skip line 
		int CurrCh; // current character 
		int LineN; // current line 
		TToken LastTok;
		bool BackP;
		PSIn SIn;
	};

	// parameters class 
	class TParser {
	public:
		enum class TAttrHeuristic : char { INFO_GAIN, GINI };
		// uint GracePeriodN; // nmin parameter 
		// uint DriftCheckN; // f parameter 
		// uint WindowSizeN; // w parameter 
		// double SplitConfidence; // delta parameter 
		// double TieBreaking; // tau parameter 
		// TAttrHeuristic AttributeHeuristic; // information gain, gini index, etc. 
		TVec<THash<TStr, TInt> > AttrsHV;
		THash<TStr, TInt> DataFormatH;
		TVec<THash<TInt, TStr> > InvAttrsHV;
		THash<TInt, TStr> InvDataFormatH;
		// hack for handling numeric attributes 
		// THash<TStr, TInt> NumAttrsH;
		// THash<TInt, TStr> InvNumAttrsH;
	private:
		void CfgParse(const TStr& FileNm);
		void InitLine(TLexer& Lexer); // dataFormat = (a1, a2, ..., an)
		void InitParam(TLexer& Lexer); // parameter list, i.e., `a1, a2, ..., an'
		void AttrLine(TLexer& Lexer); // ai: discrete(vi1, vi2, ..., vini) or aj: numeric
		void AttrParam(TLexer& Lexer, const TStr& AttrNm); // value list, i.e., `vi1, vi2, ..., vini'
		void Error(const TStr& Msg); // TODO: make it StrFmt 
	public:
		TParser(const TStr& FNm) {
			CfgParse(FNm);
		}
	};
} // namespace TDatastream

#endif
