(
var dimensions = [10, 5];
~patterns = 0!4;
~placement = (-1!dimensions[0])!dimensions[1];
w = Window().front;
w.view.palette_(QPalette.dark);
a = { { NumberBox(w).value_(-1).background_(Color.gray).maxDecimals_(0)} ! dimensions[0] } ! dimensions[1];
w.layout = VLayout(*a.collect { |x| HLayout(*x) });
a.flat.do({|e| e.action_(
	{
		|e|
		if(e.value >= 0, {e.background_(Color.white)}, {e.background_(Color.gray)});
		~patterns[0] = a.collect({|row, i|
			var r = row.collect({|cell| cell.value.asInteger});
			if(i.odd, {r = r.reverse});
			r;
		}).flat.select({|e| e >= 0});
		~patterns[0].postln; // Chaser per row, not scanning, continous line
		~patterns[1] = a.collect({|row, i|
			var r = row.collect({|cell| cell.value.asInteger});
			r;
		}).flat.select({|e| e >= 0});
		~patterns[1].postln; // Chaser per row, scanning
		~patterns[2] = a.flop.collect({|row, i|
			var r = row.collect({|cell| cell.value.asInteger});
			r;
		}).flat.select({|e| e >= 0});
		~patterns[2].postln; // Chaser per column, not scanning, continous line
		~patterns[3] = a.flop.collect({|row, i|
			var r = row.collect({|cell| cell.value.asInteger});
			if(i.odd, {r = r.reverse});
			r;
		}).flat.select({|e| e >= 0});
		~patterns[3].postln; // Chaser per column, scanning
})});
)


([
	[0, 1],
	[2, 3]
].flop)