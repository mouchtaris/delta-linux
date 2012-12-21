using std;
//&using #lala;	//recursive build error
&function generate_function_call(func, args)
	{ return <<~func(~args)>>; }

&function generate_next_stage_code(code)
	{ return << !(<<(~code)>>) >>; }

function f(x, y) { return x + y; }

!(generate_next_stage_code(generate_function_call(<<g>>, <<1, 2>>)));
//no function g available (typed g instead of f) - stage result error
//&lala();	//stage compilation error