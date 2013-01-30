



build:
	gcc *.c -o shell


run:
	gcc *.c -o shell; ./shell

debug:
	gcc *.c -o shell -g; gdb -q shell
      

clean:
	rm shell; rm -rf *.dSYM





b: clean build
c: clean 
r: clean run 
d: clean debug 
