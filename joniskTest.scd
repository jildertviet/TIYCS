SerialPort.listDevices
p = SerialPort.new("/dev/cu.wchusbserial1420", 230400, crtscts: true);
p.close

~numJonisk = 4;
~values = Bus.alloc(\control, s, ~numJonisk * 3);
~values.setn([0, 100, 0, 0]);

( // GBR
x.stop;
x = {
	inf.do{
		~values.getn(4, {|v|
			var g = v[0];
			var r = v[1];
			var b = v[2];
			var w = v[3];
			var mode = 0;
			p.putAll([0, g, b, r, w, mode, 0, 255]);
			"send".postln;
		});
		(1/30).wait;
	}
}.fork
)


y.free; y = {Out.kr(~values, [114,0,154,254] * SinOsc.kr(1/12).abs.pow(3).max(0.5))}.play;
~values.setn(0!4)
~values.setn([114,0,154,254].rotate(0))

~values.setn([0,0,254,0].rotate(0))

0 = B
2 = G
3 = R



~values.setn([103/2,0,92/2,254]); // PINK
~values.getn(4, {1.post});
// Id, R, G, B, W, mode, lagTime, stopByte


j = JPontLights.new(p)
j.sendS(0, 200, 100, 100, 0);

(
var addr = 0xFF!6;
var end = [101,110,100];
// p.putAll(addr ++ [0x05] ++ [0x01, 0x10, 0x00, 0x00] ++ end);
p.putAll(addr ++ [0x07] ++ [0x00] ++ end);
)