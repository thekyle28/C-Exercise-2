#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "evalexp.h"
struct env *envglb; //global variables

//environment structure, which is a list of variables and their values.
struct env{
    char var[8];
    int value;
    struct env *next;
};

//creates a new env with one variable.
struct env *mkenv(char varName[8], int value){
    struct env *env = malloc(sizeof(struct env));
    strncpy(env->var, varName, 8);
    env->value = value;
    env->next = NULL;
    return env;
}

struct env *copyEnv(struct env *env){

    // the head element
    struct env *newEnv = malloc(sizeof(struct env));
    struct env *first = newEnv; //first points to the first element in the list.
    strncpy(newEnv->var, env->var, 8);
    newEnv->value = env->value;

    //the rest of the environment
    env = env->next;
    while (env) { //makes sure that the environment isn't null
        newEnv->next = malloc(sizeof(struct env));
        newEnv = newEnv->next;
        strncpy(newEnv->var, env->var, 8);
        newEnv->value = env->value;
        env=env->next;
    }
    newEnv->next = NULL;
    return first;
} //fairly sure copy is correct.

//adds a variable to an environment and overrides it if it already exists.
void addVarOverride(char var[8], int value, struct env *env)
{
        while(env->next){ //checks that the next pointer is not null.
            if(strcmp(var, env->var)==0){ //if the variables have the same name, overwrite the variable value with the new one.
                env->value = value;
            }
                env = env->next; //if the next pointer is not null then move along the list.
        }
        //the last variable would have been reached at this point in the code.
        if(strcmp(var, env->var)==0){ //if the variables have the same name, overwrite the variable value with the new one.
            env->value = value;
        }
        else //otherwise add it to the end of the environment list.
        env->next = mkenv(var, value);

}

//adds variables without overwriting values, and doesn't add the variable if it is already in the environment.
void addVar(char var[8], int value, struct env *env)
{
    while(env->next){ //checks that the next pointer is not null.
        if(strcmp(var, env->var)==0){ //if the variables have the same name, overwrite the variable value with the new one.
            return;
        }
        env = env->next; //if the next pointer is not null then move along the list.
    }
    //the last variable would have been reached at this point in the code.
    if(strcmp(var, env->var)==0){ //if the variables have the same name, overwrite the variable value with the new one.
        return;
    }
    else //otherwise add it to the end of the environment list.
        env->next = mkenv(var, value);

}

//returns the value for a given variable in a list of variables and values.
int getval(char var[8], struct env *env){
    if(env){ //check that the environment is not null
            if(strcmp(env->var, var)==0) //if the variable names are the same
                return env->value; //return the value
        else
                return getval(var,env->next); //check the next environment struct for the variable.
    }
    else {
        char str[20];
        strcat(str, "Error variable not found");
        printf("%s\n", str);
        return -1;
    }
}

int evalexpenv(struct exp *e, struct env *env); //forward declaration so that mutual recursion can occur.

int evalexplist2(struct explist *el, enum op op, struct env *env){
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
                //Add the evaluation of the head of the expression to the rest of the list.
                return evalexpenv(el->head, env) + evalexplist2(el->tail, op, env);
            case ismult:
                //Multiply the evaluation of the head of the expression to the rest of the list.
                return evalexpenv(el->head, env) * evalexplist2(el->tail, op, env);
        }
    }
}

int evalexpenv(struct exp *e, struct env *env){
    if (!e) return 0;
    switch (e->tag){
        case isconstant:
            return e->constant;
        case isopapp :{
            return evalexplist2(e->exps, e->op, env);
        }
        case islet: {
            //copy the environment and pass it to the
            struct env *copy;
            copy = copyEnv(env);
            addVarOverride(e->bvar,evalexpenv(e->bexp,copy),copy);
            addVar(e->bvar,evalexpenv(e->bexp,env),env);
            return evalexpenv(e->body, copy);
        }
        case isvar:
            return getval(e->var, env);
    }
    printf("%s\n", "Type not found.");
    return -1;

}

int evalexp(struct exp *e){
    envglb = mkenv("",0);//initialise an environment
    return evalexpenv(e, envglb);
}