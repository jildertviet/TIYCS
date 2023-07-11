(
Pdef(\joniskTestPattern,
	Pbind(\type, \triggerJonisk,
		\jonisk, Pseq(~j.jonisks.at(~j.patterns[0]), inf),
		\delta, 0.5
	)
).quant_([1]).play
)



Pdef(\joniskTestPattern).stop;