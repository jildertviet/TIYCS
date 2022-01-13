SerialPort.listDevices
p = SerialPort.new("/dev/cu.wchusbserial1420", 230400, crtscts: true);
// p = SerialPort.new("/dev/tty.wchusbserial14120", 230400, crtscts: true);
p.close

(
~numJonisk = 16;
~values = Bus.alloc(\control, s, ~numJonisk * 4);
SynthDef(\jonisk, {
	|b=1, rgb=#[1,1,1], mode=0, out=0, rgbAdd=#[0,0,0], curve = -4, trigRand=1, a=0.1, s=1.0, r=1.0, gate=0, level=1, noiseMul=0, lagTime=0.01|
	var output;
	var env = EnvGen.kr(Env.linen(a, s, r, level, curve: curve), Changed.kr(gate));
	var brightness = Mix.kr([env, b]).min(1);
	output = (Lag2.kr(brightness , lagTime) + LFDNoise1.ar(1/4, noiseMul).abs).min(1) * rgb;
	output = (output + rgbAdd).min(1);
	Out.kr(out, output ++ 0); // Also add an empty white channel
}).add;
)

(
j.do{|e| e.free};
j = Array.fill(~numJonisk, {|i| Synth(\jonisk, [out: ~values.subBus(i*4)])});

( // GBR
~updateJonisk.stop; ~updateJonisk = {
	inf.do{
		~values.getn(~numJonisk * 4, {|v|
			// v.postln;
			var g = v[0];
			var r = v[1];
			var b = v[2];
			var w = 0;
			var end = [101,110,100];
			var msg = (0xFF!6) ++ [0x05] ++ ((v*255).asInteger) ++ end;
			msg.postln;
			p.putAll(msg);
		});
		(1/30).wait;
	}
}.fork;
);

~setBrightness = {
	|id, b|
	if(id == -1, {
		j.do{|j| j.set(\b, b)}
	}, {
		j[id].set(\b, b);
	});
	// ~joniskBrightness.value = b;
};
~setRGB = {
	|id, rgb|
	if(id == -1, {
		j.do{|j| j.set(\rgb, rgb)};
	}, {
		j[id].set(\rgb, rgb);
	});
};
~setRGBAdd = {
	|id, rgb|
	if(id == -1, {
		j.do{|j| j.set(\rgbAdd, rgb)};
	}, {
		j[id].set(\rgbAdd, rgb);
	});
};
~doEnv = {
	|id, a, s, r, level=1|
	if(id == -1, {
		j.do{|j| j.set(\a, a, \s, s, \r, r, \level, level, \gate, 1.0.rand);}
	}, {
		j[id].set(\a, a, \s, s, \r, r, \level, level, \gate, 1.0.rand);
	});
};
)


(
x.stop;
x = {
	inf.do{
		var c = Color.rand;
		c = c.toJV;
		c[3] = 250;
		p.putAll((0xFF!6) ++ [0x05] ++ c ++ [101,110,100]);
		0.4.wait;
	}
}.fork;
)

p.putAll((0xFF!6) ++ [0x05] ++ [255, 255, 0, 10] ++ [101,110,100])
p.putAll((0xFF!6) ++ [0x05] ++ [255, 255, 255, 255] ++ [101,110,100])
p.putAll((0xFF!6) ++ [0x05] ++ [0,0,0, 255] ++ [101,110,100])
p.putAll((0xFF!6) ++ [0x05] ++ (0!4) ++ [101,110,100])

// #################################################################################################### TEST
~setBrightness.value(-1, 1);
~setBrightness.value(-1, 0);

~setRGB.value(-1, Color.white.asArray);
~setRGB.value(-1, Color.blue.asArray);
~setRGB.value(-1, [254, 103/3,92/2] / 254); // PINK

~setRGBAdd.value(-1, Color.white.asArray);
~doEnv.value(-1, 0.1, 0.2, 0.4, level: 1.0);

~setRGB.value(-1, Color.white.asArray);

j.do{|e| e.set(\b, 0.1, \noiseMul, 0)}; // Glow Jonisk

j[2].set(\b, 1)
j[2].set(\b, 0)

j[15].set(\a, 0.1, \s, 0.1, \r, 0.5, \level, 1, \gate, 1.0.rand);
(
~blinkTest .stop;
~blinkTest = {
	inf.do{
		|i|
		var index = i % 16;
		j[index.asInteger].set(\a, 0.1, \s, 0.1, \r, 0.5, \level, 1, \gate, 1.0.rand);
		0.1.wait;
	}
}.fork;
)

~setRGB.value(-1, Color.white.asArray);

(
	~setMode.value(9, -1); ~screens.do{|screen|screen.sendMsg("/setValueById", 1, -1)}; // Benzine
~blink.stop;
~setBrightness.value(-1, 1);
~blink = {
	inf.do{
		~setRGB.value(-1, Color.red.asArray);
		0.5.wait;
		~setRGB.value(-1, Color.black.asArray);
		1.wait;
	}
}.fork
)