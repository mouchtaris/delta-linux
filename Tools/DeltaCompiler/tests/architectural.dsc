//Exception should be in the form: [@class : string, @data : <<...>>]
&function ArchitecturalException(exception) {
	return [
		@ex : exception,
		method Raise { return <<throw [@class : ~(@ex.class), ~(@ex.data)];>>; },
		method Ensure(condition) { return <<if (not ~condition) ~(@Raise());>>; },
		method Filter(code, filter) {
			local ex = <<$ex>>;
			//if (true)	//TODO: while(filter.has_injection_points())
				//filter.inject(ex);	//to allow filtering access the exception var with ~~ex
			return <<
				try { ~code; }
				trap ~ex { assert (~ex).class == ~(@ex.class); ~filter; throw ~ex;}
			>>;			
		},
		method Handle(code, handler) {
			return <<
				try { ~code; }
				trap $ex { assert $ex.class == ~(@ex.class); ~handler; }
			>>;
		}
	];
}

&arch = ArchitecturalException([@class : "NegativeNumberException"]);

x = 0;
!(arch.Ensure(<<x >= 0>>));
x = -1;
!(arch.Handle(
	arch.Filter(
		arch.Ensure(<<x >= 0>>),
		<<std::print("filtering code\n")>>
	),
	<<std::print("x is negative\n")>>
));

&bank = ArchitecturalException([@class : "NegativeAmount"]);
function BankAccount(initial_amount) {
	return [
		@amount : initial_amount,
		method Withdraw(amount) {
			!(bank.Ensure(<<@amount >= amount>>));
			@amount -= amount;
		},
		method CancelTransaction { std::print("Canceling transaction\n"); }
	];
}
account = BankAccount(10);
!(bank.Ensure(<<account.amount >= 0>>));
!(bank.Handle(<<account.Withdraw(100)>>, <<account.CancelTransaction()>>));
