#include "abquick.h"

ABQK_DEFINE0(sequence0, abNewSequence0);
ABQK_DEFINE1(sequence1, abNewSequence1);
ABQK_DEFINE0(comma0, abNewComma0);
ABQK_DEFINE1(comma1, abNewComma1);
ABQK_DEFINE2(comma2, abNewComma2);
ABQK_DEFINE0(nothing, abNewNothing);
ABQK_DEFINE2(define, abNewDefine);
ABQK_DEFINE2(declare, abNewDeclare);
ABQK_DEFINE2(with, abNewWith);
ABQK_DEFINE2(add, abNewAdd);
ABQK_DEFINE2(label, abNewLabel);
ABQK_DEFINE2(has, abNewHas);
ABQK_DEFINE2(apply1, abNewApply1);
ABQK_DEFINE3(apply2, abNewApply2);
ABQK_DEFINE3(lambda, abNewLambda);
ABQK_DEFINE3(_if, abNewIf);
ABQK_DEFINE2(import, abNewImport);

ABQK_DEFINE1_Symbol(id, abNewId);

AbSyn emptyAdd() 
{
	return add(nothing(), nothing());
}

AbSyn emptyWith() 
{
	return with(nothing(), nothing());
}

AbSyn typeDecl(String name)
{
	return declare(id(name), id("Type"));
}

