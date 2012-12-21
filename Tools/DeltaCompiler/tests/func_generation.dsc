function generic_func_generator(name, args, body)
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

&function const_func_generator2(name, val)
	{ return generic_func_generator(name, nil, <<return ~val;>>); }

!(generic_func_generator(<<add>>, <<x, y>>, <<return x + y;>>));
!(const_func_generator2(<<ten>>, <<10>>));
!(const_func_generator2(<<twenty>>, <<20>>));
std::print(add(1, 2), " ", ten(), " ", twenty(), "\n");