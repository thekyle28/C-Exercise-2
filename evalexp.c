#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "evalexp.h"

struct env *env;

//environment structure, which is a list of variables, their values and their scopes.
struct env{
    char variableName[8];
    int value;
    int scope;
    struct env *next;
};

struct env *mkenv(char variableName[8], int value, int scope, struct env *next){
    struct env *env = malloc(sizeof(struct env));
    strncpy(env->variableName, variableName, 8);
    env->value = value;
    env->scope = scope;
    env->next = next;
    return env;
}

struct env *mkemptyenv()
{
    struct env *env = malloc(sizeof(struct env));
    strncpy(env->variableName, "", 8);
    env->value = 0;
    env->scope = 0;
    env->next = NULL;
    return env;
}

void add(struct env *new, struct env *orig)
{
    //ensures that the end of the list has been reached so that the new element may be added.
    while(orig->next){ //checks that the next pointer is not null.
        orig = orig->next; //if the next pointer is not null then move along the list.
    }
    orig->next = new; //assign the next value to be the new environment.
}


//a method that compares the new variable to all of the variables in the environment list to see if it is a duplicate variable name.
bool isDuplicateVar(char var[8], struct env *env){
    if(env){
        if(strcmp(var, env->variableName)==0)
            return true;
        else
            return isDuplicateVar(var, env->next);
    }
    else
        return false;
}

//returns the value for a given variable in a list of variables and values.
int getval(char var[8], int scope, struct env *env){
    if(env){ //check that the environment is not null
        //if the variable is a duplicate, check that the scope of it is correct before returning the value.
        if(isDuplicateVar(var, env)){
            if((strcmp(env->variableName, var)==0) && scope == env->scope){ //if the variable names and the scopes are the same
                return env->value; //return the value
            }
        }
        else{
            if((strcmp(env->variableName, var)==0)) //otherwise if the variable names are the same
                return env->value; //return the value
            return getval(var,scope,env->next); //check the next environment struct for the variable.
        }
    }
    else printf("%s\n", "Error variable not found.");
    return -1;
}


int evalexplist(struct explist *el, enum op op){
    if (!(el)){ //if the head is NULL i.e. the list is empty
        switch (op){
            case isplus:
                return 0; //if it is a sum operation return 0 so it doesn't affect the result.
            case ismult:
                return 1; // if it is a multiplication operation return 1 so that it doesn't affect the result.
        }
    }

    else{ //otherwise if the head isn't NULL
        switch (op){
            case isplus:
                return evalexp(el->head) + evalexplist(el->tail, op); //Add the evaluation of the head of the expression to the rest of the list.
            case ismult:
                return evalexp(el->head) * evalexplist(el->tail, op);//Multiply the evaluation of the head of the expression to the rest of the list.
        }
    }
}

int evalexp2(struct exp *e, int currentScope, struct env *env); //forward declaration so that mutual recursion can occur.

int evalexplist2(struct explist *el, int scope, enum op op, struct env *env){
    if (!(el)){ //if the head is NULL i.e. the list is empty
        switch (op){
            case isplus:
                return 0; //if it is a sum operation return 0 so it doesn't affect the result.
            case ismult:
                return 1; // if it is a multiplication operation return 1 so that it doesn't affect the result.
        }
    }

    else{ //otherwise if the head isn't NULL
        switch (op){
            case isplus:
                return evalexp2(el->head, scope, env) + evalexplist2(el->tail, scope, op, env); //Add the evaluation of the head of the expression to the rest of the list.
            case ismult:
                return evalexp2(el->head, scope, env) * evalexplist2(el->tail, scope, op, env);//Multiply the evaluation of the head of the expression to the rest of the list.
        }
    }
}

int evalexp2(struct exp *e, int currentScope, struct env *env){
    switch (e->tag){
        case isconstant:
            return e->constant;
        case isopapp :
            return evalexplist2(e->exps, currentScope, e->op, env);
        case islet: {
            struct env *newEnv;
            //create a new env for the new variable, incrementing the scope of that variable and the current scope.
            newEnv = mkenv(e->bvar, evalexp2(e->bexp, currentScope++, env), (env->scope)++, NULL);
            add(newEnv, env);
            return evalexp2(e->body, currentScope++, env);
        }

        //return the evaluated expression body along with the new environment including the new variable.
            //return evalexp2(e->body, (env->scope)++, add(newEnv,env));
        case isvar:
            return getval(e->var, currentScope, env);
    }
    printf("%s\n", "Error variable not found.");
    return -1;

}

int evalexp(struct exp *e){
        switch (e->tag){
        case isconstant:
            return e->constant;
        case isopapp :
            return evalexplist2(e->exps,1,e->op, mkemptyenv());
        case islet:
            return evalexp2(e->body, 0, mkenv(e->bvar, evalexp(e->bexp), 0, NULL));
        case isvar:
            printf("%s\n", "Error, there are no let bindings yet in the expression, so there can be no variables.");
    }
    return -1;
}