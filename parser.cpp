#include "parser.h"

namespace TDatastream {
	///////////////////////////////
	// simple-lexical-analyzer
	TToken TLexer::GetNextTok() {
		if (BackP) {
			BackP = false;
			return LastTok;
		}
		CurrCh = SIn->GetCh();
		// skip whitespace 
		EatWs();
		// ignore comments 
		while (CurrCh == '#') { SkipLn(); EatWs(); }

		switch (CurrCh) {
		case '(':
			LastTok = TToken("(", TTokType::LPARENTHESIS, LineN);
			return LastTok;
		case ')':
			LastTok = TToken(")", TTokType::RPARENTHESIS, LineN);
			return LastTok;
		case ':':
			LastTok = TToken(":", TTokType::COLON, LineN);
			return LastTok;
		case ';':
			LastTok = TToken(";", TTokType::SEMIC, LineN);
			return LastTok;
		case ',':
			LastTok = TToken(",", TTokType::COMMA, LineN);
			return LastTok;
		case '=':
			LastTok = TToken("=", TTokType::EQU, LineN);
			return LastTok;
		}
		// identifier?
		if (IsValid(CurrCh)) {
			TChA ChA;
			ChA.AddCh(CurrCh);
			while (!SIn->Eof() && IsValid(SIn->PeekCh())) {
				CurrCh = SIn->GetCh();
				ChA.AddCh(CurrCh);
			}
			if (ChA == "dataFormat") {
				LastTok = TToken(ChA, TTokType::DFORMAT, LineN);
			} else if (ChA == "discrete") {
				LastTok = TToken(ChA, TTokType::DISCRETE, LineN);
			} else if (ChA == "numeric") {
				LastTok = TToken(ChA, TTokType::NUMERIC, LineN);
			} else {
				LastTok = TToken(ChA, TTokType::ID, LineN);
			}
		} else {
			if (!SIn->Eof()) {
				printf("[Line %d] Illegal character: '%c'.\n", LineN, CurrCh);
				Fail;
			}
			LastTok = TToken("", TTokType::END, LineN); // end-of-file
		}
		return LastTok;
	}

	void TLexer::EatWs() {
		while (!SIn->Eof() && TCh::IsWs(CurrCh)) {
			if (CurrCh == '\n') ++LineN;
			CurrCh = SIn->GetCh();
		}
	}

	void TLexer::SkipLn() {
		while (!SIn->Eof() && CurrCh != '\n') { CurrCh = SIn->GetCh(); }
	}
	///////////////////////////////
	// parameters
	void TParser::CfgParse(const TStr& FileNm) {
		TLexer Lexer(FileNm);
		TToken Tok;
		//while ((Tok = Lexer.GetNextTok()).Type != TTokType::END) {
		//	printf("\tToken: %s\n", Tok.Val.CStr());
		//}
		printf("--------- Parsing format specification ---------\n");
		InitLine(Lexer);
		printf("--------- Parsing attributes ---------\n");
		AttrLine(Lexer);
		printf("Parsing succeeded!\n\n"); 
	}

