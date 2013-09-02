&&function generic_func_generator(name, args, body)
	{ return <<function ~name(~args) { ~body; }>>; }	
&!(generic_func_generator(
	<<const_generator>>,
	<<name, val>>,
	<<return <<function ~~name { return ~~val;} >>; >>
));

!(const_generator(<<one>>, <<1>>));
std::print(one(), "\n");
!(const_generator(<<two>>, <<2>>));
std::print(two(), "\n");