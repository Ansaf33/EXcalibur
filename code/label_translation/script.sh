lex labels.l
gcc lex.yy.c table.c -o program
./program ../assembly_code.xsm ../proper_assembly_code.xsm