	void TParser::InitLine(TLexer& Lexer) {
		TToken Tok;
		Tok = Lexer.GetNextTok();
		if (Tok.Type != TTokType::DFORMAT) {
			printf("[Line %d] Expected 'dataFormat' keyword instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
			Fail;
		}
		// =
		Tok = Lexer.GetNextTok();
		if (Tok.Type != TTokType::COLON) {
			printf("[Line %d] Expected ':' instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
			Fail;
		}
		// (
		Tok = Lexer.GetNextTok();
		if (Tok.Type != TTokType::LPARENTHESIS) {
			printf("[Line %d] Expected '(' instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
			Fail;
		}
		// parameter list 
		InitParam(Lexer);
		// )
		Tok = Lexer.GetNextTok();
		if (Tok.Type != TTokType::RPARENTHESIS) {
			printf("[Line %d] Expected ')' instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
			Fail;
		}
	}

	void TParser::InitParam(TLexer& Lexer) {
		TToken Tok;
		int IdxN = 0;
		while (true) {
			// id
			Tok = Lexer.GetNextTok();
			if (Tok.Type != TTokType::ID) {
				printf("[Line %d] Expected identifier instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
				Fail;
			}
			printf("Declaring '%s'...\n", Tok.Val.CStr());
			DataFormatH.AddDat(Tok.Val, IdxN);
			InvDataFormatH.AddDat(IdxN, Tok.Val);
			// ,
			Tok = Lexer.GetNextTok();
			if (Tok.Type == TTokType::RPARENTHESIS) { break; } // end of parameter list 
			if (Tok.Type != TTokType::COMMA) {
				printf("[Line %d] Expected ',' instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
				Fail;
			}
			++IdxN;
		}
		++IdxN;
		AttrsHV.Reserve(IdxN, IdxN);
		InvAttrsHV.Reserve(IdxN, IdxN);
		Lexer.PutBack();
	}

	void TParser::AttrLine(TLexer& Lexer) {
		TToken Tok;
		TStr AttrNm;
		while (true) {
			// id
			Tok = Lexer.GetNextTok();
			if (Tok.Type == TTokType::END) { break; } // end-of-file
				
			if (Tok.Type != TTokType::ID) {
				printf("[Line %d] Expected identifier instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
				Fail;
			}
			AttrNm = Tok.Val;
			// make sure attribute was ``declared'' in dataFormat statement 
			if (!DataFormatH.IsKey(AttrNm)) {
				printf("Attribute '%s' is undeclared.\n", AttrNm.CStr());
				Fail;
			}
			// :
			Tok = Lexer.GetNextTok();
			if (Tok.Type != TTokType::COLON) {
				printf("[Line %d] Expected ':' instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
				Fail;
			}
			// discrete/numeric
			Tok = Lexer.GetNextTok();
			if (Tok.Type == TTokType::DISCRETE) {
				printf("Nominal attirubte %s\n", AttrNm.CStr());
				// (
				Tok = Lexer.GetNextTok();
				if (Tok.Type != TTokType::LPARENTHESIS) {
					printf("[Line %d] Expected '(' instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
					Fail;
				}
				AttrParam(Lexer, AttrNm);
				// )
				Tok = Lexer.GetNextTok();
				if (Tok.Type != TTokType::RPARENTHESIS) {
					printf("[Line %d] Expected ')' instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
					Fail;
				}
			} else if (Tok.Type == TTokType::NUMERIC) {
				printf("Numeric atribute\n");
				const int CountN = DataFormatH.GetDat(AttrNm);
				AttrsHV.GetVal(CountN).AddDat("", 0);
				InvAttrsHV.GetVal(CountN).AddDat(0, "");
			} else {
				printf("[Line %d] Expected attribute (discrete or numeric) type instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
				Fail;
			}
		}
	}

	void TParser::AttrParam(TLexer& Lexer, const TStr& AttrNm) {
		int IdxN = 0;
		const int CountN = DataFormatH.GetDat(AttrNm);
		TToken Tok;
		TStr ValNm;
		while (true) { // loop through all values 
			// id
			Tok = Lexer.GetNextTok();
			if (Tok.Type != TTokType::ID) {
				printf("[Line %d] Expected identifier instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
				Fail;
			}
			ValNm = Tok.Val;
			// set up the mappings 
			AttrsHV.GetVal(CountN).AddDat(ValNm, IdxN);
			InvAttrsHV.GetVal(CountN).AddDat(IdxN, ValNm);
			// ,
			Tok = Lexer.GetNextTok();
			if (Tok.Type == TTokType::RPARENTHESIS) { break; } // end of parameter list 
			if (Tok.Type != TTokType::COMMA) {
				printf("[Line %d] Expected ',' instead of '%s'.\n", Tok.LineN, Tok.Val.CStr());
				Fail;
			}
			++IdxN;
		}
		Lexer.PutBack();
	}

	void TParser::Error(const TStr& Msg) {
		throw TExcept::New("Parsing error.");
	}
} // namespace TDatastream
