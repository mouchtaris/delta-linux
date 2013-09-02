&function RemoteObject () {
	return [
		method send () { throw "m1"; },
		method receive () { std::print("receiving, no throw\n"); }
	];
}
&remoteObj = RemoteObject();
&remoteObj.send();

&function StackWithDbyC () {
	return [
		method pop () { throw [@class:"ContractException"]; },
		method push () { std::print("push, no throw\n"); }
	];
}
&stack = StackWithDbyC();
&stack.push();

&function ConfigurationManager () {
	return [
		method read () { throw [@class:"IOException"]; },
		method configure () { std::print("configuring, no throw\n"); }
	];
}
&configMgr = ConfigurationManager();
& try { configMgr.read(); } trap ConfigException { std::print(ConfigException); }
