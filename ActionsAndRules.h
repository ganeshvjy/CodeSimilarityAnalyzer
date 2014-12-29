#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.h - declares new parsing rules and actions     //
//  ver 2.0                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//  Platform:      Dell Precision T7400, Vista Ultimate SP1        //
//  Application:   Prototype for CSE687 Pr1, Sp09                  //
//  Author:        Jim Fawcett, CST 4-187, Syracuse University     //
//                 (315) 443-3948, jfawcett@twcny.rr.com           //
/////////////////////////////////////////////////////////////////////
//  Versio 2.1  - Added functionality to check for if,then,else    //
//              - Added functionality to check class,struct,num    //
//              - Added functionality to build XML tree            //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This module defines several action classes.  Its classes provide
specialized services needed for specific applications.  The modules
Parser, SemiExpression, and Tokenizer, are intended to be reusable
without change.  This module provides a place to put extensions of
these facilities and is not expected to be reusable.

Public Interface:
=================
Toker t(someFile);              // create tokenizer instance
SemiExp se(&t);                 // create a SemiExp attached to tokenizer
Parser parser(se);              // now we have a parser
Rule1 r1;                       // create instance of a derived Rule class
Action1 a1;                     // create a derived action
r1.addAction(&a1);              // register action with the rule
parser.addRule(&r1);            // register rule with parser
while(se.getSemiExp())          // get semi-expression
parser.parse();               //   and parse it

Build Process:
==============
Required files
- Parser.h, Parser.cpp, ScopeStack.h, ScopeStack.cpp,
ActionsAndRules.h, ActionsAndRules.cpp, ConfigureParser.cpp,
ItokCollection.h, SemiExpression.h, SemiExpression.cpp, tokenizer.h, tokenizer.cpp
Build commands (either one)
- devenv CodeAnalysis.sln
- cl /EHsc /DTEST_PARSER parser.cpp ActionsAndRules.cpp \
semiexpression.cpp tokenizer.cpp /link setargv.obj

Maintenance History:
====================
ver 2.0 : 01 Jun 11
- added processing on way to building strong code analyzer
ver 1.1 : 17 Jan 09
- changed to accept a pointer to interfaced ITokCollection instead
of a SemiExpression
ver 1.0 : 12 Jan 06
- first release

*/
//
#pragma once
#include <queue>
#include <string>
#include <sstream>
#include <algorithm>
#include "Parser.h"
#include "ITokCollection.h"
#include "ScopeStack.h"
#include "Tokenizer.h"
#include "SemiExpression.h"
#include "MTree\MTree\MNode.h"
#include "MTree\MTree\MTree.h"
#include "XmlWriter.h"
#include "AppStruct.h"

using namespace TMTree;

///////////////////////////////////////////////////////////////
// ScopeStack element is application specific


///////////////////////////////////////////////////////////////
// Repository instance is used to share resources
// among all actions.

class Repository  // application specific
{
	ScopeStack<element> stack;
	Toker* p_Toker;
public:
	Repository(Toker* pToker)
	{
		p_Toker = pToker;
	}
	ScopeStack<element>& scopeStack()
	{
		return stack;
	}
	Toker* Toker()
	{
		return p_Toker;
	}
	size_t lineCount()
	{
		return (size_t)(p_Toker->lines());
	}
};

///////////////////////////////////////////////////////////////
// rule to detect beginning of anonymous scope

