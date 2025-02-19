lex lexical.l
yacc -d parser.y
gcc AST.c reghandling.c symbol_table/Gsymbol.c symbol_table/varList.c operators/optrans.c map_implementation/map.c y.tab.c lex.yy.c -o program
./program input.txt assembly_code.xsm
cd label_translation
./script.sh
cd ../../xsm_expl
./xsm -l library.lib -e ../code/proper_assembly_code.xsm --debug


