#ifndef Delegate_h__
#define Delegate_h__

// 0 parameters...
#define SUFFIX       0
#define TEMPLATE_PARAMS
#define TEMPLATE_ARGS
#define PARAMS
#define ARGS

#include "Delegate_base.h"



#undef SUFFIX
#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS



// 1 parameter...
#define SUFFIX       1
#define TEMPLATE_PARAMS  \
	, class TP1
#define TEMPLATE_ARGS    , TP1
#define PARAMS       TP1 p1
#define ARGS         p1

#include "Delegate_base.h"



#undef SUFFIX
#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS

// 2 parameters...
#define SUFFIX       2
#define TEMPLATE_PARAMS  \
	, class TP1, class TP2
#define TEMPLATE_ARGS  \
	,TP1, TP2
#define PARAMS       TP1 p1, TP2 p2
#define ARGS         p1, p2

#include "Delegate_base.h"

#undef SUFFIX
#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS


// 3 parameters...
#define SUFFIX       3
#define TEMPLATE_PARAMS  \
	, class TP1, class TP2, class TP3
#define TEMPLATE_ARGS  \
	,TP1, TP2, TP3
#define PARAMS       TP1 p1, TP2 p2, TP3 p3
#define ARGS         p1, p2, p3

#include "Delegate_base.h"

#undef SUFFIX
#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS

// 4 parameters...
#define SUFFIX 4
#define TEMPLATE_PARAMS  \
	, class TP1, class TP2, class TP3, class TP4
#define TEMPLATE_ARGS  \
	,TP1, TP2, TP3, TP4
#define PARAMS       TP1 p1, TP2 p2, TP3 p3, TP4 p4
#define ARGS         p1, p2, p3,  p4

#include "Delegate_base.h"

#undef SUFFIX
#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS


// 5 parameters...
#define SUFFIX       5
#define TEMPLATE_PARAMS  \
	, class TP1, class TP2, class TP3, class TP4, class TP5
#define TEMPLATE_ARGS  \
	,TP1, TP2, TP3, TP4, TP5
#define PARAMS       TP1 p1, TP2 p2, TP3 p3, TP4 p4, TP5 p5
#define ARGS         p1, p2, p3, p4, p5

#include "Delegate_base.h"

#undef SUFFIX
#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS




//event declarations
#define EVENT(NAME)  typedef Event0<void> ##NAME;

#define EVENT0(NAME) typedef Event0<void> ##NAME; //same as previous

#define EVENT1(NAME, P1) typedef Event1<void, P1> ##NAME;

#define EVENT2(NAME, P1, P2) typedef Event2<void, P1, P2> ##NAME;

#define EVENT3(NAME, P1, P2, P3) typedef Event3<void, P1, P2, P3> ##NAME;

#define EVENT4(NAME, P1, P2, P3, P4) typedef Event4<void, P1, P2, P3, P4> ##NAME;

#define EVENT5(NAME, P1, P2, P3, P4, P5) typedef Event5<void, P1, P2, P3, P4, P5> ##NAME;

//delegate declarations
#define DELEGATE0(NAME) typedef Delegate0<void> ##NAME;


#endif // Delegate_h__