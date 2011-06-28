using std;
server_dll = dllimportdeltalib(corba::SERVER_DLL);
assert server_dll;

if (not corba::init("NameService=file://../ns.ior", "InterfaceRepository=file://../if_repo.ior"))
	print("Unable to initialize corba services\n");
else {
	function create_account(name, balance, isSavings) {
		local account_implementation = [
			@name {
				@set (method(val) { @name = val; })
				@get (method() { return @name; })
			},
			{.name : name },
			@balance {
				@set (method(val) { @balance = val; })
				@get (method() { return @balance; })
			},
			{.balance : balance },
			
			method deposit(amount) { @balance += amount; },
			method withdraw(amount) {
				if(amount > @balance)
					throw [
						{.id : "IDL:Bank/Account/Overdraft:1.0" },
						{.value: [{.reason : "Insufficient funds"}] }
					];
				else
					@balance -= amount;
			}
			
		];
		local account = nil;
		if (isSavings) {
			local savings_account_implementation = [
				method deposit(amount) {
					local newAmount = amount * 1.1;	//suppose 10% interest in every deposit
					getbase(self, 0)..deposit(newAmount);
				},
				method withdrawAll() {
					print("SavingsAccount::withdrawAll\n");
					getbase(self, 0)..balance = 0;
				}
			];
			inherit(savings_account_implementation, account_implementation);
			account = corba::server("IDL:Bank/SavingsAccount:1.0", savings_account_implementation);
		}
		else
			account = corba::server("IDL:Bank/Account:1.0", account_implementation);
		assert account;
		return account;
	}

	account_manager_implementation = [
		{.accounts : []},
		method open(name, initialBalance, isSavingsAccount) {
			if (self.accounts[name] != nil)
				return self.accounts[name];
			else {
				print("opening: ", name, ", ", initialBalance, ", ", isSavingsAccount, "\n");
				return self.accounts[name] = create_account(name, initialBalance, isSavingsAccount);
			}
		},
		
		method close(account) {
			print("close: ", account, "\n");
			local implementation = account["@server"];
			assert implementation;	//native account server
			local name = implementation.name;
			assert self.accounts[name];
			self.accounts[name] = nil;
			account.destroy();
		},
		
		method shutdown() { corba::shutdown(); }
	];
	
	account_manager = corba::server("IDL:Bank/AccountManager:1.0", account_manager_implementation);
	assert account_manager;
	
	if (not account_manager.register("AccountManager"))
		print("Unable to register server\n");
	else {
		print(account_manager.ior(), "\n");
		corba::run();
	}
	corba::cleanup();
}
dllunimport(server_dll);