class BeginningOfScope : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		//std::cout << "\n--BeginningOfScope rule";
		if (pTc->find("{") < pTc->length())
		{
			doActions(pTc);
			return false;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePush : public IAction
{
	Repository* p_Repos;
public:
	HandlePush(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		element elem;
		elem.type = "unknown";
		elem.name = "anonymous";
		elem.lineBegin = p_Repos->lineCount();
		elem.lineEnd = p_Repos->lineCount();
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect end of scope

class EndOfScope : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		//std::cout << "\n--EndOfScope rule";
		if (pTc->find("}") < pTc->length())
		{
			doActions(pTc);
			return false;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to handle scope stack at end of scope

class HandlePop : public IAction
{
	Repository* p_Repos;
	std::vector<element>scopes; //contains the list of scopes within a function
	static std::vector<treeContent>funcList;
	typedef MNode<std::string> Node;
public:
	HandlePop(){

	}
	HandlePop(Repository* pRepos)
	{
		p_Repos = pRepos;
		
	}
	std::vector<treeContent> GetTree(){
		return funcList;
	}
	void clearTree(){
		funcList.clear();
	}
	void quicksort(std::vector<element>& scopeTree, int begin, int end)
	{
		int i = begin;
		int j = end;
		int pivot = scopeTree[(begin + end) / 2].lineBegin;
		while (i <= j){
			while (scopeTree[i].lineBegin < pivot){ i++; }
			while (scopeTree[j].lineBegin > pivot){ j--; }
			if (i <= j){
				element temp = scopeTree[i];
				scopeTree[i] = scopeTree[j];
				scopeTree[j] = temp;
				i++;
				j--;
			}
		}
		if (begin < j)
			quicksort(scopeTree, begin, j);
		if (i < end)
			quicksort(scopeTree, i, end);
	}
	
//////////////////////////////////////////////////////////////////
//   to print tree for the scopes fetched

	void printTree(treeContent tC)
	{
		Node* pRoot = new Node(tC.nodeFunction);
		for (size_t i = 0; i < tC.inElements.size(); i++)
		{
			Node* pChild = new Node(tC.inElements[i].name);
			pRoot->addChild(pChild);
		}
		class newOper : public Operation<Node>
		{
		public:
			bool operator()(Node* pNode)
			{
				std::cout << "\n -- " << pNode->ToString();
				return false;
			}
		};

		MTree<Node> tree(new newOper);
		tree.makeRoot(pRoot);
		tree.verbose() = true;

		tree.walk(pRoot);

		std::cout << std::endl;

	}
	void doAction(ITokCollection*& pTc)
	{
		
		if (p_Repos->scopeStack().size() == 0)
			return;
		
		element elem = p_Repos->scopeStack().pop();
		elem.lineEnd = p_Repos->lineCount()+1;
		elem.lineCount = elem.lineEnd - elem.lineBegin;
		elem.complexity = p_Repos->scopeStack().size();
		//std::cout<<elem.show()<<std::endl;
		

		//---------------------New code - Begin-------------------
		scopes.push_back(elem);
		
		if (elem.type == "Function")
		{
			//scopes.pop_back;
			treeContent tC;
			tC.nodeFunction = elem.name;
			tC.lineCount = elem.lineEnd - elem.lineBegin;
			// Complexity Calculation 
			

			for (size_t i = 0; i < scopes.size(); i++){
				tC.inElements.push_back(scopes[i]);
			}
			tC.complexity = elem.complexity;
			if (tC.inElements.size()>2){
				quicksort(tC.inElements, 0, tC.inElements.size() - 1);
			}

			for (size_t i = 0; i < tC.inElements.size(); i++){
				int count = 0;
				size_t j = i;
				int level = tC.inElements[i].complexity;
				while ((j < tC.inElements.size() - 1) && (level < tC.inElements[++j].complexity)){
					count++;
				}
				tC.inElements[i].complexity = count;
			}
			
			funcList.push_back(tC);
			//printTree(tC)+
			;
			scopes.clear();
		}
		//---------------------New code - End --------------------
		/*if (elem.type == "Function")
		{
			/*std::cout << "\n\t\t Scope Name\tBegin\tEnd\n"
				<< "\t\t" << std::string(30, '-') << std::endl;
			std::cout << "\t\t"	<< elem.name << "\t\t" << elem.lineBegin + 1 << "\t" << elem.lineEnd + 1 << "\n";
			for (size_t i = 0; i < scopes.size(); i++){
				std::cout
					<< "\t\t" << scopes[i].name << "\t\t" << scopes[i].lineBegin + 1 << "\t" << scopes[i].lineEnd + 1 << "\n";
			}
			
			treeContent tC;
			tC.nodeFunction = elem.name;
			tC.lineCount = elem.lineEnd - elem.lineBegin;
			for (size_t i = 0; i < scopes.size(); i++){
				tC.inElements.push_back(scopes[i]);
			}
			tC.level = elem.level;
			// to print complexity only /b switch is set
			printTree(tC);
			//std::cout << "Node count:" << tC.inElements.size()+1 << std::endl;
			/*if (complexity)
			{
				std::cout << "\n\t\tFunc Name" << "|\t" << "size" << "|\tComplexity" << "\n\t\t" << std::string(30, '-') << std::endl;
				std::cout << "\t\t" << tC.nodeFunction <<"\t|"<< tC.lineCount <<"\t|"<<  tC.inElements.size()<<"\n";
			}
			
			if (tC.inElements.size() > maxLevel.inElements.size()){
				maxLevel = tC;
				
			}	
			//scopes.clear();
		}
		if ((elem.type == "class") || (elem.type == "namespace")
			|| (elem.type == "enum") || (elem.type == "struct"))
		{
		//std::cout << "\n\t\t" << elem.type + elem.name << "\t"
		//		<< elem.lineBegin + 1 << "\t" << elem.lineEnd + 1 << "\n";
			scopes.push_back(elem);
		}

		if (elem.type == "Loop")
		{
			elem.lineCount = p_Repos->lineCount() - elem.lineCount + 1;
			scopes.push_back(elem);
			
		}*/
	}
};

///////////////////////////////////////////////////////////////
// rule to detect preprocessor statements

class PreprocStatement : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		//std::cout << "\n--PreprocStatement rule";
		if (pTc->find("#") < pTc->length())
		{
			doActions(pTc);
			return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to print preprocessor statement to console

class PrintPreproc : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{

		std::cout << "\n\n  Preproc Stmt: " << pTc->show().c_str();
	}
};

///////////////////////////////////////////////////////////////
// rule to detect function definitions

class FunctionDefinition : public IRule
{
public:
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch","else","try","do" };
		for (int i = 0; i<8; ++i)
		if (tok == keys[i])
			return true;
		return false;
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.find("(");
			if (len < tc.length() && !isSpecialKeyWord(tc[len - 1]))
			{
				//std::cout << "\n--FunctionDefinition rule";
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push function name onto ScopeStack

class PushFunction : public IAction
{
	Repository* p_Repos;
public:
	PushFunction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push function scope
		std::string name = (*pTc)[pTc->find("(") - 1];
		element elem;
		elem.type = "Function";
		elem.name = name;
		elem.lineBegin = p_Repos->lineCount()+1;
		elem.lineCount = p_Repos->lineCount()+1;
		p_Repos->scopeStack().push(elem);
	}
};

/****************************/
///////////////////////////////////////////////////////////////
// rule to detect loop definitions

class LoopDefinition : public IRule
{
public:
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch","else","try","do"};
		for (int i = 0; i<8; i++)
		if (tok == keys[i])
			return true;
		return false;
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		size_t len = 0;
		if (tc[tc.length() - 1] == "{")
		{
			
			len = tc.find("(");
			if (len < tc.length() && len>0 && isSpecialKeyWord(tc[len - 1]))
			{
				//std::cout << "\n--LoopDefinition rule";
				doActions(pTc);
				return true;
			}
			else
			{
				size_t pos = tc.find("{");
				if (pos < tc.length() && ((pos==1 && (tc[pos - 1] == "else"))|| 
					(pos==2 && (tc[pos - 2] == "else")) ||
					(pos==1 && (tc[pos - 1] == "try")) || 
					(pos==2 && (tc[pos - 2] == "try")) ||
					(pos==1 && (tc[pos - 1] == "do")) || 
					(pos==2 && (tc[pos - 2] == "do")))){
					doActions(pTc);
					return true;
				}
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push Loop name onto ScopeStack

class PushLoop : public IAction
{
	Repository* p_Repos;
public:
	PushLoop(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();

		// push Loop scope
		std::string name;
		ITokCollection& tc = *pTc;
	    
		size_t pos = tc.find("(");
		size_t len = tc.length();
		if (pos == len){
			pos = tc.find("else");
			if (pos == len){
				pos = tc.find("do");
				if (pos == len){ name = "try"; }
				else { name = "do"; }
			}
			else { name = "else"; }
		}
		else
		{
			name = (*pTc)[pTc->find("(") - 1];
		}
		
		element elem;
		elem.type = "Loop";
		elem.name = name;
		elem.lineBegin = p_Repos->lineCount();
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);
	}
};

///////////////////////////////////////////////////////////////
// rule to detect class/struct/namespace/enum definitions

class UserDefinedDefinition : public IRule
{
public:
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch", "else","try" };
		for (int i = 0; i<7; ++i)
		if (tok == keys[i])
			return true;
		return false;
	}
	bool isUserDefined(const std::string& tok)
	{
		const static std::string keys[]
			= { "class","struct","namespace","enum" };
		for (int i = 0; i<4; ++i)
		if (tok == keys[i])
			return true;
		return false;
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{

			size_t len = tc.find("{");
			if (len < tc.length() && (len>=2)&& isUserDefined(tc[len - 2]))
			{
				doActions(pTc);
				return true;
			}
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////
// action to push Class/namespace/struct/enum onto ScopeStack

class PushUserDefined : public IAction 
{
	Repository* p_Repos;
public:
	PushUserDefined(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	bool isUserDefined(const std::string& tok)
	{
		const static std::string keys[]
			= { "class", "struct", "namespace", "enum" };
		for (int i = 0; i<4; ++i)
		if (tok == keys[i])
			return true;
		return false;
	}
	void doAction(ITokCollection*& pTc)
	{
		// pop anonymous scope
		p_Repos->scopeStack().pop();
		ITokCollection& tc = *pTc;
		std::string name;
		std::string type;
		// push Loop scope
		size_t len = tc.find("{");
		if (len < tc.length() && isUserDefined(tc[len - 2]))
		{
			name = tc[len - 1];
			type = tc[len - 2];

		}
		//else if (len < tc.length() && tc.find("/n") &&(len>=2)&& isUserDefined(tc[len - 3])){
		//	name = tc[len - 2];
		//	type = tc[len - 3];
		//}
		element elem;
		elem.type = type;
		elem.name = name;
		elem.lineBegin = p_Repos->lineCount();
		elem.lineCount = p_Repos->lineCount();
		p_Repos->scopeStack().push(elem);
	}
};

///////////////////////////////////////////////////////////////
// action to send semi-expression that starts a function def
// to console

class PrintFunction : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		std::cout << "\n\n  FuncDef Stmt: " << pTc->show().c_str();
	}
};

///////////////////////////////////////////////////////////////
// action to send signature of a function def to console

class PrettyPrintFunction : public IAction
{
public:
	void doAction(ITokCollection*& pTc)
	{
		pTc->remove("public");
		pTc->remove(":");
		pTc->trimFront();
		int len = pTc->find(")");
		std::cout << "\n\n  Pretty Stmt:    ";
		for (int i = 0; i<len + 1; ++i)
			std::cout << (*pTc)[i] << " ";
	}
};

#endif
