(
var dimensions = [10, 5];
w = Window("Placement", Rect(0, 600, 1000, 500)).front;
w.view.palette_(QPalette.dark);
a = { {
	var width = w.view.bounds.width / dimensions[0];
	var height = w.view.bounds.height / dimensions[1];
	var c = View.new(w, Rect(0, 0, width, 100));
	var b = NumberBox(c,Rect(0, height*0.5 - (17.5), width * 0.9, 25)).value_(-1).background_(Color.gray).maxDecimals_(0).align_(\center);
	c.background = Color(0, 0, 0, 0.1);
	c;
} ! dimensions[0] } ! dimensions[1];
w.layout = VLayout(*a.collect { |x| HLayout(*x) });
a.flat.do({|e| e.children[0].action_(
	{
		|e|
		if(e.value >= 0, {e.background_(Color.white)}, {e.background_(Color.gray)});
		~patterns[0] = a.collect({|row, i|
			var r = row.collect({|cell| cell.children[0].value.asInteger});
			if(i.odd, {r = r.reverse});
			r;
		}).flat.select({|e| e >= 0});
		~patterns[0].postln; // Chaser per row, not scanning, continous line
		~patterns[1] = a.collect({|row, i|
			var r = row.collect({|cell| cell.children[0].value.asInteger});
			r;
		}).flat.select({|e| e >= 0});
		~patterns[1].postln; // Chaser per row, scanning
		~patterns[2] = a.flop.collect({|row, i|
			var r = row.collect({|cell| cell.children[0].value.asInteger});
			r;
		}).flat.select({|e| e >= 0});
		~patterns[2].postln; // Chaser per column, not scanning, continous line
		~patterns[3] = a.flop.collect({|row, i|
			var r = row.collect({|cell| cell.children[0].value.asInteger});
			if(i.odd, {r = r.reverse});
			r;
		}).flat.select({|e| e >= 0});
		~patterns[3].postln; // Chaser per column, scanning
})});
)