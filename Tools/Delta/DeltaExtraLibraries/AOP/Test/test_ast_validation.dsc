&x = <<1>>;
&y = <<~x + 2>>;
&function identity(ast) { return ast; }
std::print(!(identity(y)));