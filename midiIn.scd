MIDIClient.init;
m = MIDIIn.connect;

(
~noteOn = {
	|src, chan, num, vel|
	switch(num,
	36, {
		"Jonisks to red".postln;
	});
};
~noteOff = {
	|src, chan, num, vel|
	switch(num,
	36, {
		"Jonisks off".postln;
	});
};
~control = { |src, chan, num, val|
	[chan,num,val].postln;
	switch(num,
		3, {
			("Jonisk brightness: " ++ val).postln;
		}
	)
};

)
m.addFuncTo(\noteOn, ~noteOn);
m.addFuncTo(\noteOff, ~noteOff);
m.addFuncTo(\control, ~control);
