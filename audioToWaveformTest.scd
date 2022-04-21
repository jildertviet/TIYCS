t = TIYCS.new.gui
b = Buffer.alloc(s, 400);
x.free; x = {var input = CombC.ar(LPF.ar(SoundIn.ar(0), 10000), 0.2, 0.01, 1); RecordBuf.ar(Lag.ar(input, 0.00001), b.bufnum)}.play;

(
~sendVolume.stop; ~sendVolume = {
	inf.do{
		b.getToFloatArray(count: -1, action:{
			arg array;
			var toSendA, toSendB;

			var aArray = array.at((0..floor(array.size/2)-1));
			var bArray = array.at(((array.size/2) .. (array.size-1)));

			toSendA = Array.newFrom(aArray).collect({|e|e.asBytes}).reshape(aArray.size * 8).asInteger;
			toSendB = Array.newFrom(bArray).collect({|e|e.asBytes}).reshape(bArray.size * 8).asInteger;
			// array.postln;
			toSendA = Int8Array.newFrom(toSendA);
			toSendB = Int8Array.newFrom(toSendB);
			// toSend.postln;
			t.screens[0].sendMsg("/setWave", toSendA);
			t.screens[2].sendMsg("/setWave", toSendB);
		});
		(1/30).wait;
	}
	}.fork;
);

x.free; ~sendVolume.stop;

// x = {inf.do{t.setBus(3, 0.5 + 1.0.rand, [0,2]); 0.1.wait}}.fork;