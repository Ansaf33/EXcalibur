# EXcalibur
Optimising the Compiler created for the EXPL language using Node Minimisation and LRU Caching for Registers with stored subexpression results.

## Common Sub-expression Elimination:

The expression or sub-expression that has been appeared and computed before and appears again during the computation of the code is the common sub-expression. Elimination of that sub-expression is known as Common sub-expression elimination.

The advantage of this elimination method is to make the computation faster and better by avoiding the re-computation of the expression. In addition, it utilizes memory efficiently.

### Types of common sub-expression elimination

The two types of elimination methods in common sub-expression elimination are:

1. **Local Common Sub-expression elimination**– It is used within a single basic block. Where a basic block is a simple code sequence that has no branches.

2. **Global Common Sub-expression elimination**– It is used for an entire procedure of common sub-expression elimination.

#### References
- GFG Article on CSE (https://www.geeksforgeeks.org/common-subexpression-elimination-code-optimization-technique-in-compiler-design/)
- My Github Repository (https://github.com/Ansaf33/CompilerLab) for original unoptimized code.
- ALSU 6.2.1 for more details on 3-address code format
- ALSU 8.5 for details on local code optimization.
