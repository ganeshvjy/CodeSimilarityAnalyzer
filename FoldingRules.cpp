/////////////////////////////////////////////////////////////////////
//  FoldingRules.h - declares ITokCollection folding rules         //
//  ver 1.1                                                        //
//  Language:      Visual C++ 2010, SP1                            //
//  Platform:      Dell Precision T7400, Win 7 Pro SP1             //
//  Application:   Code Analysis Research                          //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////

#include "FoldingRules.h"

void codeFoldingRules::MergeForCollections(ITokCollection*& pTc)
{
  // have for(int i=0;
  size_t posParen = pTc->find(")");
  size_t posColon = pTc->find(":");
  if(posParen != posColon + 2)
  {
    pTc->get(false);  // get i<N;
    pTc->get(false);  // get ++i) {
  }
}

void codeFoldingRules::MergeOperatorTokens(ITokCollection*& pTc)
{
  pTc->merge("operator","(");
}

void codeFoldingRules::doFold(ITokCollection*& pTc)
{
  if(pTc->find("for") < pTc->length())
    codeFoldingRules::MergeForCollections(pTc);
  if(pTc->find("operator") < pTc->length())
    codeFoldingRules::MergeOperatorTokens(pTc);
}