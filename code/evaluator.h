#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <stdio.h>
#include <stdlib.h>
#include "AST.h"

void evaluate(struct TreeNode* root);

bool boolean_expressionEvaluator(struct TreeNode* root);

int arithmetic_expressionEvaluator(struct TreeNode* root);

int getDetails(void);

#endif
