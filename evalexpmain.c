// main function for evaluation expression exercise

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "evalexp.h"

// tree construction functions

struct exp *mkvar(char *cp)
{
    struct exp *ep = malloc(sizeof(struct exp));
    ep->tag = isvar;
    strncpy(ep->var, cp, 8);
    return ep;
};

struct exp *mkconstant(int n)
{
    struct exp *ep = malloc(sizeof(struct exp));
    ep->tag = isconstant;
    ep->constant = n;
    return ep;
};

struct exp *mkopapp(enum op op, struct explist *el)
{
    struct exp *ep = malloc(sizeof(struct exp));
    ep->tag = isopapp;
    ep->op =  op;
    ep->exps =  el;
    return ep;
};

struct exp *mklet(char *cp, struct exp *e1, struct exp *e2)
{
    struct exp *ep = malloc(sizeof(struct exp));
    ep->tag = islet;
    strncpy(ep->bvar, cp, 8);
    ep->bexp = e1;
    ep->body = e2;
    return ep;
};

struct explist *cons(struct exp *e, struct explist *l)
{
    struct explist *l2 = malloc(sizeof(struct explist));
    l2->head = e;
    l2->tail = l;
    return l2;
}

// main for testing

int main(int argc, const char * argv[])
{
    struct exp *e1, *e2, *e3, *e4, *e5;
    struct explist *l;
    l = NULL;
    l = cons(mkconstant(5), l);
    l = cons(mkconstant(3), l);
    l = cons(mkconstant(2), l);
    e1 = mkopapp(isplus, l);
    l = NULL;
    l = cons(mkvar("y"), l);
    l = cons(mkvar("x"), l);
    l = cons(mkvar("z"), l);
    e2 = mkopapp(ismult, l);
    e3 = mklet("x", e1, e2);
    e4 = mklet("y", mkconstant(5), e3);
    e5 = mklet("z", mkconstant(40), e4);
    printf("%d\n", evalexp(e5)); // should print 2000

    struct exp *e6, *e7;

    l = NULL;
    l = cons(mkconstant(5), l);
    l = cons(mkconstant(3), l);
    l = cons(mkconstant(2), l);
    e6 = mkopapp(isplus, l);

    l = NULL;
    l = cons(mkvar("x"), l);
    l = cons(mkvar("x"), l);
    l = cons(mkvar("x"), l);
    e7 = mkopapp(ismult, l);

    e6 = mklet("x", e6, e7);

    printf("%d\n", evalexp(e6)); // should print  1000

// let x = (let x = 2 in x + 3) in x + (let x = 4 + x in x + 9) + x
    struct exp *e8, *e9, *e10, *e11, *e12, *e13,*e14;

    l = NULL;
    l = cons(mkconstant(4), l);
    l = cons(mkvar("x"), l);
    e8 = mkopapp(isplus, l); // 4 + x
    l = NULL;
    l = cons(mkvar("x"), l);
    l = cons(mkconstant(9), l);
    e9 = mkopapp(isplus, l); // x + 9
    e10 = mklet("x", e8, e9); // let x = 4 + x in x + 9

    l = NULL;
    l = cons(mkvar("x"), l);
    l = cons(e10, l);
    l = cons(mkvar("x"), l);
    e11 = mkopapp(isplus, l); // x + (let x = 4 + x in x + 9) + x

    l = NULL;
    l = cons(mkconstant(3), l);
    l = cons(mkvar("x"), l);
    e12 = mkopapp(isplus,l); // x + 3
    e13 = mklet("x", mkconstant(2), e12); // let x = 2 in x + 3

    e14 = mklet("x", e13, e11); // let x = (let x = 2 in x + 3) in x + (let x = 4 + x in x + 9) + x
    printf("%d\n", evalexp(e14)); // should print 28

    return 0;
}

