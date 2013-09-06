&func = <<function f(){}>>;
&std::print("Function: " + func.GetName() + "\n");	//requires the aspect to be woven
&func = << ~func >>;
!(func);